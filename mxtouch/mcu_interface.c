
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
#include <string.h>
#include "driver_init.h"  /* use the driver type */
#include "atmel_mxt_ts.h"
#include "mcu_interface.h"

/*************************************************************************************
 ***********************************Modify Note***************************************
The lock and unlock should be modified based on your MCU atomic definition.
 *************************************************************************************/

#define  LOCK() CRITICAL_SECTION_ENTER()
#define  UNLOCK() CRITICAL_SECTION_LEAVE() 

#ifdef CONFIG_UPGRADE
/*************************************************************************************
 ***********************************Modify Note***************************************
 If there's no config to upgrade, the array should be null; Otherwise, you need to 
 copy the config array using the raw2array tool. Please contact the config provider
 to get the tool.
 *************************************************************************************/
#if 1
uint8_t file_device_info[] = {0,0,0,0,0,0,0};
uint32_t file_block_info_crc = 0;
uint32_t file_cfg_crc = 0;
uint8_t file_cfg_data[] = {};
#endif

#if 0
uint8_t file_device_info[] = {0xA4, 0x19, 0x10, 0xAA, 0x20, 0x14, 0x28};
uint32_t file_block_info_crc = 0xE356C3;
uint32_t file_cfg_crc = 0x82A64E;
uint8_t file_cfg_data[] = {
	0x0044, 0x0000, 0x0049, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0026, 0x0000, 0x0040, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0047, 0x0000, 0x00C8, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x0F, 0xD8, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x006E, 0x0000, 0x0028, 0x11, 0x00, 0x59, 0x07, 0x51, 0x09, 0x19, 0x09, 0x89, 0x0A, 0x71, 0x0B, 0x49, 0x0B, 0xD1, 0x0C, 0x69, 0x0D, 0xE1, 0x0C, 0x89, 0x0E, 0x69, 0x0E, 0x99, 0x0E, 0x51, 0x0F, 0xC9, 0x0F, 0x69, 0x0D, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
	0x006E, 0x0001, 0x0028, 0x01, 0x00, 0xE1, 0x09, 0x41, 0x0B, 0xE9, 0x0A, 0x41, 0x0A, 0x81, 0x09, 0xC9, 0x08, 0x61, 0x08, 0xD9, 0x07, 0x61, 0x04, 0x89, 0x0B, 0x09, 0x0B, 0x49, 0x0B, 0xE9, 0x09, 0x39, 0x09, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
	0x006E, 0x0002, 0x0028, 0x01, 0x00, 0x41, 0x0A, 0xA1, 0x09, 0xA1, 0x09, 0x29, 0x09, 0x91, 0x07, 0xA1, 0x07, 0x51, 0x07, 0xD9, 0x05, 0x29, 0x05, 0xA1, 0x0A, 0x99, 0x09, 0x29, 0x0A, 0x29, 0x08, 0x99, 0x06, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
	0x006E, 0x0003, 0x0028, 0x11, 0x00, 0x51, 0x07, 0x19, 0x09, 0xE9, 0x08, 0x49, 0x0A, 0x39, 0x0B, 0x49, 0x0B, 0xA9, 0x0C, 0x89, 0x0D, 0x01, 0x0D, 0x81, 0x0E, 0x91, 0x0E, 0x91, 0x0E, 0x69, 0x0F, 0xB9, 0x0F, 0x69, 0x0D, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
	0x006E, 0x0004, 0x0028, 0x01, 0x00, 0xE1, 0x09, 0x51, 0x0B, 0xF1, 0x0A, 0x61, 0x0A, 0x91, 0x09, 0xD1, 0x08, 0x61, 0x08, 0xD9, 0x07, 0x41, 0x04, 0x91, 0x0B, 0xC9, 0x0A, 0x49, 0x0B, 0xD1, 0x09, 0x29, 0x09, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
	0x006E, 0x0005, 0x0028, 0x01, 0x00, 0x11, 0x0A, 0xA1, 0x09, 0x89, 0x09, 0x39, 0x09, 0x89, 0x07, 0xA1, 0x07, 0x51, 0x07, 0xD1, 0x05, 0x19, 0x05, 0xA1, 0x0A, 0x99, 0x09, 0x31, 0x0A, 0x39, 0x08, 0xA9, 0x06, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
	0x006E, 0x0006, 0x0028, 0xCB, 0x0F, 0xCB, 0x0F, 0xCB, 0x0F, 0xCB, 0x0F, 0xCB, 0x0F, 0xCB, 0x0F, 0xCB, 0x0F, 0xCB, 0x0F, 0xCB, 0x0F, 0xCB, 0x0F, 0xCB, 0x0F, 0xCB, 0x0F, 0xCB, 0x0F, 0xCB, 0x0F, 0xCB, 0x0F, 0xCB, 0x0F, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00,
	0x006E, 0x0007, 0x0028, 0x03, 0x00, 0x13, 0x01, 0x03, 0x01, 0x03, 0x01, 0x13, 0x01, 0x13, 0x01, 0x1B, 0x01, 0x23, 0x01, 0x2B, 0x01, 0x53, 0x01, 0xF3, 0x00, 0x03, 0x01, 0x03, 0x01, 0x13, 0x01, 0x1B, 0x01, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00,
	0x006E, 0x0008, 0x0028, 0x03, 0x00, 0x03, 0x01, 0x0B, 0x01, 0x0B, 0x01, 0x13, 0x01, 0x1B, 0x01, 0x23, 0x01, 0x23, 0x01, 0x33, 0x01, 0x3B, 0x01, 0xF3, 0x00, 0x03, 0x01, 0x03, 0x01, 0x1B, 0x01, 0x2B, 0x01, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00,
	0x0076, 0x0000, 0x0003, 0x00, 0x00, 0x00,
	0x0007, 0x0000, 0x0005, 0xFF, 0xFF, 0x32, 0x42, 0x00,
	0x0008, 0x0000, 0x000F, 0x32, 0x00, 0x05, 0x05, 0xC8, 0x00, 0x00, 0x00, 0x0F, 0xD8, 0x0B, 0x01, 0x01, 0x01, 0x80,
	0x000F, 0x0000, 0x000B, 0x03, 0x1A, 0x0E, 0x02, 0x02, 0x00, 0x0E, 0x46, 0x02, 0x0F, 0x00,
	0x000F, 0x0001, 0x000B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0012, 0x0000, 0x0002, 0x00, 0x00,
	0x0013, 0x0000, 0x0010, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0018, 0x0000, 0x0013, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0019, 0x0000, 0x0016, 0x02, 0x00, 0xCC, 0x5B, 0x2C, 0x4C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0xD0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x001B, 0x0000, 0x0007, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0028, 0x0000, 0x0005, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x002A, 0x0000, 0x000D, 0x00, 0x00, 0x32, 0x23, 0xC3, 0x05, 0x00, 0x00, 0x01, 0x00, 0x19, 0x04, 0x00,
	0x002E, 0x0000, 0x000C, 0x02, 0x00, 0x10, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00,
	0x002F, 0x0000, 0x002E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0038, 0x0000, 0x0024, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x003D, 0x0000, 0x0005, 0x01, 0x00, 0x00, 0x50, 0xC3,
	0x003D, 0x0001, 0x0005, 0x01, 0x00, 0x00, 0x30, 0x75,
	0x003D, 0x0002, 0x0005, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x003D, 0x0003, 0x0005, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x003D, 0x0004, 0x0005, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x003D, 0x0005, 0x0005, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0041, 0x0000, 0x0017, 0x81, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0041, 0x0001, 0x0017, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0041, 0x0002, 0x0017, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0046, 0x0000, 0x000A, 0x00, 0x02, 0x00, 0x3D, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x0046, 0x0001, 0x000A, 0x00, 0x02, 0x00, 0x3D, 0x00, 0x01, 0x01, 0x05, 0x00, 0x00,
	0x0046, 0x0002, 0x000A, 0x00, 0x0B, 0x00, 0x3D, 0x00, 0x01, 0x01, 0x09, 0x00, 0x00,
	0x0046, 0x0003, 0x000A, 0x00, 0x0A, 0x00, 0x3D, 0x00, 0x01, 0x01, 0x0D, 0x00, 0x00,
	0x0046, 0x0004, 0x000A, 0x01, 0x0D, 0x00, 0x08, 0x00, 0x00, 0x06, 0x0E, 0x00, 0x03,
	0x0046, 0x0005, 0x000A, 0x01, 0x0D, 0x00, 0x46, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00,
	0x0046, 0x0006, 0x000A, 0x01, 0x0E, 0x00, 0x46, 0x00, 0x01, 0x00, 0x13, 0x00, 0x00,
	0x0046, 0x0007, 0x000A, 0x01, 0x0E, 0x00, 0x46, 0x00, 0x02, 0x00, 0x14, 0x00, 0x00,
	0x0046, 0x0008, 0x000A, 0x01, 0x0E, 0x00, 0x46, 0x00, 0x03, 0x00, 0x15, 0x00, 0x00,
	0x0046, 0x0009, 0x000A, 0x01, 0x0E, 0x00, 0x08, 0x00, 0x00, 0x0A, 0x16, 0x00, 0x02,
	0x0046, 0x000A, 0x000A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0046, 0x000B, 0x000A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0046, 0x000C, 0x000A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0046, 0x000D, 0x000A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0046, 0x000E, 0x000A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0046, 0x000F, 0x000A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0046, 0x0010, 0x000A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0046, 0x0011, 0x000A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0046, 0x0012, 0x000A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0046, 0x0013, 0x000A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0048, 0x0000, 0x0055, 0xFD, 0x00, 0x00, 0x01, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x0A, 0x20, 0x5A, 0x08, 0x0E, 0x00, 0x5D, 0x01, 0x37, 0x19, 0x05, 0x50, 0x6E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x1E, 0x00, 0x01, 0x37, 0x19, 0x05, 0x50, 0x6E, 0x20, 0x20, 0x20, 0x20, 0x20, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x0A, 0x2D, 0x58, 0x01, 0x37, 0x19, 0x05, 0x50, 0x6E, 0x30, 0x30, 0x30, 0x30, 0x30, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x08, 0x00, 0x58, 0x00, 0x00, 0x00, 0x03, 0x02,
	0x004D, 0x0000, 0x0002, 0x00, 0x00,
	0x004E, 0x0000, 0x000C, 0x80, 0x05, 0x03, 0x01, 0x0A, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x004F, 0x0000, 0x0004, 0x00, 0x00, 0x00, 0x00,
	0x004F, 0x0001, 0x0004, 0x00, 0x00, 0x00, 0x00,
	0x004F, 0x0002, 0x0004, 0x00, 0x00, 0x00, 0x00,
	0x0050, 0x0000, 0x000C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0054, 0x0000, 0x0004, 0x00, 0x00, 0x00, 0x00,
	0x0064, 0x0000, 0x003C, 0x8F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x19, 0x2A, 0x00, 0x00, 0xFF, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x2A, 0x00, 0x00, 0xCF, 0x02, 0x00, 0x00, 0x0E, 0x0B, 0x46, 0x0F, 0x28, 0x00, 0x00, 0x0A, 0x3C, 0x0A, 0x00, 0x02, 0x02, 0x02, 0x00, 0x0A, 0x42, 0xDC, 0x28, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0065, 0x0000, 0x001E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0068, 0x0000, 0x000B, 0x01, 0x18, 0x50, 0x0F, 0x32, 0x08, 0x14, 0x50, 0x0F, 0x32, 0x08,
	0x006C, 0x0000, 0x004B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x006D, 0x0000, 0x0009, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
	0x006F, 0x0000, 0x001B, 0x80, 0x08, 0x3C, 0x2D, 0x18, 0x18, 0x05, 0x05, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x33, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x006F, 0x0001, 0x001B, 0x0C, 0x08, 0x2D, 0x37, 0x40, 0x40, 0x0F, 0x05, 0x00, 0xFF, 0x23, 0x02, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x46, 0x1E, 0x00, 0x35, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x006F, 0x0002, 0x001B, 0x00, 0x08, 0x3C, 0x2D, 0x18, 0x18, 0x0F, 0x05, 0x00, 0xFF, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0070, 0x0000, 0x0005, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0070, 0x0001, 0x0005, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0071, 0x0000, 0x0003, 0x01, 0x08, 0x00,
};
#endif

