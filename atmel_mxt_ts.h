/*
 * atmel_mxt_ts.h
 *
 * Created: 30/08/2019 16:48:12
 *  Author: A18425
 */ 

#ifndef __ATMEL_MXT_TS_H
#define __ATMEL_MXT_TS_H

#include "mcu_interface.h"

/*************************************************************************************
 ***********************************Modify Note***************************************
 If the project has keys, you need to define the macro TOUCH_KEY.
 If the IC you used is an old part, T44 maybe not existed, you need to define T44_NONE;
 and T100 maybe not existed, you need to define T9_OBJECT.
 *************************************************************************************/
//#define TOUCH_KEY         // used for touch screen with touch keys
//#define T9_OBJECT         // used for old IC using T9 for reporting points
//#define T44_NONE          // used for old IC without T44

/* Registers */
#define MXT_OBJECT_START	0x07
#define MXT_OBJECT_SIZE		6
#define MXT_INFO_CHECKSUM_SIZE	3
/*************************************************************************************
 ***********************************Modify Note***************************************
 You may need to modify MXT_MAX_BLOCK_WRITE(the maximum number of data I2C write one
 time can do) according to the CPU capability.
 You may need to modify the buffer of MAX_MSG_SIZE, you may need to read more messages
 one time. You can only read a message one time with the value 16. If you need to 
 modify this value, please contact the driver provider.
 *************************************************************************************/
#define MXT_MAX_BLOCK_WRITE	255
#define MAX_MSG_SIZE 16           

/* Object types */
#define MXT_GEN_MESSAGE_T5		5
#define MXT_GEN_COMMAND_T6		6
#define MXT_GEN_POWER_T7		7
#define MXT_GEN_ACQUIRE_T8		8
#define MXT_TOUCH_MULTI_T9		9
#define MXT_TOUCH_KEYARRAY_T15		15
#define MXT_SPT_COMMSCONFIG_T18		18
#define MXT_SPT_GPIOPWM_T19		19
#define MXT_SPT_SELFTEST_T25		25
#define MXT_SPT_USERDATA_T38		38
#define MXT_SPT_SERIALDATACOMMAND_T68	68
#define MXT_TOUCH_MULTITOUCHSCREEN_T100 100
#define MXT_SPT_DATACONTAINER_T117		117
#define MXT_SPT_MESSAGECOUNT_T44	44
#define MXT_SPT_DYNAMICCONFIGURATIONCONTAINER_T71 71
#define MXT_PROCI_KEYTHRESHOLD_T14 14

/* MXT_GEN_MESSAGE_T5 object */
#define MXT_RPTID_NOMSG		0xff

/* MXT_GEN_COMMAND_T6 field */
#define MXT_COMMAND_RESET	0
#define MXT_COMMAND_BACKUPNV	1
#define MXT_COMMAND_CALIBRATE	2
#define MXT_COMMAND_REPORTALL	3
#define MXT_COMMAND_DIAGNOSTIC	5

#define BIT(x)	(1 << (x))
/* Define for T6 status byte */
#define MXT_T6_STATUS_RESET	BIT(7)
#define MXT_T6_STATUS_OFL	BIT(6)
#define MXT_T6_STATUS_SIGERR	BIT(5)
#define MXT_T6_STATUS_CAL	BIT(4)
#define MXT_T6_STATUS_CFGERR	BIT(3)
#define MXT_T6_STATUS_COMSERR	BIT(2)

/*************************************************************************************
 ***********************************Modify Note***************************************
 There're some structures that need to be packed. You may need to modify the packed
 key word according to the compiler.
 *************************************************************************************/
/* MXT_GEN_POWER_T7 field */
struct t7_config {
	u8 idle;
	u8 active;
} __attribute__((__packed__));

#define MXT_POWER_CFG_RUN		0
#define MXT_POWER_CFG_DEEPSLEEP		1

/* MXT_SPT_COMMSCONFIG_T18 */
#define MXT_COMMS_CTRL		0
#define MXT_COMMS_CMD		1
#define MXT_COMMS_RETRIGEN      BIT(6)

/* Define for MXT_GEN_COMMAND_T6 */
#define MXT_BOOT_VALUE		0xa5
#define MXT_RESET_VALUE		0x01
#define MXT_BACKUP_VALUE	0x55

#ifdef T9_OBJECT

/* MXT_TOUCH_MULTI_T9 field */
#define MXT_T9_CTRL		0
#define MXT_T9_ORIENT		9
#define MXT_T9_RANGE		18

/* MXT_TOUCH_MULTI_T9 status */
#define MXT_T9_UNGRIP		BIT(0)
#define MXT_T9_SUPPRESS		BIT(1)
#define MXT_T9_AMP		    BIT(2)
#define MXT_T9_VECTOR		BIT(3)
#define MXT_T9_MOVE		    BIT(4)
#define MXT_T9_RELEASE		BIT(5)
#define MXT_T9_PRESS		BIT(6)
#define MXT_T9_DETECT		BIT(7)

