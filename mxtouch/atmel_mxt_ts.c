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
#include <string.h>
#include "atmel_mxt_ts.h"
#include "mcu_interface.h"

/* maxtouch data read from IC or need to be initialized */
static struct mxt_data mxtData;
  
#ifdef SECURITY    
static int mxt_resync_comm(struct mxt_data *data);
#endif

static size_t mxt_obj_size(const struct mxt_object *obj)
{
	return obj->size_minus_one + 1;
}

static size_t mxt_obj_instances(const struct mxt_object *obj)
{
	return obj->instances_minus_one + 1;
}

#ifdef BOOTLOADER_PROCESS
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
		dev_err("Appmode i2c address %02x not found\n", appmode);
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
#endif

static void mxt_proc_t6_messages(struct mxt_data *data, u8 *msg)
{
	u8 status = msg[1];
	u32 crc = msg[2] | (msg[3] << 8) | (msg[4] << 16);

	if (crc != data->config_crc) {
		data->config_crc = crc;
		dev_dbg("T6 Config Checksum: %06X\n", crc);
	}

	data->crc_completion = 1;

	/* Detect reset */
	if (status & MXT_T6_STATUS_RESET)
		data->reset_completion = 1;

	/* Output debug if status has changed */
	if (status != data->t6_status)
		dev_dbg("T6 Status %02X%s%s%s%s%s%s%s\n",
			status,
			status == 0 ? " OK" : "",
			status & MXT_T6_STATUS_RESET ? " RESET" : "",
			status & MXT_T6_STATUS_OFL ? " OFL" : "",
			status & MXT_T6_STATUS_SIGERR ? " SIGERR" : "",
			status & MXT_T6_STATUS_CAL ? " CAL" : "",
			status & MXT_T6_STATUS_CFGERR ? " CFGERR" : "",
			status & MXT_T6_STATUS_COMSERR ? " COMSERR" : "");

#ifdef SECURITY
	if (status & MXT_T6_STATUS_COMSERR) {
		dev_err("T6 COMSERR Error, Seqnum(%d)\n", data->msg_num.txseq_num);
	}
#endif

	/* Save current status */
	data->t6_status = status;
}