/* get the length of config data here for use */
u16 get_cfg_length(void)
{
	return sizeof(file_cfg_data);
}
#endif 

/*************************************************************************************
 ***********************************Modify Note***************************************
 You may need to use the delay function that your MCU implemented
 *************************************************************************************/
void msleep(uint32_t timeout)
{
	delay_ms(timeout);
}

/*************************************************************************************
 ***********************************Modify Note***************************************
 You need to use a timer to implement this function as your MCU can do. 
 Wait for the value of completion to vary with time limit.
 *************************************************************************************/
int mxt_wait_for_completion(u8 *completion, unsigned int timeout_ms)
{
	unsigned int  timer_1ms_count = 0;
	
	while (timer_1ms_count < timeout_ms && *completion == 0) {
		msleep(1);   /* delay for 1ms */
	}
	
	if (*completion == 0) {
		dev_err("Wait for completion timed out.\n");
		return -ETIMEDOUT;
	}
	
	*completion = 0;
	
	return 0;
}

#ifdef BOOTLOADER_PROCESS
/*************************************************************************************
 ***********************************Modify Note***************************************
 Implemented with your MCU I2C transfer function.
 Read the number of count values from the 7 bit bootloader I2C address.
 *************************************************************************************/
int mxt_bootloader_read(u8 i2c_addr, u8 *val, unsigned int count)
{
	int ret;
	struct _i2c_m_msg msg;
	struct io_descriptor *I2C_Mxt_io;

	i2c_m_sync_get_io_descriptor(&I2C_Mxt, &I2C_Mxt_io);
	i2c_m_sync_enable(&I2C_Mxt);
	i2c_m_sync_set_slaveaddr(&I2C_Mxt, i2c_addr, I2C_M_SEVEN);

	msg.addr = i2c_addr;
	msg.flags = I2C_M_STOP | I2C_M_RD;
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
	i2c_m_sync_set_slaveaddr(&I2C_Mxt, i2c_addr, I2C_M_SEVEN);

    msg.flags = I2C_M_STOP;
	msg.addr = i2c_addr;
	msg.len = count;
	msg.buffer = (u8 *)val;

	ret = _i2c_m_sync_transfer(&I2C_Mxt.device, &msg);
	if (ret != 0) {
		dev_err("mxt_bootloader_write: i2c send failed (%d)\n", ret);
	}

	return ret;
}
#endif

