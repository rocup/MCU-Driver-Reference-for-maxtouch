/*
* Atmel maXTouch Touchscreen driver
*
* Copyright (C) 2019- Microchip Corporation
*
* Author: Rocup Wan <rocup.wan@microchip.com>
*
* This program is free software; you can redistribute  it and/or modify it
* under  the terms of  the GNU General  Public License as published by the
* Free Software Foundation;  either version 2 of the  License, or (at your
* option) any later version.
*
*/

/*************************************************************************************
 ***********************************Modify Note***************************************
 You need to add or modify the system head file according to your MCU and compiler
 *************************************************************************************/
#include <errno.h>         /* use the error type */
#include <stdbool.h>       /* use the bool type */
#include "atmel_mxt_ts.h"
#include "mcu_interface.h"

/* maxtouch data read from IC or need to be initialized */
static struct mxt_data mxtData;        

static size_t mxt_obj_size(const struct mxt_object *obj)
{
	return obj->size_minus_one + 1;
}

static size_t mxt_obj_instances(const struct mxt_object *obj)
{
	return obj->instances_minus_one + 1;
}

static int mxt_lookup_bootloader_address(struct mxt_data *data, bool retry)
{
	u8 appmode = data->i2c_addr;
	u8 bootloader;
	u8 family_id = data->info.family_id;

	switch (appmode) {
	case 0x4a:
	case 0x4b:
		/* Chips after 1664S use different scheme */
		if (retry || family_id >= 0xa2) {
			bootloader = appmode - 0x24;
			break;
		}
		/* Fall through for normal case */
	case 0x4c:
	case 0x4d:
	case 0x5a:
	case 0x5b:
		bootloader = appmode - 0x26;
		break;

	default:
		dev_err("Appmode i2c address 0x%02x not found\n", appmode);
		return -EINVAL;
	}

	data->bootloader_addr = bootloader;
	return 0;
}

static int mxt_probe_bootloader(struct mxt_data *data, bool alt_address)
{
	int error;
	u8 buf[3];
	bool crc_failure, extended_id;

	error = mxt_lookup_bootloader_address(data, alt_address);
	if (error)
		return error;

	/* Check bootloader status and version information */
	error = mxt_bootloader_read(data->bootloader_addr, buf, sizeof(buf));
	if (error)
		return error;

	crc_failure = (buf[0] & ~MXT_BOOT_STATUS_MASK) == MXT_APP_CRC_FAIL;
	extended_id = buf[0] & MXT_BOOT_EXTENDED_ID;

	dev_info("Found bootloader addr:%02x ID:%u%s%u%s\n",
		 data->bootloader_addr,
		 extended_id ? (buf[1] & MXT_BOOT_ID_MASK) : buf[0],
		 extended_id ? " version:" : "",
		 extended_id ? buf[2] : 0,
		 crc_failure ? ", APP_CRC_FAIL" : "");

	return 0;
}

int mxt_send_bootloader_cmd(struct mxt_data *data, bool unlock)
{
	int ret;
	u8 buf[2];

	if (unlock) {
		buf[0] = MXT_UNLOCK_CMD_LSB;
		buf[1] = MXT_UNLOCK_CMD_MSB;
	} else {           /* non-zero and not unlock value, used to quit the bootloader mode */
		buf[0] = 0x01;
		buf[1] = 0x01;
	}

	ret = mxt_bootloader_write(data->bootloader_addr, buf, 2);
	if (ret)
		return ret;

	return 0;
}

static void mxt_proc_t6_messages(struct mxt_data *data, u8 *msg)
{
	u8 status = msg[1];
	u32 crc = msg[2] | (msg[3] << 8) | (msg[4] << 16);

	if (crc != data->config_crc) {
		data->config_crc = crc;
		dev_dbg("T6 Config Checksum: 0x%06X\n", crc);
	}

	data->crc_completion = 1;

	/* Detect reset */
	if (status & MXT_T6_STATUS_RESET)
		data->reset_completion = 1;

	/* Output debug if status has changed */
	if (status != data->t6_status)
		dev_dbg("T6 Status 0x%02X%s%s%s%s%s%s%s\n",
			status,
			status == 0 ? " OK" : "",
			status & MXT_T6_STATUS_RESET ? " RESET" : "",
			status & MXT_T6_STATUS_OFL ? " OFL" : "",
			status & MXT_T6_STATUS_SIGERR ? " SIGERR" : "",
			status & MXT_T6_STATUS_CAL ? " CAL" : "",
			status & MXT_T6_STATUS_CFGERR ? " CFGERR" : "",
			status & MXT_T6_STATUS_COMSERR ? " COMSERR" : "");

	/* Save current status */
	data->t6_status = status;
}