struct t9_range {
	u16 x;
	u16 y;
} __attribute__((__packed__));

/* MXT_TOUCH_MULTI_T9 orient */
#define MXT_T9_ORIENT_SWITCH	BIT(0)

#else

/* T100 Multiple Touch Touchscreen */
#define MXT_T100_CTRL		0
#define MXT_T100_CFG1		1
#define MXT_T100_XRANGE		13
#define MXT_T100_YRANGE		24

#define MXT_T100_CFG_SWITCHXY	BIT(5)

#define MXT_T100_DETECT		BIT(7)
#define MXT_T100_TYPE_MASK	0x70
#define MXT_T100_EVENT_MASK	0x0F

enum t100_type {
	MXT_T100_TYPE_FINGER		= 1,
	MXT_T100_TYPE_PASSIVE_STYLUS	= 2,
	MXT_T100_TYPE_ACTIVE_STYLUS	= 3,
	MXT_T100_TYPE_HOVERING_FINGER	= 4,
	MXT_T100_TYPE_GLOVE		= 5,
	MXT_T100_TYPE_LARGE_TOUCH	= 6,
};

#endif

/* Delay times */
#define MXT_BACKUP_TIME		50	/* msec */
#define MXT_RESET_TIME		200	/* msec */
#define MXT_RESET_TIMEOUT	3000	/* msec */
#define MXT_CRC_TIMEOUT		1000	/* msec */
#define MXT_FW_RESET_TIME	3000	/* msec */
#define MXT_WAKEUP_TIME		25	/* msec */

/* Command to unlock bootloader */
#define MXT_UNLOCK_CMD_MSB	0xaa
#define MXT_UNLOCK_CMD_LSB	0xdc

/* Bootloader mode status */
#define MXT_WAITING_BOOTLOAD_CMD	0xc0	/* valid 7 6 bit only */
#define MXT_WAITING_FRAME_DATA	0x80	/* valid 7 6 bit only */
#define MXT_FRAME_CRC_CHECK	0x02
#define MXT_FRAME_CRC_FAIL	0x03
#define MXT_FRAME_CRC_PASS	0x04
#define MXT_APP_CRC_FAIL	0x40	/* valid 7 8 bit only */
#define MXT_BOOT_STATUS_MASK	0x3f
#define MXT_BOOT_EXTENDED_ID	BIT(5)
#define MXT_BOOT_ID_MASK	0x1f

#define TEST_BIT(nr, addr) ((1UL & (((const int *) addr)[nr >> 5] >> (nr & 31))) != 0UL)

struct mxt_info {
	u8 family_id;
	u8 variant_id;
	u8 version;
	u8 build;
	u8 matrix_xsize;
	u8 matrix_ysize;
	u8 object_num;
};

struct mxt_object {
	u8 type;
	u16 start_address;
	u8 size_minus_one;
	u8 instances_minus_one;
	u8 num_report_ids;
} __attribute__((__packed__));

struct crc_control {
    u32 start_off;
	u32 end_off;
	u8 last_data;
	bool last_odd;
	bool data_end;	
}__attribute__((__packed__));

/* Each client has this additional data */
struct mxt_data {
	u8 i2c_addr;
	struct mxt_info info;
	unsigned int irq;
	unsigned int max_x;
	unsigned int max_y;
	bool in_bootloader;
	u8 max_reportid;
	u32 config_crc;
	u32 info_crc;
	u8 bootloader_addr;
	u8 msg_buf[MAX_MSG_SIZE];
	u8 t6_status;
	u8 last_message_count;
	u8 num_touchids;
	struct t7_config t7_cfg;
	bool use_retrigen_workaround;
	u8 *cfg_data;
	u16 cfg_length;
	
#ifdef TOUCH_KEY
    unsigned long t15_keystatus;
    u8 t15_keys_instance0;
	u8 t15_num_keys;
#endif
	/* Cached parameters from object table */
	u16 T5_address;
#ifndef T44_NONE
	u16 T44_address;
#endif
	u8 T5_msg_size;
	u8 T6_reportid;
	u16 T6_address;
	u16 T7_address;
#ifdef TOUCH_KEY
	u8 T15_reportid_min;
	u8 T15_reportid_max;
#endif
	u16 T18_address;
	u8 T19_reportid;
#ifdef T9_OBJECT
	u8 T9_reportid_min;
	u8 T9_reportid_max;
	u16 T9_address;
#else
    u8 T100_reportid_min;
    u8 T100_reportid_max;
    u16 T100_address;
#endif
	u16 T38_address;
	u16 T117_address;
	u16 T68_address;

	/* for reset handling */
	u8 reset_completion;

	/* for config update handling */
	u8 crc_completion;

	/* for power up handling */
	u8 chg_completion;
};

int mxt_initialize();

#endif /* __ATMEL_MXT_TS_H */