/*************************************************************************************
 ***********************************Modify Note***************************************
 Implemented with your MCU I2C transfer function.
 Read the number of len values from the 7 bit normal address, 16 bit reg.
 Write addr: 2 byte address
 Read Data: retry for one time
 *************************************************************************************/
int mxt_read_reg_normal(u8 i2c_addr, u16 reg, u16 len, void *val)
{
	struct _i2c_m_msg msg;
	u8 buf[2];
	int ret;
	bool retry = false;
	struct io_descriptor *I2C_Mxt_io;

    LOCK();
	
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
		goto i2c_end;
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
			ret = -EIO;
			goto i2c_end;
		}
	}

i2c_end:
	UNLOCK();

	return ret;
}

/*************************************************************************************
 ***********************************Modify Note***************************************
 Implemented with your MCU I2C transfer function.
 Write the number of len values to the 7 bit normal address, 16 bit reg, need to 
 write only one time, need to comply to the I2C write sequence in the datasheet, 
 Otherwise it can not write successfully. Please note that the maximum buffer size
 should be MXT_MAX_BLOCK_WRITE add 2.
 Write process: 2 byte address, data (max length is MXT_MAX_BLOCK_WRITE)
 *************************************************************************************/
int mxt_write_reg_normal(u8 i2c_addr, u16 reg, u16 len, void *val)
{
	struct _i2c_m_msg msg;
	u8 buf[MXT_MAX_BLOCK_WRITE + 2];
	bool retry = false;
	int32_t           ret;
	struct io_descriptor *I2C_Mxt_io;

    LOCK();
	
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
			ret =  -EIO;
		}
	}
	
	UNLOCK();
	
	return ret;
}

