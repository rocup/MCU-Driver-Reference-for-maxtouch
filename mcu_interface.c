
/*
 * mcu_interface.c
 *
 * Created: 06/09/2019 10:50:21
 *  Author: A18425
 */ 
/*************************************************************************************
 ***********************************Modify Note***************************************
 You need to add or modify the system head file according to your MCU and compiler
 *************************************************************************************/
#include <errno.h>        /* use the error type */
#include "driver_init.h"  /* use the driver type */
#include "atmel_mxt_ts.h"
#include "mcu_interface.h"

/*************************************************************************************
 ***********************************Modify Note***************************************
 If there's no config to upgrade, the array should be null; Otherwise, you need to 
 copy the config array using the raw2array tool. Please contact the config provider
 to get the tool.
 *************************************************************************************/
#if 1
u8 file_cfg_data[] = {};
#endif

#if 1
u8 file_cfg_data[] = {
	0x0075,0x0000,0x00F0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0075,0x0001,0x00F0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0075,0x0002,0x00F0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0075,0x0003,0x00F0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0075,0x0004,0x00F0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0075,0x0005,0x00F0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0075,0x0006,0x00F0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0075,0x0007,0x00F0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0075,0x0008,0x00F0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0075,0x0009,0x00F0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0075,0x000A,0x00F0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0075,0x000B,0x00F0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0075,0x000C,0x00F0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0075,0x000D,0x00F0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0025,0x0000,0x0082,0x10,0x19,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0026,0x0000,0x0040,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0047,0x0000,0x00C8,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x0F,0xD8,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x006E,0x0000,0x0068,0x49,0x00,0x31,0x01,0x89,0x01,0xD9,0x01,0x39,0x01,0x19,0x02,0xA1,0x01,0x31,0x02,0x81,0x01,0x39,0x02,0x71,0x01,0xC1,0x01,0x69,0x01,0xA1,0x00,0xA9,0x00,0x71,0x01,0x49,0x01,0x21,0x01,0x19,0x01,0x81,0x01,0xB1,0x00,0xB1,0x00,0xD9,0x00,0x11,0x01,0x09,0x00,0x99,0x00,0x61,0x0A,0xB9,0x09,0x61,0x0A,0xF9,0x09,0x71,0x09,0x31,0x09,0xE1,0x08,0xE1,0x08,0x11,0x08,0xD1,0x08,0x31,0x08,0xA1,0x06,0x51,0x07,0xE9,0x05,0x99,0x05,0x61,0x06,0x59,0x06,0x59,0x05,0x41,0x04,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,
	0x006E,0x0001,0x0068,0x01,0x00,0x61,0x0E,0xA1,0x11,0x81,0x11,0x99,0x11,0x39,0x12,0x29,0x11,0xD9,0x10,0x01,0x10,0x19,0x10,0x81,0x0D,0x59,0x0D,0x11,0x0D,0x41,0x0B,0xD1,0x0A,0x51,0x09,0x29,0x08,0x19,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,
	0x006E,0x0002,0x0068,0xA3,0x01,0x93,0x01,0x8B,0x01,0x8B,0x01,0x93,0x01,0x83,0x01,0x8B,0x01,0x8B,0x01,0x8B,0x01,0x83,0x01,0x8B,0x01,0x8B,0x01,0x8B,0x01,0x93,0x01,0x9B,0x01,0x8B,0x01,0x93,0x01,0x93,0x01,0x93,0x01,0x93,0x01,0x9B,0x01,0x9B,0x01,0x93,0x01,0x93,0x01,0xA3,0x01,0x9B,0x01,0x23,0x01,0x2B,0x01,0x23,0x01,0x23,0x01,0x2B,0x01,0x33,0x01,0x33,0x01,0x33,0x01,0x3B,0x01,0x33,0x01,0x3B,0x01,0x4B,0x01,0x43,0x01,0x5B,0x01,0x5B,0x01,0x53,0x01,0x53,0x01,0x63,0x01,0x6B,0x01,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,
	0x006E,0x0003,0x0068,0x03,0x00,0xCB,0x0F,0xCB,0x0F,0xCB,0x0F,0xCB,0x0F,0xCB,0x0F,0xCB,0x0F,0xCB,0x0F,0xCB,0x0F,0xCB,0x0F,0xCB,0x0F,0xCB,0x0F,0xCB,0x0F,0xCB,0x0F,0xCB,0x0F,0xCB,0x0F,0xCB,0x0F,0xCB,0x0F,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,
	0x0076,0x0000,0x0003,0x00,0x00,0x00,
	0x0007,0x0000,0x0005,0x3C,0x0A,0x32,0x42,0x00,
	0x0008,0x0000,0x000F,0x32,0x00,0x0A,0x0A,0x00,0x00,0x00,0x00,0x0F,0xC4,0x0B,0x01,0x01,0x01,0x80,
	0x000F,0x0000,0x000B,0x03,0x11,0x2D,0x03,0x03,0x00,0x06,0x19,0x02,0x03,0x00,
	0x000F,0x0001,0x000B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0012,0x0000,0x0002,0x00,0x00,
	0x0013,0x0000,0x0006,0x01,0x00,0x00,0x03,0x00,0x00,
	0x0018,0x0000,0x0013,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0019,0x0000,0x0015,0x03,0x00,0xA8,0x61,0x08,0x52,0x94,0x5C,0xF4,0x4C,0x00,0x00,0x00,0x00,0xFA,0xB8,0x0B,0xB8,0x0B,0x00,0x00,
	0x001B,0x0000,0x0007,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x002A,0x0000,0x000E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x002E,0x0000,0x000C,0x80,0x00,0x10,0x18,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,
	0x0038,0x0000,0x0024,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x003D,0x0000,0x0005,0x01,0x00,0x00,0x50,0xC3,
	0x003D,0x0001,0x0005,0x01,0x00,0x00,0x30,0x75,
	0x003D,0x0002,0x0005,0x00,0x00,0x00,0x00,0x00,
	0x003D,0x0003,0x0005,0x00,0x00,0x00,0x00,0x00,
	0x003D,0x0004,0x0005,0x00,0x00,0x00,0x00,0x00,
	0x003D,0x0005,0x0005,0x00,0x00,0x00,0x00,0x00,
	0x0041,0x0000,0x0017,0x81,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0041,0x0001,0x0017,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0041,0x0002,0x0017,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0046,0x0000,0x000A,0x01,0x02,0x00,0x3D,0x00,0x00,0x01,0x01,0x00,0x00,
	0x0046,0x0001,0x000A,0x01,0x02,0x00,0x3D,0x00,0x01,0x01,0x05,0x00,0x00,
	0x0046,0x0002,0x000A,0x01,0x0B,0x00,0x3D,0x00,0x01,0x01,0x09,0x00,0x00,
	0x0046,0x0003,0x000A,0x01,0x0A,0x00,0x3D,0x00,0x01,0x01,0x0D,0x00,0x00,
	0x0046,0x0004,0x000A,0x01,0x0D,0x00,0x08,0x00,0x00,0x06,0x0E,0x00,0x03,
	0x0046,0x0005,0x000A,0x01,0x0D,0x00,0x46,0x00,0x00,0x00,0x12,0x00,0x00,
	0x0046,0x0006,0x000A,0x01,0x0E,0x00,0x46,0x00,0x01,0x00,0x13,0x00,0x00,
	0x0046,0x0007,0x000A,0x01,0x0E,0x00,0x46,0x00,0x02,0x00,0x14,0x00,0x00,
	0x0046,0x0008,0x000A,0x01,0x0E,0x00,0x46,0x00,0x03,0x00,0x15,0x00,0x00,
	0x0046,0x0009,0x000A,0x01,0x0E,0x00,0x08,0x00,0x00,0x0A,0x16,0x00,0x02,
	0x0046,0x000A,0x000A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0046,0x000B,0x000A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0046,0x000C,0x000A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0046,0x000D,0x000A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0046,0x000E,0x000A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0046,0x000F,0x000A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0046,0x0010,0x000A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0046,0x0011,0x000A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0046,0x0012,0x000A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0046,0x0013,0x000A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0048,0x0000,0x0059,0xFD,0x00,0x00,0x01,0x00,0xFF,0x01,0x00,0x00,0x06,0x00,0x00,0x00,0x0A,0x21,0x41,0x0D,0x08,0x00,0x5D,0x01,0x0D,0x23,0x3E,0x4B,0x5C,0x18,0x18,0x18,0x18,0x18,0x10,0x10,0x10,0x10,0x10,0x00,0x00,0x23,0x00,0x07,0x0D,0x23,0x3E,0x4B,0x5C,0x20,0x20,0x20,0x20,0x20,0x18,0x18,0x18,0x18,0x18,0x00,0x05,0x32,0x33,0x4F,0x0D,0x23,0x3E,0x4B,0x5C,0x40,0x40,0x40,0x40,0x40,0x30,0x30,0x30,0x30,0x30,0x00,0x0A,0x00,0x33,0x00,0x00,0x00,0x03,0x03,0x00,0x00,0x00,0x00,
	0x004E,0x0000,0x000C,0x00,0x0F,0x0A,0x03,0x00,0x00,0x20,0x00,0x00,0x32,0x0F,0x04,
	0x004F,0x0000,0x0004,0x00,0x00,0x00,0x00,
	0x0050,0x0000,0x000E,0x00,0x0A,0x64,0x14,0x0A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0064,0x0000,0x0040,0x8F,0xA0,0x00,0x00,0x00,0x00,0x05,0x88,0x00,0x11,0x00,0x06,0x08,0xCF,0x02,0x26,0x2E,0x00,0x00,0x00,0x2D,0x00,0x03,0x03,0x7F,0x07,0x14,0x14,0x08,0x05,0x32,0x0A,0x1E,0x00,0x00,0x0A,0x3C,0x0A,0x00,0x02,0x02,0x02,0x00,0x0A,0x45,0xDC,0x3C,0x05,0x00,0x03,0x00,0x00,0x00,0x05,0xA6,0x2E,0x00,0x00,0x94,0x12,0x00,0x24,0x00,0x00,
	0x0068,0x0000,0x000B,0x01,0x1B,0x5A,0x0F,0x32,0x05,0x1B,0x5A,0x0F,0x32,0x05,
	0x006C,0x0000,0x004B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x006D,0x0000,0x0009,0x00,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,
	0x006F,0x0000,0x001E,0x08,0x08,0x37,0x1E,0x18,0x18,0x0F,0x05,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x37,0x1E,0x05,0x00,0x33,0x46,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x006F,0x0001,0x001E,0x10,0x08,0x37,0x1E,0x20,0x20,0x08,0x0A,0x00,0x1E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x0070,0x0000,0x0005,0x00,0x00,0x00,0x00,0x00,
	0x0071,0x0000,0x0003,0x01,0x00,0x08,
};
#endif