#ifdef T9_OBJECT
static void mxt_proc_t9_message(struct mxt_data *data, u8 *message)
{
	int id;
	int x;
	int y;
	u8 status;
	u8 type = 0;

	id = message[0] - data->T9_reportid_min;
	status = message[1];
	x = (message[2] << 4) | ((message[4] >> 4) & 0xf);
	y = (message[3] << 4) | ((message[4] & 0xf));

	/* Handle 10/12 bit switching */
	if (data->max_x < 1024)
		x >>= 2;
	if (data->max_y < 1024)
		y >>= 2;

	dev_dbg(
		"[%u] %c%c%c%c%c%c%c%c x: %5u y: %5u\n",
		id,
		(status & MXT_T9_DETECT) ? 'D' : '.',
		(status & MXT_T9_PRESS) ? 'P' : '.',
		(status & MXT_T9_RELEASE) ? 'R' : '.',
		(status & MXT_T9_MOVE) ? 'M' : '.',
		(status & MXT_T9_VECTOR) ? 'V' : '.',
		(status & MXT_T9_AMP) ? 'A' : '.',
		(status & MXT_T9_SUPPRESS) ? 'S' : '.',
		(status & MXT_T9_UNGRIP) ? 'U' : '.',
		x, y);

	if (status & MXT_T9_DETECT) {
		/*
		 * Multiple bits may be set if the host is slow to read
		 * the status messages, indicating all the events that
		 * have happened.
		 */
		if (status & MXT_T9_RELEASE) {
			dev_dbg("[%u] release\n", id);

			/* report type and status */
			report_state(type, status);
		}

        dev_dbg("[%u] type:%u x:%u y:%u \n", id, status, x, y);

        /* report type event and coordinate */
        report_state(type, status);
        report_coordinate(x, y);

	} else {
		/* Touch no longer active, close out slot */
		dev_dbg("[%u] release\n", id);

		/* report type and event */
		report_state(type, status);
	}
}
#else
static void mxt_proc_t100_message(struct mxt_data *data, u8 *message)
{
	int id;
	u8 status;
	u8 type = 0;
	u8 event = 0;
	u16 x;
	u16 y;
	bool active = false;

	id = message[0] - data->T100_reportid_min - 2;

	/* ignore SCRSTATUS events */
	if (id < 0)
		return;

	status = message[1];
	event = status & MXT_T100_EVENT_MASK;
	x = message[2] + (message[3] << 8);
	y = message[4] + (message[5] << 8);

	if (status & MXT_T100_DETECT) {
		type = (status & MXT_T100_TYPE_MASK) >> 4;

		switch (type) {
		case MXT_T100_TYPE_HOVERING_FINGER:
		case MXT_T100_TYPE_FINGER:
		case MXT_T100_TYPE_GLOVE:
		case MXT_T100_TYPE_PASSIVE_STYLUS:
		case MXT_T100_TYPE_ACTIVE_STYLUS:
			active = true;
			break;

		case MXT_T100_TYPE_LARGE_TOUCH:
			/* Ignore suppressed touch */
			break;

		default:
			dev_dbg("Unexpected T100 type\n");
			return;
		}
	}

	if (active) {
		dev_dbg("[%u] type:%u x:%u y:%u \n", id, type, x, y);

        /* report type event and coordinate */
		report_state(type, event);
		report_coordinate(x, y);

	} else {
		dev_dbg("[%u] release\n", id);

		/* report type and event */
		report_state(type, event);
	}
}
#endif

#ifdef TOUCH_KEY
static void mxt_proc_t15_messages(struct mxt_data *data, u8 *msg)
{
	int key;
	bool curr_state, new_state;
	unsigned long keystates = msg[2] + (msg[3] << 8);
	int id = msg[0] - data->T15_reportid_min;
	
	if(id)
	    keystates <<= data->t15_keys_instance0;

	for (key = 0; key < data->t15_num_keys; key++) {
		curr_state = TEST_BIT(key, &data->t15_keystatus);
		new_state = TEST_BIT(key, &keystates);

		if (!curr_state && new_state) {
			dev_dbg("T15 key press: %u\n", key);
			data->t15_keystatus |= (1U << key);
			
			report_key(data->t15_keystatus, 1);
		
		} else if (curr_state && !new_state) {
			dev_dbg("T15 key release: %u\n", key);
			data->t15_keystatus &= ~(1U << key);
			
		    report_key(data->t15_keystatus, 0);
		}
	}
}
#endif