#ifdef SECURITY
// update the sequence number based on the flag 
static u8 mxt_update_seq_num(struct mxt_data *data, bool reset_counter, u8 counter_value)
{
	u8 current_val;

	current_val = data->msg_num.txseq_num;

	if (reset_counter) {
		data->msg_num.txseq_num = counter_value;
		} else {
		data->msg_num.txseq_num++;
	}

	return current_val;
}

// update the sequence number with lock
u8 mxt_update_seq_num_lock(struct mxt_data *data, bool reset_counter, u8 counter_value)
{
	u8 val;

	LOCK();

	val = mxt_update_seq_num(data, reset_counter, counter_value);

	UNLOCK();
	
	return val;
}

// 8 bit crc calculation for message check
static u8 mxt_calc_crc8(unsigned char crc, unsigned char data)
{

	static const u8 crcpoly = 0x8C;
	u8 index;
	u8 fb;
	index = 8;
	
	do {
		fb = (crc ^ data) & 0x01;
		data >>= 1;
		crc >>= 1;
		if (fb)
		crc ^= crcpoly;
	} while (--index);
	
	return crc;
}

// set reset pin for hardware reset
void set_reset_pin_level(bool level)
{
	gpio_set_pin_level(RST, level);
}

/*************************************************************************************
 ***********************************Modify Note***************************************
 Implemented with your MCU I2C transfer function.
 Read the number of len values with crc from the 7 bit normal address, 16 bit reg.
 Write addr: 2 byte address, 1 byte sequence number, 1 byte 8 bit crc
 Read Data: if it is T5 or T144's address, need to check the crc of all the data 
 *************************************************************************************/