static struct timer_task timer_1ms_task;
static volatile uint32_t timer_1ms_count = 0;

/* get the length of config data here for use */
u16 get_cfg_length(void)
{
	return sizeof(file_cfg_data);
}

/* The timer callback used to count the number */
static void timer_1ms_task_cb(const struct timer_task *const timer_task)
{
	timer_1ms_count++;
}

/*************************************************************************************
 ***********************************Modify Note***************************************
 You need to use a timer to implement this function as your MCU can do. 
 clock 32.77K, set 1ms callback to count to the timeout value.
 *************************************************************************************/
void msleep(uint32_t timeout)
{
	timer_1ms_count = 0;
	
	timer_1ms_task.interval = 1;
	timer_1ms_task.cb       = timer_1ms_task_cb;
	timer_1ms_task.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_0, &timer_1ms_task);
	timer_start(&TIMER_0);
	
	while (timer_1ms_count < timeout) {
		//sleep(4);   /* STANDBY */
	}
	
	timer_remove_task(&TIMER_0, &timer_1ms_task);
	timer_stop(&TIMER_0);
}

/*************************************************************************************
 ***********************************Modify Note***************************************
 You need to use a timer to implement this function as your MCU can do. 
 Wait for the value of completion to vary with time limit.
 *************************************************************************************/