static int mxt_proc_message(struct mxt_data *data, u8 *message)
{
	u8 report_id = message[0];

	if (report_id == MXT_RPTID_NOMSG)
		return 0;

	if (report_id == data->T6_reportid) {
		mxt_proc_t6_messages(data, message);
#ifdef T9_OBJECT
    } else if (report_id >= data->T9_reportid_min &&
            report_id <= data->T9_reportid_max) {
	    mxt_proc_t9_message(data, message);
#else
	} else if (report_id >= data->T100_reportid_min &&
		   report_id <= data->T100_reportid_max) {
		mxt_proc_t100_message(data, message);
#endif

#ifdef TOUCH_KEY
	} else if (report_id >= data->T15_reportid_min
		   && report_id <= data->T15_reportid_max) {
		mxt_proc_t15_messages(data, message);
#endif
	}
	
	return 1;
}

static int mxt_read_and_process_messages(struct mxt_data *data, u8 count)
{
	int ret;
	int i;
	u8 num_valid = 0;

	/* Safety check for msg_buf */
	if (count > data->max_reportid)
		return -EINVAL;

	for (i = 0;  i < count; i++) {
		/* Process remaining messages if necessary */
		ret = mxt_read_reg(data->i2c_addr, data->T5_address, data->T5_msg_size, data->msg_buf);
		if (ret) {
			dev_err("Failed to read %u messages (%d)\n", 1, ret);
			return ret;
		}
		
		ret = mxt_proc_message(data, data->msg_buf);

		if (ret == 1)
			num_valid++;
	}

	/* return number of messages read */
	return num_valid;
}

#ifndef T44_NONE
static void mxt_process_messages_t44(struct mxt_data *data)
{
	int ret;
	u8 count, num_left;

	/* Read T44 firstly */
	ret = mxt_read_reg(data->i2c_addr, data->T44_address, 1, data->msg_buf);
	if (ret) {
		dev_err("Failed to read T44 (%d)\n", ret);
		return;
	}

	count = data->msg_buf[0];

	/*
	 * This condition may be caused by the CHG line being configured in
	 * Mode 0. It results in unnecessary I2C operations but it is benign.
	 */
	if (count == 0)
		return;

	if (count > data->max_reportid) {
		dev_warn("T44 count %d exceeded max report id\n", count);
		count = data->max_reportid;
	}

	num_left = count;

	/* Process remaining messages if necessary */
	if (num_left) {
		ret = mxt_read_and_process_messages(data, num_left);
		if (ret < 0)
			return;
		else if (ret != num_left)
			dev_warn("Unexpected invalid message\n");
	}

	return;
}
#endif

static int mxt_process_messages_until_invalid(struct mxt_data *data)
{
	int count, read;
	u8 tries = 2;

	count = data->max_reportid;

	/* Read messages until we force an invalid */
	do {
		read = mxt_read_and_process_messages(data, count);
		if (read < count)
			return 0;
	} while (--tries);

	dev_err("CHG pin isn't cleared\n");
	return -EBUSY;
}

#ifdef T44_NONE
static void mxt_process_messages(struct mxt_data *data)
{
	int total_handled, num_handled;
	u8 count = data->last_message_count;

	if (count < 1 || count > data->max_reportid)
		count = 1;

	/* include final invalid message */
	total_handled = mxt_read_and_process_messages(data, count + 1);
	if (total_handled < 0)
		return;
	/* if there were invalid messages, then we are done */
	else if (total_handled <= count)
		goto update_count;

	/* keep reading two msgs until one is invalid or reportid limit */
	do {
		num_handled = mxt_read_and_process_messages(data, 2);
		if (num_handled < 0)
			return;

		total_handled += num_handled;

		if (num_handled < 2)
			break;
	} while (total_handled < data->num_touchids);

update_count:
	data->last_message_count = total_handled;

	return;
}
#endif

static void mxt_interrupt(void)
{
	struct mxt_data *data = &mxtData;

	data->chg_completion = 1;
	
    dev_dbg("interrupt\n");

	if (data->in_bootloader) 
		return;
	
#ifndef T44_NONE
	if (data->T44_address) 
		return mxt_process_messages_t44(data);
#else
	return mxt_process_messages(data);
#endif
}