#ifdef TOUCH_OBJECT    // touch screen 
#ifdef T9_OBJECT       // old IC use T9 object for touch screen
static void mxt_proc_t9_message(struct mxt_data *data, u8 *message)
{
	int id;
	int x;
	int y;
	u8 status;
	u8 type = 0;

	id = message[0] - data->tch_obj_info.reportid_min.T9;
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
#else    // new IC use T100 for touch screen
static void mxt_proc_t100_message(struct mxt_data *data, u8 *message)
{
	int id = 0;
	u8 status;
	u8 type = 0;
	u8 event = 0;
	u16 x;
	u16 y;
	bool active = false;
	
#ifdef SECOND_TOUCH
    int id_sec = 0;
#endif

	id = message[0] - data->tch_obj_info.reportid_min.T100 - MXT_RSVD_RPTIDS;

	/* ignore SCRSTATUS events */
	if (id < 0)
		return;
		
#ifdef SECOND_TOUCH
	if (id >= MXT_MIN_RPTID_SEC) {
		id_sec = (message[0] - data->tch_obj_info.reportid_min.T100 - MXT_MIN_RPTID_SEC -
		MXT_RSVD_RPTIDS);
	}

	/* Skip SCRSTATUS events */
	if (id_sec < 0)
	    return;
#endif

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

#ifdef SECOND_TOUCH
    if (id >= MXT_MIN_RPTID_SEC) {
	    report_id(id_sec);
	} else {
	    report_id(id);
    }
#endif

	if (active) {
	#ifdef SECOND_TOUCH
	    dev_dbg("[%u] type:%u x:%u y:%u \n", id_sec, type, x, y);
	#else
		dev_dbg("[%u] type:%u x:%u y:%u \n", id, type, x, y);
	#endif

#ifdef SECOND_TOUCH
        if (id >= MXT_MIN_RPTID_SEC) {
			/* report type event and coordinate */
			report_state(type, event);
			report_coordinate(x, y);
			
			if (id == MXT_MIN_RPTID_SEC)
			{
				report_single_touch();
			}
		} else 
#endif
        {
            /* report type event and coordinate */
		    report_state(type, event);
		    report_coordinate(x, y);
		}
	} else {
		/* report type and event */
	#ifdef SECOND_TOUCH
		
	    if (id >= MXT_MIN_RPTID_SEC) {
			
			dev_dbg("[%u] release\n", id_sec);
			
			report_state(type, event);
		    
		    if (id == MXT_MIN_RPTID_SEC)
		    {
			    report_single_touch();
		    }
	    } else
	#endif
	    {
	        dev_dbg("[%u] release\n", id);
		    report_state(type, event);
		}
	}
}
#endif
#endif

#ifdef TOUCH_KEY
static void mxt_proc_t15_messages(struct mxt_data *data, u8 *msg)
{
	int key;
	bool curr_state, new_state;
	unsigned long keystates = msg[2] + (msg[3] << 8);
	int id = msg[0] - data->T15_reportid_min;
	
	if(id) // the second instance will shift the key states with the key number of the first instance
	    keystates <<= data->t15_num_keys_inst0;

	for (key = 0; key < data->t15_num_keys_all; key++) {
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

#ifdef SELF_TEST
static void mxt_proc_t10_messages(struct mxt_data *data, u8 *msg)
{
	u8 rid = msg[0];
	u8 status = msg[1];
	u8 cmd = msg[2];
	const char *info = "";

	/*
		Status:
			0x31: All on-demand tests has passed
			0x32: An on demand test has failed
			0x3F: The test code supplied in the CMD field is not associated with a valid test
			0x11: All POST tests have completed successfully
			0x12: A POST test has failed
			0x21: All BIST tests have completed successfully
			0x22: A BIST testd has failed
			0x23: BIST test cycle overrun
		CMD:
			2: Clock Related tests
			3: Flash Memory tests
			4: RAM memory tests
			5: CTE tests
			6: Signal Limit tests
			7: Power-related tests
			8: Pin Fault tests:
				The test failed becasue of pin fault. THe INFO fields indicated the first pin fault that was
					detected.Note that if the initial pin fault test fails, then the Self Test T25 object will
					generate a message with this result code on reset
					SEQ_NUM:
						0x01: Driven Ground
						0x02: Driven Hight
						0x03: Walking 1
						0x04: Walking 0
						0x07: Initial High Voltage
					X_PIN:
					Y_PIN:
						the number of the pin + 1(e.g. value 3 will mean X2)
						Both ZERO: DS pin
	*/
    if (rid == data->test_obj_info.reportid_min.T10) {
	    switch(status)	{
		    case 0x31:
		        info = "PASS: ";
		        break;
		    case 0x32:
		        info = "Failed: ";
		        break;
		    case 0x3F:
		        info = "Un-Supported: ";
		        break;
		    case 0x11:
		        info = "POST PASS: ";
		        break;
		    case 0x12:
		        info = "POST Failed: ";
		        break;
		    case 0x21:
		        info = "BIST PASS: ";
		        break;
		    case 0x22:
		        info = "BIST Failed: ";
		        break;
		    case 0x23:
		        info = "BIST overrun: ";
		        break;
		    default:
		        info = "Unknown: ";
	    }
	    dev_info("T10 Status %2x  Info: %02s\n", status, info);

	    switch(cmd) {
		    case 2:
		        info = "Clock Related tests \n";
		        break;
		    case 3:
		        info = "Flash Memory tests \n";
		        break;
		    case 4:
		        info = "RAM Memory tests \n";
		        break;
		    case 5:
		        info = "CTE tests\n";
		        break;
		    case 6:
		        info = "Signal Limit tests \n";
		        break;
		    case 7:
		        info = "Power-related tests \n";
		        break;
			// if you need to get the detail message of pin fault, you should use the message from msg[3] to show
		    case 8:
		        info = "Pin Fault tests \n";
		        break;
		    default:
		        info = "\n";
	    }
    }
	
	dev_info("T10 test group failure: %2x  Info: %02s\n", cmd, info);
}

static void mxt_proc_t25_messages(struct mxt_data *data, u8 *msg)
{
	u8 rid = msg[0];
	u8 status = msg[1];
	const char *info = "";

	/*
		T25 message:
			0xFE: All tests passed
			0xFD: The test code supplied in the CMD field is not associated with a valid test
			0x01: Avdd is not present.The failure is reported to the host every 200ms
			0x12: The test failed becasue of pin fault. THe INFO fields indicated the first pin fault that was
					detected.Note that if the initial pin fault test fails, then the Self Test T25 object will
					generate a message with this result code on reset
					SEQ_NUM:
						0x01: Driven Ground
						0x02: Driven Hight
						0x03: Walking 1
						0x04: Walking 0
						0x07: Initial High Voltage
					X_PIN:
					Y_PIN:
						the number of the pin + 1(e.g. value 3 will mean X2)
						Both ZERO: DS pin
			0x17: The test failed because of a signal limit fault.
	*/

	if (rid == data->test_obj_info.reportid_min.T25) {
		switch(status) {
			case 0xFE:
			    info = "PASS\n";
			    break;
			case 0xFD:
			    info = "Un-Supported\n";
			    break;
			case 0x1:
			    info = "AVDD is not present\n";
			    break;
			// if you need to get the detail message of pin fault, you should use the message from msg[2] to show
			case 0x12:     
			    info = "Pin fault\n";
			    break;
			case 0x17:
			    info = "Signal limit fault\n";
			    break;
			default:
			info = "Unknown: ";
		}

		dev_info("T25 Status 0x%2x  Info: %02s\n", status, info);
	}
}

static int mxt_set_selftest(struct mxt_data *data, u8 cmd, bool wait)
{
	u16 reg;
	int timeout_counter = 0;
	int ret;
	u8  val;

    // T25 and T10 use the same address to store info 
    reg = data->test_obj_info.address.T10; 
	if (reg == 0) {
		dev_err("No Selftest Object found");
		return -EEXIST;
	}

	val = cmd;
	ret = mxt_write_reg(reg + MXT_SELFTEST_CMD, sizeof(val), &val, data);
	if (ret) {
		dev_err("Send Test Command %02x failed\n", cmd);
		return ret;
	}

	if (!wait) {
		return 0;
	}

	do {

		msleep(MXT_SELFTEST_TIME);
		ret = mxt_read_reg(reg + MXT_SELFTEST_CMD, 1, &val, data);
		if (ret) {
			dev_err("Read Test Command %02x failed\n", cmd);
			return ret;
		}

	} while ((val != 0) && (timeout_counter++ <= 100));

	if (timeout_counter > 100) {
		dev_err("Test Command Timeout\n");
		return -ETIMEDOUT;
	}

	return 0;
}
#endif

static int mxt_proc_message(struct mxt_data *data, u8 *message)
{
	u8 report_id = message[0];

	if (report_id == MXT_RPTID_NOMSG
#ifdef SECOND_TOUCH
         || report_id == MXT_RPTID_RVSD
#endif
	)
		return 0;

	if (report_id == data->T6_reportid) {
		mxt_proc_t6_messages(data, message);
		
#ifdef TOUCH_OBJECT

#ifdef T9_OBJECT
    } else if (report_id >= data->tch_obj_info.reportid_min.T9 &&
            report_id <= data->tch_obj_info.reportid_max.T9) {
	    mxt_proc_t9_message(data, message);
#else
	} else if (report_id >= data->tch_obj_info.reportid_min.T100 &&
		   report_id <= data->tch_obj_info.reportid_max.T100) {
		mxt_proc_t100_message(data, message);
#endif

#endif

#ifdef TOUCH_KEY
	} else if (report_id >= data->T15_reportid_min
		   && report_id <= data->T15_reportid_max) {
		mxt_proc_t15_messages(data, message);
#endif

#ifdef SELF_TEST
    } else if (report_id == data->test_obj_info.reportid_min.T10) {
		if (data->test_all_cmd == MXT_T10_TEST_ALL)
            mxt_proc_t10_messages(data, message);
        else
            mxt_proc_t25_messages(data, message);
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
		ret = mxt_read_reg(data->T5_address, data->T5_msg_size, data->msg_buf, data);
		
#ifdef SECURITY
		if (ret && data->crc_enabled) {    // read error, try to resync 
			ret = mxt_resync_comm(data);
		}
#endif
			
		if (ret) {
			dev_err("Failed to read %u messages (%d)\n", 1, ret);
			return ret;
		}
	
	#ifdef DEBUG_INFO    // print all the message bytes
		dev_info("message [ ");
		for (int i = 0; i < data->T5_msg_size; i++)
		    dev_info("%x ", data->msg_buf[i]);
		dev_info(" ]\n ");
	#endif

		ret = mxt_proc_message(data, data->msg_buf);

		if (ret == 1)
			num_valid++;
	}

	/* return number of messages read */
	return num_valid;
}

#ifndef T44_NONE
static void mxt_process_messages_t44_t144(struct mxt_data *data)
{
	int ret;
	u8 count, num_left;
	
    /* For new HA parts, read only T144 count, otherwise, read T44 */
	/* Read T44 or T144 firstly, they used the same address space */
	ret = mxt_read_reg(data->count_address.T44, data->msg_count_size, data->msg_buf, data);
	if (ret) {
		dev_err("Failed to read T44 or T144 (%d)\n", ret);

#ifdef SECURITY		
		// read error, try to resync
		if (data->crc_enabled) {
			// Recovery requires RETRIGEN bit to be enabled in config
			mxt_resync_comm(data);
		}
#endif
		return;
	}

	count = data->msg_buf[0];
 
	/*
	 * This condition may be caused by the CHG line being configured in
	 * Mode 0. It results in unnecessary I2C operations but it is benign.
	 */
	if (count == 0)
		return;
	else if (count > data->max_reportid) {
		dev_warn("T44/T144 count %d exceeded max report id\n", count);
		
#ifdef SECURITY		// read data exception, try to resync
		if (data->crc_enabled) {
			// Recovery requires RETRIGEN bit to be enabled in config
			mxt_resync_comm(data);
		}
#endif
		return;    ////////////
	}

	num_left = count;

	/* Process remaining messages if necessary */
	if (num_left) {
		//dev_dbg("Remaining messages to process\n");

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

	// Read messages until we force an invalid 
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

#ifdef BOOTLOADER_PROCESS
	if (data->in_bootloader) 
		return;
#endif
	
#ifndef T44_NONE
    if (data->count_address.T44) {
	    return mxt_process_messages_t44_t144(data);
    } 
#else
    else {
	    return mxt_process_messages(data);
	}
#endif
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
#ifdef SECURITY
    int buff;
#endif
	
	/* If it's low level trigger, it will not need retrigger */
	if (check_low_level_trigger() == true) {
	    dev_info("Level triggered\n");
	    return 0;
	}
		
	if (data->T18_address) {
	     error = mxt_read_reg(data->T18_address + MXT_COMMS_CTRL, 1, &val, data);
		if (error)
			return error;

		if (!(val & MXT_COMMS_RETRIGEN)) {/* wrong in the standard driver ????? */
		    dev_info("RETRIGEN enabled\n");
			return 0;
		}
	}

#ifdef SECURITY
    if (data->crc_enabled) {   // need to enable retrigger in security mode
        buff = val | MXT_COMMS_RETRIGEN;
	    error = mxt_write_reg(data->T18_address + MXT_COMMS_CTRL, 1, &buff, data);
        if (error)
            return error;
		
		dev_warn("Enabling RETRIGEN workaround\n");
	}
#endif

    data->use_retrigen_workaround = true;

    return 0;
}

#ifdef POWER_CONTROL
static int mxt_set_t7_power_cfg(struct mxt_data *data, u8 sleep)
{
	int error;
	struct t7_config *new_config;
	struct t7_config deepsleep = { .active = 0, .idle = 0 };

	if (sleep == MXT_POWER_CFG_DEEPSLEEP)
	    new_config = &deepsleep;
	else
	    new_config = &data->t7_cfg; 

    error = mxt_write_reg(data->T7_address, sizeof(data->t7_cfg), new_config, data);

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
	error = mxt_read_reg(data->T7_address, sizeof(data->t7_cfg), &data->t7_cfg, data);
	if (error)
		return error;

	if (data->t7_cfg.active == 0 || data->t7_cfg.idle == 0) {
		if (!retry) {
			dev_dbg("T7 cfg zero, retry\n");
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
#endif

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
#ifdef CONFIG_UPGRADE
static int mxt_t6_command(struct mxt_data *data, u16 cmd_offset, u8 value, bool wait)
{
	u16 reg;
	u8 command_register;
	int timeout_counter = 0;
	int ret;

	reg = data->T6_address + cmd_offset;
	
	ret = mxt_write_reg(reg, 1, &value, data);
	if (ret)
		return ret;

	if (!wait)
		return 0;

    /* For the command, if it executed successfully, the command value will be zero*/
	do {
		msleep(20);

        ret = mxt_read_reg(reg, 1, &command_register, data);
		if (ret)
			return ret;
	} while (command_register != 0 && timeout_counter++ <= 100);

	if (timeout_counter > 100) {
		dev_err("Command failed!\n");
		return -EIO;
	}

	return 0;
}

static int mxt_soft_reset(struct mxt_data *data, bool reset_enabled)
{
	int ret = 0;

	dev_info("Resetting device\n");

	disable_irq(data->irq);

	data->reset_completion = 1;

	ret = mxt_t6_command(data, MXT_COMMAND_RESET, MXT_RESET_VALUE, false);
	if (ret)
		return ret;
#ifdef SECURITY		
	if (reset_enabled)
	    mxt_update_seq_num_lock(data, true, 0);
#endif

	/* Ignore CHG line for 100ms after reset */
	msleep(MXT_RESET_INVALID_CHG);

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

static int mxt_update_cfg(struct mxt_data *data)
{
	int ret = 0;
	u16 addr = 0;
	u8 *cfg = data->cfg_data;
	u8 size = 0;
	u32 calculated_crc = 0;
	u8 type;
	int error;
	u16 writing_len;
	u16 writed_len;
	struct crc_control crc_ctrl;
	u8 *buffer;
	
	// compare the maxtouch information
	buffer = (u8 *)(&data->info);
	for (int i = 0; i < MXT_OBJECT_START; i++) {
		// the maxtouch information mismatch, return error type 5
		if (buffer[i] != file_device_info[i]) {
		
		    error = -5;
		    dev_err("maxtouch information mismatch, ret=%d\n", error);
		    return error;
		}
	}
		
	// FIXME: the Report All command can't get out the config crc not
	mxt_update_crc(data, MXT_COMMAND_REPORTALL, 1);

	//Clear messages after update in cases /CHG low
	error = mxt_process_messages_until_invalid(data);
	if (error)
	    dev_dbg("Unable to read CRC\n");

     /* if config version and checksum match, skip upgrade, return*/
    if (data->config_crc == file_cfg_crc) {
		dev_info("Config CRC consistent, no need to upgrade\n");
        return 0;
	}
	
	crc_ctrl.end_off = 0;        /* initialize as 0 to sign the start of crc calculation */
    crc_ctrl.last_odd = false;
    crc_ctrl.data_end = false;
	
	/* Stop T70 Dynamic Configuration before calculation of CRC */
	mxt_update_crc(data, MXT_COMMAND_BACKUPNV, MXT_BACKUP_W_STOP);   

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
				
			error = mxt_write_reg(addr + writed_len, writing_len, &cfg[i + writed_len], data);

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
	
	if (file_cfg_crc != calculated_crc)
	    dev_warn("Config CRC in file inconsistent, calculated=%06X, file=%06X\n", 
		calculated_crc, file_cfg_crc);

    msleep(50);	//Allow delay before issuing backup and reset
	mxt_update_crc(data, MXT_COMMAND_BACKUPNV, MXT_BACKUP_VALUE);
    msleep(200);	//Allow 200ms before issuing reset
	
    ret = mxt_soft_reset(data, true);

	if (ret)
		return ret;

	dev_info("Config successfully updated\n");

#ifdef POWER_CONTROL
	/* T7 config may have changed */
	mxt_init_t7_power_cfg(data);
#endif
	
	ret = mxt_check_retrigen(data);
	if (ret)
	    return ret;

	return ret;
}
#endif

static int mxt_acquire_irq(struct mxt_data *data)
{
	int error;
	
	if (!data->irq) {
		register_mxt_irq(&data->irq, mxt_interrupt);
	} 
	
	//enable_irq(data->irq);
	
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
	u8 num_instances;

    num_instances = mxt_obj_instances(object);

	if (object->num_report_ids) {
		min_id = reportid;
		reportid += object->num_report_ids * num_instances;
		max_id = reportid - 1;
	} else {
		min_id = 0;
		max_id = 0;
	}

	dev_dbg(
		"T%u Start:%u Size:%u Instances:%u Report IDs:%u-%u\n",
		object->type, object->start_address,
		mxt_obj_size(object), num_instances,
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
#ifdef SECURITY
            if (data->crc_enabled)
                data->T5_msg_size = mxt_obj_size(object);
            else
#endif
                /* CRC not enabled, so skip last byte */
			    data->T5_msg_size = mxt_obj_size(object) - 1;
		}
		data->T5_address = object->start_address;
		break;
	case MXT_GEN_COMMAND_T6:
		data->T6_reportid = min_id;
		data->T6_address = object->start_address;
		break;
#ifdef POWER_CONTROL
	case MXT_GEN_POWER_T7:
		data->T7_address = object->start_address;
		break;
#endif
		
#ifdef TOUCH_KEY
	case MXT_TOUCH_KEYARRAY_T15:
	    data->T15_address = object->start_address;
		data->T15_reportid_min = min_id;
		data->T15_reportid_max = max_id;
		data->T15_size = object->size_minus_one + 1;
		break;
#endif
	case MXT_SPT_COMMSCONFIG_T18:
		data->T18_address = object->start_address;
		break;
	/*case MXT_SPT_GPIOPWM_T19:
		data->T19_reportid = min_id;
		break;*/
		
#ifdef TOUCH_OBJECT		
		
#ifdef T9_OBJECT
    case MXT_TOUCH_MULTI_T9:
    /* Only handle messages from first T9 instance */
	    data->tch_obj_info.address.T9 = object->start_address;
        data->tch_obj_info.reportid_min.T9 = min_id;
        data->tch_obj_info.reportid_max.T9 = min_id + object->num_report_ids - 1;
        data->num_touchids = object->num_report_ids;
    break;
#else
	case MXT_TOUCH_MULTITOUCHSCREEN_T100:
	    data->tch_obj_info.address.T100 = object->start_address;
        /* first two report IDs reserved */
        data->num_touchids = object->num_report_ids - MXT_RSVD_RPTIDS;
		data->tch_obj_info.reportid_min.T100 = min_id;
		data->tch_obj_info.reportid_max.T100 = max_id;
#ifdef SECOND_TOUCH
        data->T100_size = object->size_minus_one + 1;
        data->T100_instances = num_instances;
#endif
		break;
#endif

#endif

#ifdef CONFIG_UPGRADE
	case MXT_SPT_USERDATA_T38:
		data->T38_address = object->start_address;
		break;
	case MXT_SPT_SERIALDATACOMMAND_T68:
		data->T68_address = object->start_address;
		break;
	case MXT_SPT_DATACONTAINER_T117:
		data->T117_address = object->start_address;
		break;
#endif
		
#ifndef T44_NONE
	case MXT_SPT_MESSAGECOUNT_T44:
	    data->count_address.T44 = object->start_address;
		data->msg_count_size = mxt_obj_size(object);
	    break;
#endif

#ifdef SECURITY
    case MXT_SPT_MESSAGECOUNT_T144:
		data->count_address.T144 = object->start_address;
		//data->crc_enabled = true;
        data->msg_count_size = mxt_obj_size(object);
		dev_info("CRC enabled\n");
		break;
#endif

#ifdef SELF_TEST
     case MXT_SPT_SELFTESTCONTROL_T10:
         data->test_obj_info.address.T10 = object->start_address;
         data->test_obj_info.reportid_min.T10 = min_id;
		 data->test_all_cmd = MXT_T10_TEST_ALL;
         break;
	 case MXT_SPT_SELFTEST_T25:
	     data->test_obj_info.address.T25 = object->start_address;
	     data->test_obj_info.reportid_min.T25 = min_id;
		 data->test_all_cmd = MXT_T25_TEST_ALL;
	     break;
#endif
	}

	/* Store maximum reportid */
	data->max_reportid = reportid;
	*ptr_id = reportid;

#ifndef T44_NONE
	/* If T44/ T144 exists, T5 position has to be directly after */
	if (data->count_address.T44 && data->T5_address && 
	   (data->T5_address != data->count_address.T44 + data->msg_count_size)) {
		dev_err("Invalid T44 position\n");
		return -EINVAL;
	}
#endif

	return 0;
}

#ifdef SECURITY
static bool mxt_lookup_ha_chips(const struct mxt_info *info)
{
	bool is_ha = false;
	
	if (!info) {
		return false;
	}
	
	switch (info->family_id) {
		case 0xA6:
		if (info->variant_id == 0x14) {
			// "336UD-HA"
			is_ha = true;
		}
		break;
		default:
		;
	}
	
	return is_ha;
}

static int mxt_hard_reset(struct mxt_data *data)
{
	int ret = 0;

	dev_info("Resetting device\n");

	disable_irq(data->irq);

	data->reset_completion = 1;

	set_reset_pin_level(false);
	
	msleep(MXT_RESET_GPIO_TIME);

	mxt_update_seq_num_lock(data, true, 0);
	
	set_reset_pin_level(true);

	/* Ignore CHG line for 100ms after reset */
	msleep(MXT_RESET_INVALID_CHG);

	enable_irq(data->irq);

	ret = mxt_wait_for_completion(&data->reset_completion, MXT_RESET_TIMEOUT);
	if (ret)
	    return ret;

	return 0;
}

static int mxt_resync_comm(struct mxt_data *data)
{
	int error;
	uint8_t num_objects;
	u32 calculated_crc = 0;
	u16 count = 0;
	bool insync = false;
	bool is_ha = false;
	u16 addr;
	u16 round;
	struct crc_control crc_ctrl;
	
    /* Start check the Seq Num */
    for ( round = 0; round < 3 && insync != true; round ++ ){
	    // 'I' Round, use overflow search or hardware reset
	    if (round == 0) {
		    // <Round.0>: Assumed Seqnum change to `0` with unknown reason, so current is 2
		    count = 1;  // one time's trying is enough	      
		    mxt_update_seq_num_lock(data, true, 2); // CRC missed + ID info, so the Info block will be 3
			
			dev_info("Resync Round <I.0>: Assumed the seq round to 2\n");
		}else if (round == 1) {
		    // <Round.1>: use the overlfow method to retrieve the seq num
		    count = 256;
		    // start with 0
		    mxt_update_seq_num_lock(data, true, 0);
			dev_info("Resync Round <I.1>: Using overflow to search seq\n");
		} else {
		    // <Round.2>: use Hardware reset to retrieve the seq number, set seq to 0
		    count = 1;
		    dev_info("Resync Round <I.2>: Using Hardware reset to sync\n");

		    mxt_hard_reset(data);    // if hardware reset is not available, Round 2 can be omitted
	    }
		
		do {
			//dev_info("retry times (%d)\n", count);
			if (round == 1) {
				mxt_update_seq_num_lock(data, true, 0);
			}
			
			/* Re-read first 7 bytes of ID header */
			error = mxt_read_reg(0x00, sizeof(struct mxt_info), data->msg_buf, data);

			if (error) {
				dev_err(
				"Resync Read Info Block failed (I2C communication error?), Seqnum(%d)", data->msg_num.txseq_num);
				return error;
			}
			
			data->info = *(struct mxt_info *)data->msg_buf;
				
			if (mxt_lookup_ha_chips(&data->info)) {
				if (is_ha == false) {
				    is_ha = true;
				    dev_info("Resync: Found HA chips\n");
				}
			} else {       // id info mismatch, only id info only firstly
				count--;
				continue;
			}
			
			num_objects = data->info.object_num;

            calculated_crc = 0;
			crc_ctrl.start_off = 0;
			crc_ctrl.end_off = sizeof(struct mxt_info);
			crc_ctrl.last_odd = false;
			crc_ctrl.data_end = false;
			/* start to calculate the information block checksum */
			mxt_calculate_crc((u8 *)&data->info, &calculated_crc, &crc_ctrl);
				
			crc_ctrl.end_off = MXT_OBJECT_SIZE;
	
			addr = MXT_OBJECT_START;
				
			for (int i = 0; i < num_objects; i++) {
				/* read data to buffer, start from index 1 for crc calculating */
				error = mxt_read_reg(addr, MXT_OBJECT_SIZE, data->msg_buf, data);

				if (error) {
					dev_err(
					"Resync Read Info Block failed (I2C communication error?), Seqnum(%d)", data->msg_num.txseq_num);
					return error;
				}
				addr += MXT_OBJECT_SIZE;
					
				// the last frame data need to deal with the last data
				if (i == num_objects - 1)
				    crc_ctrl.data_end = true;
					
				mxt_calculate_crc(data->msg_buf, &calculated_crc, &crc_ctrl);
			}
			
			/* read the information block checksum */
			error = mxt_read_reg(addr, MXT_INFO_CHECKSUM_SIZE, data->msg_buf, data);
			if (error) {
				dev_err(
				"Resync Read Info Block failed (I2C communication error?), Seqnum(%d)", data->msg_num.txseq_num);
				return error;
			}
			
			/* Extract & calculate checksum */
			data->info_crc = data->msg_buf[0] | (data->msg_buf[1] << 8) | (data->msg_buf[2] << 16);
				
			if (data->info_crc == calculated_crc) {
				dev_info(
				"Resync is complete: Info Block CRC = %06X\n", data->info_crc);
				insync = true;
			}

		    count--;
		} while (insync == false && count != 0);
	}

	if (insync != true) {
		error = true;
		dev_info("Resync failed\n");
		return error;
	}

	return 0;
}
#endif

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

    error = mxt_read_reg(0, size, id_buf, data);
	if (error) {
		return error;
	}
	
	dev_info(
		 "Family: %u Variant: %u Firmware V%u.%u.%02X Objects: %u\n",
		 data->info.family_id, data->info.variant_id,
		 data->info.version >> 4, data->info.version & 0xf,
		 data->info.build, data->info.object_num);
    
#ifdef SECURITY
    if (mxt_lookup_ha_chips(&data->info)) {
	    dev_info("Found mXT366UD-HA\n");
    } else {
		data->crc_enabled = false;   // need to set false as it's true as first to do crc read
	}
#endif
	
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
        error = mxt_read_reg(addr, size, buf, data);
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
    error = mxt_read_reg(addr, MXT_INFO_CHECKSUM_SIZE, buf, data);
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
			"Info Block CRC error calculated=%06X read=%06X\n",
			calculated_crc, data->info_crc);
		error = -EIO;
		return error;
	}

	return 0;
}

#ifdef TOUCH_OBJECT
#ifdef T9_OBJECT
static int mxt_read_t9_resolution(struct mxt_data *data)
{
	int error;
	struct t9_range range;
	unsigned char orient;
	
	error = mxt_read_reg(data->tch_obj_info.address.T9 + MXT_T9_RANGE, sizeof(range), &range, data);
	if (error)
	    return error;

	error = mxt_read_reg(data->tch_obj_info.address.T9 + + MXT_T9_ORIENT, 1, &orient, data);
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
static int mxt_read_t100_config(struct mxt_data *data, u8 instance)
{
	int error;
	u16 range_x, range_y;
	u8 cfg;
	u16 obj_size = 0;
	
#ifdef SECOND_TOUCH
    u8 T100_enable;
	
    if (instance == 2) {
	    T100_enable = data->tch_obj_info.address.T100 + data->T100_size + MXT_T100_CTRL;
	    
	    if ((T100_enable & MXT_T100_ENABLE_BIT_MASK) == 0x01 ){
		    obj_size = data->T100_size;
		} else {
		    dev_info("T100 secondary input device not enabled\n");
		    
		    return 1;
	    }
    }
#endif

    error = mxt_read_reg(data->tch_obj_info.address.T100 + obj_size + MXT_T100_XRANGE,
                         sizeof(range_x), &range_x, data);
    if (error)
        return error;

    error = mxt_read_reg(data->tch_obj_info.address.T100 + obj_size + MXT_T100_YRANGE,
                         sizeof(range_y), &range_y, data);
    if (error)
        return error;

    error =  mxt_read_reg(data->tch_obj_info.address.T100 + obj_size + MXT_T100_CFG1,
                          1, &cfg, data);
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
#endif

#ifdef TOUCH_KEY
static int mxt_read_t15_num_keys(struct mxt_data *data)
{
	u8 xsize, ysize;
	u16 num_keys = 0, offset = 0, instance;
	u8 T15_enable;
	int error = 0;
	
	// maximum instance number is 2
	for (instance = 0; instance < MXT_T15_MAX_INS; instance++) {
		offset = data->T15_size * instance;
		error = mxt_read_reg(data->T15_address + offset + MXT_T15_CTRL,
		                     sizeof(T15_enable), &T15_enable, data);
		if (error) {
			dev_err("read T15 instance(%d) CTRL failed\n", instance);
			return error;
		}

		if ((T15_enable & MXT_T15_ENABLE_BIT_MASK) != 0x01 ){
			dev_info("T15 instance(%d) input device not enabled\n", instance);
		} else {
			/* read first T15 size */
			error = mxt_read_reg(data->T15_address + offset + MXT_T15_XSIZE,
			                     sizeof(xsize), &xsize, data);
			if (error) {
				dev_err("read T15 instance(%d) XSIZE failed\n", instance);
				return error;
			}

			error = mxt_read_reg(data->T15_address +  offset + MXT_T15_YSIZE,
			                     sizeof(ysize), &ysize, data);
			if (error) {
				dev_err("read T15 instance(%d) YSIZE failed\n", instance);
				return error;
			}

			num_keys += xsize * ysize;
			if (instance == 0) {
				data->t15_num_keys_inst0 = num_keys;
			}
			
			data->t15_num_keys_all += num_keys;
			
			dev_info("T15 instance(%d) has %d keys\n", instance, xsize * ysize);
		}
	}
	
	return error;
}
#endif 

#ifdef TOUCH_OBJECT
static int mxt_initialize_input_device(struct mxt_data *data, bool primary)
{
	int error = 0;

#ifdef T9_OBJECT
    error = mxt_read_t9_resolution(data);
    if (error)
        dev_warn("Failed to initialize T9 resolution\n");
#else
    if (primary) {
        error = mxt_read_t100_config(data, 1);
	}
#ifdef SECOND_TOUCH
    else {
	    error = mxt_read_t100_config(data, 2);
	}
#endif
	if (error)
		dev_warn("Failed to read T100 config\n");
#endif

    report_resolution(data->max_x, data->max_y);

	return error;
}
#endif

static int mxt_configure_objects(struct mxt_data *data)
{
	int error;

#ifdef POWER_CONTROL
	error = mxt_init_t7_power_cfg(data);
	if (error) {
		dev_err("Failed to initialize power cfg\n");
		return error;
	}
#endif

#ifdef CONFIG_UPGRADE
	if (data->cfg_length) {
		error = mxt_update_cfg(data);
		if (error)
		    dev_warn("Error %d updating config\n", error);
	}
#endif
	
	error = mxt_check_retrigen(data);
	if (error)
	    dev_err("RETRIGEN Not Enabled or unavailable\n");

#ifdef TOUCH_OBJECT
    dev_info("mxt_init: Registering devices\n");

    error = mxt_initialize_input_device(data, true);
    if (error)
        return error;
		
#ifdef SECOND_TOUCH	
    if (data->T100_instances > 1) {
	    error = mxt_initialize_input_device(data, false);
	    if (error)
	        dev_warn("Error %d registering secondary device\n", error);
	    } else {
	    dev_warn("No touch object detected\n");
    }
#endif
#endif

#ifdef TOUCH_KEY
	if (data->T15_reportid_min) {
		error = mxt_read_t15_num_keys(data);
		if (error) {
			// Set key to zero.
			data->t15_num_keys_inst0 = 0;
			dev_warn("Failed get t15 instance(0) numkeys, set to 0\n");
		}
	}
#endif

	return 0;
}

int mxt_initialize()
{
	struct mxt_data *data = &mxtData;
	int error;
#ifdef BOOTLOADER_PROCESS
    int recovery_attempts = 0;
#endif

    /*************************************************************************************
	 ***********************************Modify Note***************************************
	 1. I2C need to be initialized, 
	 2. RST pin initialized to be output high level,
	 3. CHG pin initialized to be input, 
	 4. External interrupt initialized to be low level trigger in priority, 
	    or falling edge trigger 
	 
	 All of these are done in this example by atmel_start_init() 
	 *************************************************************************************/
	data->chg_completion = 0;
	data->reset_completion = 0;
	data->crc_completion = 0;
#ifdef SECURITY
    data->msg_num.txseq_num = 0x00; //Initialize the TX seq_num
    data->crc_enabled = true;	//Initialize as true to read information block 
#endif
	
	data->i2c_addr = I2C_ADDRESS;

#ifdef CONFIG_UPGRADE
	data->cfg_data = file_cfg_data;
	data->cfg_length = get_cfg_length();
#endif

	while (1) {
		error = mxt_read_info_block(data);
		if (!error)
			break;

#ifdef SECURITY    
        if (data->crc_enabled) {  // resync one time to read information block again
			if (mxt_resync_comm(data)) {
				// resync failed directly exit
				dev_info("Read Info block resync failed, exit");
				return 0;
			}
			// resyc successfully, read inforamtion block again
			error = mxt_read_info_block(data);
			if (!error)
			    break;
		} 
#endif

#ifdef BOOTLOADER_PROCESS
        // HA chips don't need to check bootloader??
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
#endif
	}

	error = mxt_acquire_irq(data);
	if (error)
		return error;
	
	error = mxt_configure_objects(data);
	if (error)
		return error;

#ifdef SELF_TEST
    // test all after initialization, T10/ T25 must be enabled and report enabled in the config
    error = mxt_set_selftest(data, data->test_all_cmd, true);
    if (error != 0) {
	    dev_err("Set Selftest to test all failed, T10/ T25 may be not enabled\n");
	    return error;
    }
#endif

	return 0;
}