int mxt_wait_for_completion(u8 *completion, unsigned int timeout_ms)
{
	timer_1ms_count = 0;
	
	timer_1ms_task.interval = 33;
	timer_1ms_task.cb       = timer_1ms_task_cb;
	timer_1ms_task.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_0, &timer_1ms_task);
	timer_start(&TIMER_0);
	
	while (timer_1ms_count < timeout_ms && *completion == 0) {
		sleep(4);   /* STANDBY */
	}
	
	timer_remove_task(&TIMER_0, &timer_1ms_task);
    timer_stop(&TIMER_0);
	
	if (*completion == 0) {
		dev_err("Wait for completion timed out.\n");
		return -ETIMEDOUT;
	}
	
	*completion = 0;
	
	return 0;
}

/*************************************************************************************
 ***********************************Modify Note***************************************
 Implemented with your MCU I2C transfer function.
 Read the number of count values from the 10 bit bootloader I2C address.
 *************************************************************************************/
int mxt_bootloader_read(u8 i2c_addr, u8 *val, unsigned int count)
{
	int ret;
	struct _i2c_m_msg msg;
	struct io_descriptor *I2C_Mxt_io;

	i2c_m_sync_get_io_descriptor(&I2C_Mxt, &I2C_Mxt_io);
	i2c_m_sync_enable(&I2C_Mxt);
	i2c_m_sync_set_slaveaddr(&I2C_Mxt, i2c_addr, I2C_M_TEN);

	msg.addr = i2c_addr;
	msg.flags = I2C_M_STOP | I2C_M_RD | I2C_M_TEN;
	msg.len = count;
	msg.buffer = val;

	ret = _i2c_m_sync_transfer(&I2C_Mxt.device, &msg);
	if (ret != 0) {
		dev_err("mxt_bootloader_read: i2c recv failed (%d)\n", ret);
	}

	return ret;
}