static int mxt_t6_command(struct mxt_data *data, u16 cmd_offset, u8 value, bool wait)
{
	u16 reg;
	u8 command_register;
	int timeout_counter = 0;
	int ret;

	reg = data->T6_address + cmd_offset;
	
	ret = mxt_write_reg(data->i2c_addr, reg, 1, &value);
	if (ret)
		return ret;

	if (!wait)
		return 0;

    /* For the command, if it executed successfully, the command value will be zero*/
	do {
		msleep(20);
		ret = mxt_read_reg(data->i2c_addr, reg, 1, &command_register);
		if (ret)
			return ret;
	} while (command_register != 0 && timeout_counter++ <= 100);

	if (timeout_counter > 100) {
		dev_err("Command failed!\n");
		return -EIO;
	}

	return 0;
}

static int mxt_soft_reset(struct mxt_data *data)
{
	int ret = 0;

	dev_info("Resetting device\n");

	disable_irq(data->irq);

	data->reset_completion = 1;

	ret = mxt_t6_command(data, MXT_COMMAND_RESET, MXT_RESET_VALUE, false);
	if (ret)
		return ret;

	/* Ignore CHG line for 100ms after reset */
	msleep(100);

	enable_irq(data->irq);

	ret = mxt_wait_for_completion(&data->reset_completion, MXT_RESET_TIMEOUT);
	if (ret)
		return ret;

	return 0;
}

static void mxt_update_crc(struct mxt_data *data, u8 cmd, u8 value)
{
	/*
	 * On failure, CRC is set to 0 and config will always be
	 * downloaded.
	 */
	data->config_crc = 0;
	data->crc_completion = 0;

	mxt_t6_command(data, cmd, value, true);

	/*
	 * Wait for crc message. On failure, CRC is set to 0 and config will
	 * always be downloaded.
	 */
	mxt_wait_for_completion(&data->crc_completion, MXT_CRC_TIMEOUT);
}

static void mxt_calc_crc24(u32 *crc, u8 firstbyte, u8 secondbyte)
{
	static const unsigned int crcpoly = 0x80001B;
	u32 result;
	u32 data_word;

	data_word = (secondbyte << 8) | firstbyte;
	result = ((*crc << 1) ^ data_word);

	if (result & 0x1000000)
		result ^= crcpoly;

	*crc = result;
}

static void mxt_calculate_crc(u8 *base, u32 *crc, struct crc_control *crc_ctrl)
{
	u8 *ptr = base + crc_ctrl->start_off;
	u8 *last_val = base + crc_ctrl->end_off - 1;

    if (crc_ctrl->end_off < crc_ctrl->start_off)
        return;

    if (crc_ctrl->last_odd == true) {   /* last data frame is odd, this time calculate the last data */
	    mxt_calc_crc24(crc, crc_ctrl->last_data, *ptr);
		ptr += 1;
		crc_ctrl->last_odd = false;
	}

	while (ptr < last_val) {
		mxt_calc_crc24(crc, *ptr, *(ptr + 1));
		ptr += 2;
	}

	/* if len is odd, fill the last byte with 0 when it's the end of the whole data frame, 
	   otherwise store the last data */
	if (ptr == last_val) {
		if (crc_ctrl->data_end == true)
		    mxt_calc_crc24(crc, *ptr, 0);
		else {
		    crc_ctrl->last_data = *ptr;
			crc_ctrl->last_odd = true;
		}
	}
	
	/* Mask to 24-bit */
	*crc &= 0x00FFFFFF;
}

static int mxt_check_retrigen(struct mxt_data *data)
{
	int error;
	u8 val;
	
	/* If it's low level trigger, it will not need retrigger */
	if (check_low_level_trigger() == true)
	    return 0;
		
	if (data->T18_address) {
		error = mxt_read_reg(data->i2c_addr,
				       data->T18_address + MXT_COMMS_CTRL,
				       1, &val);
		if (error)
			return error;

		if (!(val & MXT_COMMS_RETRIGEN)) /* wrong in the standard driver ????? */
			return 0;
	}

	dev_warn("Enabling RETRIGEN workaround\n");
	data->use_retrigen_workaround = true;
	return 0;
}

static int mxt_set_t7_power_cfg(struct mxt_data *data, u8 sleep)
{
	int error;
	struct t7_config *new_config;
	struct t7_config deepsleep = { .active = 0, .idle = 0 };

	if (sleep == MXT_POWER_CFG_DEEPSLEEP)
	    new_config = &deepsleep;
	else
	    new_config = &data->t7_cfg; 

	error = mxt_write_reg(data->i2c_addr, data->T7_address, sizeof(data->t7_cfg), new_config);
	if (error)
	    return error;

	dev_dbg("Set T7 ACTV:%d IDLE:%d\n", new_config->active, new_config->idle);

	return 0;
}