int mxt_read_reg_crc(u16 reg, u16 len, void *val, struct mxt_data *data)
{
	struct _i2c_m_msg msg;
	u8 buf[4];
	int ret;
	bool retry = false;
	struct io_descriptor *I2C_Mxt_io;
	size_t count;
	int i;
	u8 crc_data = 0;
	char *ptr_data;
	
    LOCK();

	//Set address pointer
	count = 4;	//16bit addr, tx_seq_num, 8bit crc
	
	i2c_m_sync_get_io_descriptor(&I2C_Mxt, &I2C_Mxt_io);
	ret = i2c_m_sync_enable(&I2C_Mxt);
	i2c_m_sync_set_slaveaddr(&I2C_Mxt, data->i2c_addr, I2C_M_SEVEN);

retry_read:
	buf[0] = reg & 0xff;
	buf[1] = (reg >> 8) & 0xff;
	buf[2] = mxt_update_seq_num(data, false, 0);

	for (i = 0; i < (count - 1); i++) {
		crc_data = mxt_calc_crc8(crc_data, buf[i]);
	}

	buf[3] = crc_data;

	/* Write register */
	msg.addr   = data->i2c_addr;
	msg.len    = count;
	msg.flags  = 0;
	msg.buffer = buf;

	ret = _i2c_m_sync_transfer(&I2C_Mxt.device, &msg);

	if (ret != 0) {
		/* error occurred */
        /* If the write reg address failed, that means slave may not get the packet, reverse the seq num 
			Note this is not granteed operation, that meanings seq num might mis-matched
		*/ 
		mxt_update_seq_num(data, true, buf[2]);
		goto i2c_end;
	}

	/* Read data */
	msg.flags  = I2C_M_STOP | I2C_M_RD;
	msg.buffer = val;
	msg.len    = len;

	ret = _i2c_m_sync_transfer(&I2C_Mxt.device, &msg);

	if (ret == 0) {
		ptr_data = val;

        // only T5 and T144 need to check 8 bit CRC, 
		// if reg equals 0, T5 and T144 have not get the parsed address yet
		if (reg != 0 && (reg == data->T5_address || reg == data->count_address.T144)) {
			crc_data = 0;
			for (i = 0; i < (len - 1); i++) {
				crc_data = mxt_calc_crc8(crc_data, ptr_data[i]);
				//dev_dbg("Data = [%x], crc8 =  %x\n", ((char *) ptr_data)[i], crc_data);
			}

			if (crc_data == ptr_data[len - 1]){
				//dev_dbg("T5 Read CRC Passed\n");
			}
			else {
				//dev_dbg("T5 Read CRC Failed\n");
				ret = -EIO;       // return error for resync
				goto i2c_end;
			}
		}
		ret = 0;

	} else {
		/* error occurred */
		if (!retry) {
			dev_dbg("\nmxt_read_reg: i2c retry\n");
			msleep(MXT_WAKEUP_TIME);
			retry = true;
			goto retry_read;
		} else {
			    dev_err("mxt_read_reg: i2c transfer failed (%d)\n", ret);
			ret = -EIO;
			goto i2c_end;
		}
	}

i2c_end:
	UNLOCK();
	
	return ret;
}

/*************************************************************************************
 ***********************************Modify Note***************************************
 Implemented with your MCU I2C transfer function.
 Write the number of len values to the 7 bit normal address, 16 bit reg, need to 
 write only one time, need to comply to the I2C write sequence in the datasheet, 
 Otherwise it can not write successfully. Please note that the maximum buffer size
 should be MXT_MAX_BLOCK_WRITE add 4.
 Write process: 2 byte address, max 11 bytes data, 1 byte sequence number, 1 byte 8 bit crc
 *************************************************************************************/
