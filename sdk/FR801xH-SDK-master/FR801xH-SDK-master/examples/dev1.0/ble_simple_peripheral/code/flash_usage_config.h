/**
 * Copyright (c) 2020, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */

#ifndef _FLASH_USAGE_CONFIG_H
#define _FLASH_USAGE_CONFIG_H

/*
 * MACROS (∫Í∂®“Â)
 */

/*
 * the following MACROS is used to define flash space used in SDK.
 * user can change these values according their applications.
 */
//#define FOR_8M_FLASH		// such as FR8016HD FR8018HD 
#define FOR_4M_FLASH		// such as FR8016HA FR8012HA
//#define FOR_2M_FLASH		// such as FR8012HB

#ifdef FOR_8M_FLASH
	#define JUMP_TABLE_STATIC_KEY_OFFSET    0xFF000
	#define BLE_BONDING_INFO_SAVE_ADDR      0xFD000
	#define BLE_REMOTE_SERVICE_SAVE_ADDR    0xFE000
    #define FLASH_MAX_SIZE                  0x100000
#endif	// FOR_8M_FLASH

#ifdef FOR_4M_FLASH
	#define JUMP_TABLE_STATIC_KEY_OFFSET    0x7F000
	#define BLE_BONDING_INFO_SAVE_ADDR      0x7D000
	#define BLE_REMOTE_SERVICE_SAVE_ADDR    0x7E000
    #define FLASH_MAX_SIZE                  0x80000
#endif	// FOR_4M_FLASH

#ifdef FOR_2M_FLASH
	#define JUMP_TABLE_STATIC_KEY_OFFSET    0x3F000
	#define BLE_BONDING_INFO_SAVE_ADDR      0x3D000
	#define BLE_REMOTE_SERVICE_SAVE_ADDR    0x3E000
    #define FLASH_MAX_SIZE                  0x40000
#endif	//FOR_2M_FLASH

/*
 * uncomment this MACRO if user need protect flash from unexpected erase or write operation
 */
#define FLASH_PROTECT

#endif	// _FLASH_USAGE_CONFIG_H