static int mxt_init_t7_power_cfg(struct mxt_data *data)
{
	int error;
	bool retry = false;

recheck:
	error = mxt_read_reg(data->i2c_addr, data->T7_address, sizeof(data->t7_cfg), &data->t7_cfg);
	if (error)
		return error;

	if (data->t7_cfg.active == 0 || data->t7_cfg.idle == 0) {
		if (!retry) {
			dev_dbg("T7 cfg zero, resetting\n");
			mxt_soft_reset(data);
			retry = true;
			goto recheck;
		} else {
			dev_dbg("T7 cfg zero after reset, overriding\n");
			data->t7_cfg.active = 20;
			data->t7_cfg.idle = 100;
			return mxt_set_t7_power_cfg(data, MXT_POWER_CFG_RUN);
		}
	}

	dev_dbg("Initialized power cfg: ACTV %d, IDLE %d\n",
		data->t7_cfg.active, data->t7_cfg.idle);
	return 0;
}

/*
 * mxt_update_cfg - download configuration to chip
 *
 * Atmel array Config File Format
 *
 * The rest of the file consists of one line per object instance:
 *   <TYPE> <INSTANCE> <SIZE> <CONTENTS>
 *
 *   <TYPE> - 1-byte object type as hex
 *   <INSTANCE> - 1-byte object instance number as hex
 *   <SIZE> - 1-byte object size as hex
 *   <CONTENTS> - array of <SIZE> 1-byte hex values
 */
static int mxt_update_cfg(struct mxt_data *data)
{
	int ret;
	u16 addr = 0;
	u8 *cfg = data->cfg_data;
	u8 size = 0;
	u32 calculated_crc = 0;
	u8 type;
	int error;
	u16 writing_len;
	u16 writed_len;
	struct crc_control crc_ctrl;
	u8 version_buf[3];
	
	/* The 5, 6, 7 byte of T38 are used to sign the version of config */
	error = mxt_read_reg(data->i2c_addr, data->T38_address + 5, 3, version_buf);
	if (error) {
		dev_err("Read T38 error, ret=%d\n", error);
		return error;
	}
	
	/* search for the T38 version value in the file data */
	for (int i = 0; i < data->cfg_length; ) {
	    if (cfg[i] == 38) {
			/* skip instance and size byte, if config version match, skip upgrade, return*/
			if (cfg[i + 8] == version_buf[0] && cfg[i + 9] == version_buf[1] 
			    && cfg[i + 10] == version_buf[2])
			    return 0;
			else
			    break;
		} else {
			size = cfg[i + 2];
			i += size + 3;
		}
	}
	
	crc_ctrl.end_off = 0;        /* initialize as 0 to sign the start of crc calculation */
    crc_ctrl.last_odd = false;
    crc_ctrl.data_end = false;
	
    /* get the crc checksum of the config in ram*/
	mxt_update_crc(data, MXT_COMMAND_REPORTALL, 1);

    /* Disable interrupt before update config. I2C read and write will conflict ?????? */
    disable_irq(data->irq);         

    /* write config to ram and calculate the checksum */
    for (int i = 0; i < data->cfg_length; ) {
		
		if (cfg[i] == 117) {             /* T117 may be the first object */
			addr = data->T117_address;
		} else if (cfg[i] == 68) {       /* T68 may be the first object */
			addr = data->T68_address;
		} else if (cfg[i] == 38) {       /* T38 may be the first object */
		    addr = data->T38_address;
	    }    /* other objects' address is following T38's address in sequence */
			
		type = cfg[i++];
		i++;   /* skip the instance byte */	
		
		size = cfg[i++];

		writed_len = 0;
        do {// if more than MXT_MAX_BLOCK_WRITE bytes, write MXT_MAX_BLOCK_WRITE bytes one time
			if ((size - writed_len) >= MXT_MAX_BLOCK_WRITE)
			    writing_len = MXT_MAX_BLOCK_WRITE;
			else
			    writing_len = size - writed_len;
			
			error = mxt_write_reg(data->i2c_addr, addr + writed_len, writing_len, &cfg[i + writed_len]);
			if (error) {
				dev_err("Config write error, ret=%d\n", error);
				return error;
			}
			
			if ((size - writed_len) <= MXT_MAX_BLOCK_WRITE)
			    break;
			else 
				writed_len += MXT_MAX_BLOCK_WRITE;
				
		} while (1);
		
		/* Calculate crc of the config in the array, it should be started from T14 or T71 or T7 */
		if (type == 14 || type == 71 || type == 7 || crc_ctrl.end_off != 0) {

			crc_ctrl.start_off = 0;
			crc_ctrl.end_off = size;
			
		    if ((i + size) >= data->cfg_length)
		        crc_ctrl.data_end = true;
			else
			    crc_ctrl.data_end = false;
				
			mxt_calculate_crc(&cfg[i], &calculated_crc, &crc_ctrl);
		}
		
		i += size;
		addr += size;
	}
	
	enable_irq(data->irq);
	
	if (data->config_crc != calculated_crc)
	    dev_warn("Config CRC in file inconsistent, calculated=%06X, file=%06X\n", 
		calculated_crc, data->config_crc);

	mxt_update_crc(data, MXT_COMMAND_BACKUPNV, MXT_BACKUP_VALUE);

	ret = mxt_check_retrigen(data);
	if (ret)
		return ret;

	ret = mxt_soft_reset(data);
	if (ret)
		return ret;

	dev_info("Config successfully updated\n");

	/* T7 config may have changed */
	mxt_init_t7_power_cfg(data);

	return ret;
}