/*************************************************************************************
 ***********************************Modify Note***************************************
 Implemented with your MCU I2C transfer function.
 Write the number of count values to the 10 bit bootloader I2C address
 *************************************************************************************/
int mxt_bootloader_write(u8 i2c_addr, const u8 * const val, unsigned int count)
{
	int ret;
	struct _i2c_m_msg msg;
	struct io_descriptor *I2C_Mxt_io;

	i2c_m_sync_get_io_descriptor(&I2C_Mxt, &I2C_Mxt_io);
	i2c_m_sync_enable(&I2C_Mxt);
	i2c_m_sync_set_slaveaddr(&I2C_Mxt, i2c_addr, I2C_M_TEN);

    msg.flags = I2C_M_STOP;
	msg.addr = i2c_addr;
	msg.len = count + 2;
	msg.buffer = (u8 *)val;

	ret = _i2c_m_sync_transfer(&I2C_Mxt.device, &msg);
	if (ret != 0) {
		dev_err("mxt_bootloader_write: i2c send failed (%d)\n", ret);
	}

	return ret;
}

/*************************************************************************************
 ***********************************Modify Note***************************************
 Implemented with your MCU I2C transfer function.
 Read the number of len values from the 7 bit normal address, 16 bit reg.
 *************************************************************************************/
int mxt_read_reg(u8 i2c_addr, u16 reg, u16 len, void *val)
{
	struct _i2c_m_msg msg;
	u8 buf[2];
	int ret;
	bool retry = false;
	struct io_descriptor *I2C_Mxt_io;

	i2c_m_sync_get_io_descriptor(&I2C_Mxt, &I2C_Mxt_io);
	i2c_m_sync_enable(&I2C_Mxt);
	i2c_m_sync_set_slaveaddr(&I2C_Mxt, i2c_addr, I2C_M_SEVEN);

	buf[0] = reg & 0xff;
	buf[1] = (reg >> 8) & 0xff;

	/* Write register */
	retry_read:
	msg.addr   = i2c_addr;
	msg.len    = 2;
	msg.flags  = 0;
	msg.buffer = buf;

	ret = _i2c_m_sync_transfer(&I2C_Mxt.device, &msg);

	if (ret != 0) {
		/* error occurred */
		return ret;
	}

	/* Read data */
	msg.flags  = I2C_M_STOP | I2C_M_RD;
	msg.buffer = val;
	msg.len    = len;

	ret = _i2c_m_sync_transfer(&I2C_Mxt.device, &msg);

	if (ret != 0) {
		/* error occurred */
		if (!retry) {
			dev_dbg("mxt_read_reg: i2c retry\n");
			msleep(MXT_WAKEUP_TIME);
			retry = true;
			goto retry_read;
			} else {
			    dev_err("mxt_read_reg: i2c transfer failed (%d)\n", ret);
			return -EIO;
		}
	}

	return 0;
}

