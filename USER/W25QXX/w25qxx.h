#ifndef __W25Q256_H__
#define __W25Q256_H__

#include "stdint.h"
#include "spi.h"

/* ================= 配置区 ================= */

/* 是否启用32K擦除（部分型号支持） */
#define W25Q256_USE_32K_ERASE 1

/* ================= 命令定义 ================= */

#define CMD_WRITE_ENABLE      0x06
#define CMD_READ_STATUS1      0x05

#define CMD_PAGE_PROGRAM      0x02
#define CMD_READ_DATA         0x03

#define CMD_SECTOR_ERASE      0x20 // 4K
#define CMD_BLOCK_ERASE_32K   0x52
#define CMD_BLOCK_ERASE_64K   0xD8
#define CMD_CHIP_ERASE        0xC7

#define CMD_JEDEC_ID          0x9F
#define CMD_UNIQUE_ID         0x4B

#define CMD_POWER_DOWN        0xB9
#define CMD_RELEASE_POWERDOWN 0xAB

#define CMD_ENTER_4BYTE       0xB7

/* ================= 外部接口 ================= */

void W25Q256_Init(SPI_HandleTypeDef *hspi);

/* 基本功能 */
uint32_t W25Q256_ReadJEDECID(void);
void W25Q256_ReadUniqueID(uint8_t *id);

void W25Q256_Read(uint32_t addr, uint8_t *buf, uint32_t len);
void W25Q256_Write(uint32_t addr, uint8_t *buf, uint32_t len);

/* 擦除 */
void W25Q256_SectorErase(uint32_t addr);
void W25Q256_BlockErase64K(uint32_t addr);
void W25Q256_ChipErase(void);

/* 自动擦除写入（推荐使用） */
void W25Q256_WriteAutoErase(uint32_t addr, uint8_t *buf, uint32_t len);

/* 低功耗 */
void W25Q256_PowerDown(void);
void W25Q256_WakeUp(void);

/* 状态 */
uint8_t W25Q256_ReadStatus1(void);

#endif