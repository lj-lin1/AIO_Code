#include "flash.h"
#include <string.h>

/* ============================= */
/* 获取sector */
/* ============================= */

uint32_t Flash_GetSector(uint32_t addr)
{
    if (addr < 0x08004000)
        return FLASH_SECTOR_0;
    else if (addr < 0x08008000)
        return FLASH_SECTOR_1;
    else if (addr < 0x0800C000)
        return FLASH_SECTOR_2;
    else if (addr < 0x08010000)
        return FLASH_SECTOR_3;
    else if (addr < 0x08020000)
        return FLASH_SECTOR_4;
    else if (addr < 0x08040000)
        return FLASH_SECTOR_5;
    else if (addr < 0x08060000)
        return FLASH_SECTOR_6;
    else if (addr < 0x08080000)
        return FLASH_SECTOR_7;
    else if (addr < 0x080A0000)
        return FLASH_SECTOR_8;
    else if (addr < 0x080C0000)
        return FLASH_SECTOR_9;
    else if (addr < 0x080E0000)
        return FLASH_SECTOR_10;
    else
        return FLASH_SECTOR_11;
}

/* ============================= */
/* 获取sector起始地址 */
/* ============================= */

uint32_t Flash_GetSectorStart(uint32_t sector)
{
    switch (sector)
    {
    case FLASH_SECTOR_0:
        return 0x08000000;
    case FLASH_SECTOR_1:
        return 0x08004000;
    case FLASH_SECTOR_2:
        return 0x08008000;
    case FLASH_SECTOR_3:
        return 0x0800C000;
    case FLASH_SECTOR_4:
        return 0x08010000;
    case FLASH_SECTOR_5:
        return 0x08020000;
    case FLASH_SECTOR_6:
        return 0x08040000;
    case FLASH_SECTOR_7:
        return 0x08060000;
    case FLASH_SECTOR_8:
        return 0x08080000;
    case FLASH_SECTOR_9:
        return 0x080A0000;
    case FLASH_SECTOR_10:
        return 0x080C0000;
    case FLASH_SECTOR_11:
        return 0x080E0000;
    default:
        return 0;
    }
}

/* ============================= */
/* 获取sector大小 */
/* ============================= */

uint32_t Flash_GetSectorSize(uint32_t sector)
{
    switch (sector)
    {
    case FLASH_SECTOR_0:
    case FLASH_SECTOR_1:
    case FLASH_SECTOR_2:
    case FLASH_SECTOR_3:
        return 16 * 1024;

    case FLASH_SECTOR_4:
        return 64 * 1024;

    default:
        return 128 * 1024;
    }
}

/* ============================= */
/* 读取Word */
/* ============================= */

uint32_t STMFLASH_ReadWord(uint32_t addr)
{
    return *(volatile uint32_t *)addr;
}

/* ============================= */
/* 读取字节 */
/* ============================= */

uint8_t STMFLASH_Read(uint32_t addr, uint8_t *buf, uint32_t len)
{
    if (buf == NULL || len == 0)
        return 0;

    if (addr < STM32_FLASH_BASE || addr >= STM32_FLASH_END)
        return 0;

    if ((addr + len) > STM32_FLASH_END)
        return 0;

    uint32_t remaining = len;

    while (remaining)
    {
        uint32_t word = STMFLASH_ReadWord(addr & (~3));

        uint8_t *p = (uint8_t *)&word;

        uint32_t offset = addr % 4;

        uint32_t copy = 4 - offset;

        if (copy > remaining)
            copy = remaining;

        memcpy(buf, p + offset, copy);

        buf += copy;
        addr += copy;
        remaining -= copy;
    }

    return 1;
}

/* ============================= */
/* 写入字节 */
/* 进入新sector自动擦除 */
/* ============================= */

uint8_t STMFLASH_Write(uint32_t addr, uint8_t *buf, uint32_t len)
{
    if (buf == NULL || len == 0)
        return 0;

    if (addr < STM32_FLASH_USER_START || addr >= STM32_FLASH_END)
        return 0;

    if ((addr + len) > STM32_FLASH_END)
        return 0;

    uint32_t current_addr = addr;
    uint32_t remaining = len;
    uint32_t last_sector = 0xFFFFFFFF;

    HAL_FLASH_Unlock();

    __HAL_FLASH_DATA_CACHE_DISABLE();

    while (remaining)
    {
        uint32_t sector = Flash_GetSector(current_addr);

        if (sector != last_sector)
        {
            FLASH_EraseInitTypeDef erase;
            uint32_t sectorError;

            erase.TypeErase = FLASH_TYPEERASE_SECTORS;
            erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
            erase.Sector = sector;
            erase.NbSectors = 1;

            if (HAL_FLASHEx_Erase(&erase, &sectorError) != HAL_OK)
                goto error;

            last_sector = sector;
        }

        uint32_t word = 0xFFFFFFFF;
        uint8_t *pword = (uint8_t *)&word;

        uint32_t align = current_addr % 4;

        for (uint32_t i = 0; i < 4; i++)
        {
            if (i < align)
            {
                pword[i] = *(volatile uint8_t *)(current_addr - align + i);
            }
            else if ((i - align) < remaining)
            {
                pword[i] = buf[i - align];
            }
            else
            {
                pword[i] = 0xFF;
            }
        }

        uint32_t write_addr = current_addr - align;

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, write_addr, word) != HAL_OK)
            goto error;

        uint32_t write_bytes = 4 - align;

        if (write_bytes > remaining)
            write_bytes = remaining;

        current_addr += write_bytes;
        buf += write_bytes;
        remaining -= write_bytes;
    }

    __HAL_FLASH_DATA_CACHE_ENABLE();
    HAL_FLASH_Lock();

    return 1;

error:

    __HAL_FLASH_DATA_CACHE_ENABLE();
    HAL_FLASH_Lock();

    return 0;
}