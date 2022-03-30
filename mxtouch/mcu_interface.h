
/*
 * mcu_interface.h
 *
 * Created: 06/09/2019 10:50:05
 *  Author: A18425
 */ 

#ifndef __MCU_INTERFACE_H
#define __MCU_INTERFACE_H

/*************************************************************************************
 ***********************************Modify Note***************************************
 You need to add or modify the system head file according to your MCU and compiler
 *************************************************************************************/
#include <stdint.h>         /* used for the types definition */
#include "hal_ext_irq.h"    /* used for interrupt callback function */
#include "hal_atomic.h"     /* used for lock for sequence number operation in security mode */

// some types definition
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

/*************************************************************************************
 ***********************************Modify Note***************************************
 You may need to modify the I2C address according to the hardware design. In most 
 cases, the address will be 0x4A.
 *************************************************************************************/
#define I2C_ADDRESS 0x4A

u16 get_cfg_length(void);

// timer used to wait for completion and delay
void msleep(uint32_t timeout);
int mxt_wait_for_completion(u8 *completion, unsigned int timeout_ms);

int mxt_bootloader_read(u8 i2c_addr, u8 *val, unsigned int count);
int mxt_bootloader_write(u8 i2c_addr, const u8 * const val, unsigned int count);

// irq handler 
bool check_low_level_trigger(void);
void register_mxt_irq(unsigned int *irq, ext_irq_cb_t mxt_interrupt);
void enable_irq(unsigned int irq);
void disable_irq(unsigned int irq);

void report_state(u8 type, u8 pressed);
void report_coordinate(u16 x, u16 y);
void report_key(u8 value, u8 pressed);
void report_resolution(u16 x, u16 y);

int test_bit(int nr, const volatile void * addr);

// debug, error or info to show
extern void dev_info(const char *fmt, ...);
#define dev_dbg dev_info
#define dev_err dev_info
#define dev_warn dev_info

extern uint8_t file_device_info[];
extern uint32_t file_block_info_crc;
extern uint32_t file_cfg_crc;
extern uint8_t file_cfg_data[];

#endif /*__MCU_INTERFACE_H*/