static int mxt_acquire_irq(struct mxt_data *data)
{
	int error;

	if (!data->irq) {
		register_mxt_irq(&data->irq, mxt_interrupt);
	} 
	
	enable_irq(data->irq);

	if (data->use_retrigen_workaround) {
		error = mxt_process_messages_until_invalid(data);
		if (error)
			return error;
	}

	return 0;
}

static int mxt_parse_object_table(struct mxt_data *data, struct mxt_object *object, u8 *ptr_id)
{
	u8 reportid = *ptr_id;
	u8 min_id, max_id;

	if (object->num_report_ids) {
		min_id = reportid;
		reportid += object->num_report_ids * mxt_obj_instances(object);
		max_id = reportid - 1;
	} else {
		min_id = 0;
		max_id = 0;
	}

	dev_dbg(
		"T%u Start:%u Size:%u Instances:%u Report IDs:%u-%u\n",
		object->type, object->start_address,
		mxt_obj_size(object), mxt_obj_instances(object),
		min_id, max_id);

	switch (object->type) {
	case MXT_GEN_MESSAGE_T5:
		if (data->info.family_id == 0x80 &&
			data->info.version < 0x20) {
			/*
				* On mXT224 firmware versions prior to V2.0
				* read and discard unused CRC byte otherwise
				* DMA reads are misaligned.
				*/
			data->T5_msg_size = mxt_obj_size(object);
		} else {
			/* CRC not enabled, so skip last byte */
			data->T5_msg_size = mxt_obj_size(object) - 1;
		}
		data->T5_address = object->start_address;
		break;
	case MXT_GEN_COMMAND_T6:
		data->T6_reportid = min_id;
		data->T6_address = object->start_address;
		break;
	case MXT_GEN_POWER_T7:
		data->T7_address = object->start_address;
		break;
#ifdef TOUCH_KEY
	case MXT_TOUCH_KEYARRAY_T15:
		data->T15_reportid_min = min_id;
		data->T15_reportid_max = max_id;
		break;
#endif
	case MXT_SPT_COMMSCONFIG_T18:
		data->T18_address = object->start_address;
		break;
	case MXT_SPT_GPIOPWM_T19:
		data->T19_reportid = min_id;
		break;
#ifdef T9_OBJECT
    case MXT_TOUCH_MULTI_T9:
    /* Only handle messages from first T9 instance */
	    data->T9_address = object->start_address;
        data->T9_reportid_min = min_id;
        data->T9_reportid_max = min_id + object->num_report_ids - 1;
        data->num_touchids = object->num_report_ids;
    break;
#else
	case MXT_TOUCH_MULTITOUCHSCREEN_T100:
	    data->T100_address = object->start_address;
		data->T100_reportid_min = min_id;
		data->T100_reportid_max = max_id;
		/* first two report IDs reserved */
		data->num_touchids = object->num_report_ids - 2;
		break;
#endif
	case MXT_SPT_USERDATA_T38:
		data->T38_address = object->start_address;
		break;
	case MXT_SPT_SERIALDATACOMMAND_T68:
		data->T68_address = object->start_address;
		break;
	case MXT_SPT_DATACONTAINER_T117:
		data->T117_address = object->start_address;
		break;
#ifndef T44_NONE
	case MXT_SPT_MESSAGECOUNT_T44:
	    data->T44_address = object->start_address;
	    break;
#endif
	}

	/* Store maximum reportid */
	data->max_reportid = reportid;
	*ptr_id = reportid;

#ifndef T44_NONE
	/* If T44 exists, T5 position has to be directly after */
	if (data->T44_address && data->T5_address && (data->T5_address != data->T44_address + 1)) {
		dev_err("Invalid T44 position\n");
		return -EINVAL;
	}
#endif

	return 0;
}

