#ifndef __FLASH_DRIVER_H
#define __FLASH_DRIVER_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

/* Flash地址范围 */
#define STM32_FLASH_BASE 0x08000000
#define STM32_FLASH_END  0x08100000

/* 用户可写区域起始地址 */
#define STM32_FLASH_USER_START 0x08030000

/* 最大sector大小 */
#define FLASH_MAX_SECTOR_SIZE (128 * 1024)

/* ===================== */
/* Sector操作 */
/* ===================== */

uint32_t Flash_GetSector(uint32_t addr);
uint32_t Flash_GetSectorStart(uint32_t sector);
uint32_t Flash_GetSectorSize(uint32_t sector);

/* ===================== */
/* 基本读写 */
/* ===================== */

uint32_t STMFLASH_ReadWord(uint32_t addr);

uint8_t STMFLASH_Read(uint32_t addr, uint8_t *buf, uint32_t len);

uint8_t STMFLASH_Write(uint32_t addr, uint8_t *buf, uint32_t len);

#endif