int mxt_write_reg_crc(u16 reg, u16 length, void *val, struct mxt_data *data)
{
	u8 msgbuf[15];
	struct _i2c_m_msg msg;
	u8 databuf[MXT_MAX_BLOCK_WRITE + 4];
	//bool retry = false;
	int32_t           ret;
	struct io_descriptor *I2C_Mxt_io;
	//size_t len;
	size_t msg_count;
	u8 crc_data = 0;
	int i, j;
	u16 retry_counter = 0;
	u16 bytesToWrite = 0;
	u16 write_addr = 0;
	u16 bytesWritten = 0;
	u8 max_data_length = 11;
	volatile u16 message_length = 0;
	
	LOCK();
	
	i2c_m_sync_get_io_descriptor(&I2C_Mxt, &I2C_Mxt_io);
	i2c_m_sync_enable(&I2C_Mxt);
	i2c_m_sync_set_slaveaddr(&I2C_Mxt, data->i2c_addr, I2C_M_SEVEN);
	
	//len = length + 2;
	bytesToWrite = length;

	//Limit size of data packet
	if (length > max_data_length){
		message_length = 11;
	} else {
		message_length = length;
	}

	msg_count = message_length + 4;
	
	if (!(length == 0x00))	//Need this or else memory crash
	    memcpy(&databuf[0], val, length);	//Copy only first message to databuf

	do {
		write_addr = reg + bytesWritten;
		//dev_dbg("Reg address %x\n", write_addr);

		msgbuf[0] = write_addr & 0xff;
		msgbuf[1] = (write_addr >> 8) & 0xff;
		msgbuf[msg_count - 2] = mxt_update_seq_num(data, false, 0);

		j = 0;

		while (j < message_length){
			//Copy current messasge into msgbuffer
			msgbuf[2 + j] = databuf[bytesWritten + j];
			j++;
		}

		crc_data = 0;

		for (i = 0; i < (msg_count - 1); i++) {
			crc_data = mxt_calc_crc8(crc_data, msgbuf[i]);

			//dev_dbg("Write CRC: Data[%d] = %x, crc = 0x%x\n", i, msgbuf[i], crc_data);
		}
		
		msgbuf[msg_count - 1] = crc_data;

	//retry_write:
	    msg.addr   = data->i2c_addr;
	    msg.buffer = msgbuf;
	    msg.flags  = I2C_M_STOP;
	    msg.len    = msg_count;

	    ret = _i2c_m_sync_transfer(&I2C_Mxt.device, &msg);
		
		if (ret == 0) {
			bytesWritten = bytesWritten + message_length;	//Track only bytes in buf
			bytesToWrite = bytesToWrite - message_length;

			//dev_info("bytesWritten %i, bytesToWrite %i\n", bytesWritten, bytesToWrite);
			retry_counter = 0;

			if (bytesToWrite < message_length) {
				message_length = bytesToWrite;
				msg_count = message_length + 4;
			}
		} else {
			ret = -EIO;
			dev_err("%s: i2c send failed (%d)\n", __func__, ret);
			/* If the i2c-write failed, that means slave may not get the packet, reverse the seq num 
				Note this is not granteed operation, that meanings seq num might mis-matched
			*/
			mxt_update_seq_num(data, true, msgbuf[msg_count - 2]);
		}

		retry_counter++;

		if (retry_counter == 10)
		    break;

	} while (bytesToWrite > 0);		
	
	UNLOCK();
	
	return ret;
}
#endif

int mxt_read_reg(u16 reg, u16 len, void *val, struct mxt_data *data)
{
#ifdef SECURITY
	if (data->crc_enabled) {    // use I2C sequence and crc to read as default
		return mxt_read_reg_crc(reg, len, val, data);
	} else 
#endif
	{                           // normal I2C read 
		return mxt_read_reg_normal(data->i2c_addr, reg, len, val);
	}
}

int mxt_write_reg(u16 reg, u16 length, void *val, struct mxt_data *data)
{
#ifdef SECURITY
	if (data->crc_enabled) {    // use I2C sequence and crc to write as default
		return mxt_write_reg_crc(reg, length, val, data);
	} else
#endif
    {                           // normal I2C write
		return mxt_write_reg_normal(data->i2c_addr, reg, length, val);
	}
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
    ext_irq_register(PIN_PB05, mxt_interrupt);

    *irq = PIN_PB05;
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

#ifdef TOUCH_OBJECT
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

/* Report the resolution of x and y to the host */
void report_resolution(u16 x, u16 y)
{
	
}
#endif

#ifdef TOUCH_KEY
/* Report the value of key to the host */
void report_key(u8 value, u8 pressed)
{
	
}
#endif

#ifdef SECOND_TOUCH
/* Report the id of touch to separate the second touch instance */
void report_id(u8 id)
{
	
}

/* Report the special status of the second touch instance */
void report_single_touch()
{
	
}
#endif