static int mxt_read_info_block(struct mxt_data *data)
{
	int error;
	size_t size;
	u8 *id_buf, *buf;
	u8 num_objects;
	u32 calculated_crc = 0;
	u16 addr = 0;
	u8 reportid;
	struct crc_control crc_ctrl;

	/* Read 7-byte ID information block starting at address 0 */
	size = sizeof(struct mxt_info);
	id_buf = (u8 *)(&data->info);     

	error = mxt_read_reg(data->i2c_addr, 0, size, id_buf);
	if (error) {
		return error;
	}
	
	dev_info(
		 "Family: %u Variant: %u Firmware V%u.%u.%02X Objects: %u\n",
		 data->info.family_id, data->info.variant_id,
		 data->info.version >> 4, data->info.version & 0xf,
		 data->info.build, data->info.object_num);
	
	crc_ctrl.start_off = 0;
	crc_ctrl.end_off = size;
	crc_ctrl.last_odd = false;
	crc_ctrl.data_end = false;
	/* start to calculate the information block checksum */
	mxt_calculate_crc(id_buf, &calculated_crc, &crc_ctrl);
	
	/* get the objects number for rest of info block */
	num_objects = ((struct mxt_info *)id_buf)->object_num;
	
	addr = MXT_OBJECT_START;
	buf = data->msg_buf;       /* use message buf to store the info block data temporary */

	size = sizeof(struct mxt_object);
	crc_ctrl.end_off = size;
	
	/* Valid Report IDs start counting from 1 */
	reportid = 1;
	
	for (int i = 0; i < num_objects; i++) {
		/* read data to buffer, start from index 1 for crc calculating */
        error = mxt_read_reg(data->i2c_addr, addr, size, buf);
		if (error) {
			return error;
		}
		addr += size;

        /* Parse object table information */
        error = mxt_parse_object_table(data, (struct mxt_object *)buf, &reportid);
        if (error) {
	        dev_err("Error %d parsing object table\n", error);
	        return error;
        }
		
		// the last frame data need to deal with the last data
		if (i == num_objects - 1) 
            crc_ctrl.data_end = true;
			
		mxt_calculate_crc(buf, &calculated_crc, &crc_ctrl);
	}
	
	/* read the information block checksum */
	error = mxt_read_reg(data->i2c_addr, addr, MXT_INFO_CHECKSUM_SIZE, buf);
	if (error) {
		return error;
	}
	/* Extract & calculate checksum */
	data->info_crc = buf[0] | (buf[1] << 8) | (buf[2] << 16);
	
	/*
	 * CRC mismatch can be caused by data corruption due to I2C comms
	 * issue or else device is not using Object Based Protocol (eg i2c-hid)
	 */
	if ((data->info_crc == 0) || (data->info_crc != calculated_crc)) {
		dev_err(
			"Info Block CRC error calculated=0x%06X read=0x%06X\n",
			calculated_crc, data->info_crc);
		error = -EIO;
		return error;
	}

	return 0;
}