/*************************************************************************************
 ***********************************Modify Note***************************************
 Implemented with your MCU I2C transfer function.
 Write the number of len values to the 7 bit normal address, 16 bit reg, need to 
 write only one time, need to comply to the I2C write sequence in the datasheet, 
 Otherwise it can not write successfully. Please note that the maximum buffer size
 should be MXT_MAX_BLOCK_WRITE add 2.
 *************************************************************************************/
int mxt_write_reg(u8 i2c_addr, u16 reg, u16 len, void *val)
{
	struct _i2c_m_msg msg;
	u8 buf[MXT_MAX_BLOCK_WRITE + 2];
	bool retry = false;
	int32_t           ret;
	struct io_descriptor *I2C_Mxt_io;

	i2c_m_sync_get_io_descriptor(&I2C_Mxt, &I2C_Mxt_io);
	i2c_m_sync_enable(&I2C_Mxt);
	i2c_m_sync_set_slaveaddr(&I2C_Mxt, i2c_addr, I2C_M_SEVEN);
	
	buf[0] = reg & 0xff;
	buf[1] = (reg >> 8) & 0xff;
			
	for (int i = 0; i < len; i++)
		buf[2 + i] = *(u8 *)(val + i);
			
retry_write:
	msg.addr   = i2c_addr;
	msg.buffer = buf;
	msg.flags  = I2C_M_STOP;
	msg.len    = len + 2;

	ret = _i2c_m_sync_transfer(&I2C_Mxt.device, &msg);

	if (ret != 0) {
		/* error occurred */
		if (!retry) {
			dev_dbg("mxt_write_reg: i2c retry\n");
			msleep(MXT_WAKEUP_TIME);
			retry = true;
			goto retry_write;
			} else {
			dev_err("mxt_write_reg: i2c transfer failed (%d)\n", ret);
			return -EIO;
		}
	}
	
	return ret;
}

/*************************************************************************************
 ***********************************Modify Note***************************************
 You need to call the get interrupt trigger type function in your MCU.
 *************************************************************************************/
/* Check the trigger mode of the interrupt */
bool check_low_level_trigger(void)
{
	hri_eic_config_reg_t config = hri_eic_read_CONFIG_reg(EIC, 0);
	
	if (config & EIC_CONFIG_SENSE0(EIC_NMICTRL_NMISENSE_LOW_Val))
	    return true;
		
	return false;
}

/*************************************************************************************
 ***********************************Modify Note***************************************
 You need to call the interrupt callback register function in your MCU.
 *************************************************************************************/
/* Register the maxtouch interrupt callback function to the MCU */
void register_mxt_irq(unsigned int *irq, ext_irq_cb_t mxt_interrupt)
{
    ext_irq_register(PIN_PA16, mxt_interrupt);

    *irq = PIN_PA16;
}

/*************************************************************************************
 ***********************************Modify Note***************************************
 You need to call the interrupt enable function in your MCU  
 *************************************************************************************/
/* Enable the interrupt */
void enable_irq(unsigned int irq)
{
	ext_irq_enable(irq);
}

/*************************************************************************************
 ***********************************Modify Note***************************************
 You need to call the interrupt disable function in your MCU
 *************************************************************************************/
/* Disable the interrupt */
void disable_irq(unsigned int irq)
{
	ext_irq_disable(irq);
}

/*************************************************************************************
 ***********************************Modify Note***************************************
 You must modify these report functions to communicate with the application. If the
 function format need to be modified, remember to modify the caller in atmel_mxt_ts.c.
 *************************************************************************************/
/* Report the state of the points, including type and event and status */
void report_state(u8 type, u8 event)
{
	
}

/* Report the coordinate of x and y to the host */
void report_coordinate(u16 x, u16 y)
{
	
}

/* Report the value of key to the host */
void report_key(u8 value, u8 pressed)
{
	
}

/* Report the resolution of x and y to the host */
void report_resolution(u16 x, u16 y)
{
	
}