#ifdef T9_OBJECT
static int mxt_read_t9_resolution(struct mxt_data *data)
{
	int error;
	struct t9_range range;
	unsigned char orient;

	error = mxt_read_reg(data->i2c_addr, data->T9_address + MXT_T9_RANGE, sizeof(range), &range);
	if (error)
	    return error;

	error = mxt_read_reg(data->i2c_addr, data->T9_address + + MXT_T9_ORIENT, 1, &orient);
	if (error)
	    return error;

	/* Handle default values */
	if (range.x == 0)
	    range.x = 1023;

	if (range.y == 0)
	    range.y = 1023;

	if (orient & MXT_T9_ORIENT_SWITCH) {
		data->max_x = range.y;
		data->max_y = range.x;
		} else {
		data->max_x = range.x;
		data->max_y = range.y;
	}

	dev_dbg("Touchscreen size X%uY%u\n", data->max_x, data->max_y);

	return 0;
}
#else
static int mxt_read_t100_config(struct mxt_data *data)
{
	int error;
	u16 range_x, range_y;
	u8 cfg;

	error = mxt_read_reg(data->i2c_addr,
			       data->T100_address + MXT_T100_XRANGE,
			       sizeof(range_x), &range_x);
	if (error)
		return error;

	error = mxt_read_reg(data->i2c_addr,
			       data->T100_address + MXT_T100_YRANGE,
			       sizeof(range_y), &range_y);
	if (error)
		return error;

	error =  mxt_read_reg(data->i2c_addr,
				data->T100_address + MXT_T100_CFG1,
				1, &cfg);
	if (error)
		return error;

	/* Handle default values */
	if (range_x == 0)
		range_x = 1023;

	if (range_y == 0)
		range_y = 1023;

	if (cfg & MXT_T100_CFG_SWITCHXY) {
		data->max_x = range_y;
		data->max_y = range_x;
	} else {
		data->max_x = range_x;
		data->max_y = range_y;
	}

	dev_info("T100 Touchscreen size X%uY%u\n", data->max_x, data->max_y);

	return 0;
}
#endif

static int mxt_initialize_input_device(struct mxt_data *data)
{
	int error = 0;

#ifdef T9_OBJECT
    error = mxt_read_t9_resolution(data);
    if (error)
        dev_warn("Failed to initialize T9 resolution\n");
#else
	error = mxt_read_t100_config(data);
	if (error)
		dev_warn("Failed to read T100 config\n");
#endif

    report_resolution(data->max_x, data->max_y);

	return error;
}

static int mxt_configure_objects(struct mxt_data *data)
{
	int error;

	error = mxt_init_t7_power_cfg(data);
	if (error) {
		dev_err("Failed to initialize power cfg\n");
		return error;
	}

	if (data->cfg_length) {
		error = mxt_update_cfg(data);
		if (error)
		dev_warn("Error %d updating config\n", error);
	}

	error = mxt_initialize_input_device(data);
	if (error)
	    return error;

	return 0;
}

int mxt_initialize()
{
	struct mxt_data *data = &mxtData;
	int recovery_attempts = 0;
	int error;

    /*************************************************************************************
	 ***********************************Modify Note***************************************
	 1. I2C, Timer(RTC) need to be initialized, 
	 2. RST pin initialized to be output high level,
	 3. CHG pin initialized to be input, 
	 4. External interrupt initialized to be low level trigger in priority, 
	    or falling edge trigger 
	 
	 All of these are done in this example by atmel_start_init() 
	 *************************************************************************************/
	
	/*************************************************************************************
	 ***********************************Modify Note***************************************
	 If the project has keys designed together with the touch screen, you need to 
	 configure the whole key number and the number of key in the instance 0.
	 
	 You need to check with the engineer who provided the config file to set the value.
	 
	 If there's no key used, the macro TOUCH_KEY should be commented or set the value to 0.
	 *************************************************************************************/
	#ifdef TOUCH_KEY      
	data->t15_num_keys = 9;        /* All the keys in two instances */
	data->t15_keys_instance0 = 0;  /* If there're keys in instance 1, this value must be set */
	#endif
	
	data->chg_completion = 0;
	data->reset_completion = 0;
	data->crc_completion = 0;
	
	data->i2c_addr = I2C_ADDRESS;
	
	data->cfg_data = file_cfg_data;
	data->cfg_length = get_cfg_length();

	while (1) {
		error = mxt_read_info_block(data);
		if (!error)
			break;

		/* Check bootloader state */
		error = mxt_probe_bootloader(data, false);
		if (error) {
			dev_info("Trying alternate bootloader address\n");
			error = mxt_probe_bootloader(data, true);
			if (error) {
				/* Chip is not in appmode or bootloader mode */
				return error;
			}
		}

		/* OK, we are in bootloader, see if we can recover */
		if (++recovery_attempts > 1) {
			dev_err("Could not recover from bootloader mode\n");
			/*
			 * We can reflash from this state, so do not
			 * abort initialization.
			 */
			data->in_bootloader = true;
			return 0;
		}

		/* Attempt to exit bootloader into app mode */
		mxt_send_bootloader_cmd(data, false);
		msleep(MXT_FW_RESET_TIME);
	}

	error = mxt_check_retrigen(data);
	if (error)
		return error;

	error = mxt_acquire_irq(data);
	if (error)
		return error;

	if (data->cfg_length) {
		error = mxt_configure_objects(data);
		if (error)
			return error;
	}

	return 0;
}