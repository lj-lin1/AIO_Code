#include "flash.h"
#include "stm32f4xx_hal.h"

u32 IPreadbuff[128] = {0};

/* 读取指定地址的32位数据 */
uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
    return *(volatile uint32_t *)faddr;
}

/* 获取地址所在的扇区 */
uint32_t STMFLASH_GetFlashSector(uint32_t addr)
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
    return FLASH_SECTOR_11;
}

/*
 * 从指定地址开始写入指定长度的数据
 * WriteAddr 必须 4 字节对齐
 */
void STMFLASH_Write(uint32_t WriteAddr, uint32_t *pBuffer, uint32_t NumToWrite)
{
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError;
    uint32_t addrx = WriteAddr;
    uint32_t endaddr = WriteAddr + NumToWrite * 4;

    if (WriteAddr < 0x08030000 || (WriteAddr % 4))
        return;

    HAL_FLASH_Unlock();

    /* 擦除期间必须关闭数据缓存 */
    __HAL_FLASH_DATA_CACHE_DISABLE();

    if (addrx < 0x1FFF0000) // 主 Flash 区
    {
        while (addrx < endaddr)
        {
            if (STMFLASH_ReadWord(addrx) != 0xFFFFFFFF)
            {
                EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
                EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
                EraseInitStruct.Sector = STMFLASH_GetFlashSector(addrx);
                EraseInitStruct.NbSectors = 1;

                if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
                {
                    break;
                }
            }
            else
            {
                addrx += 4;
            }
        }
    }

    while (WriteAddr < endaddr)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, WriteAddr, *pBuffer) != HAL_OK)
        {
            break;
        }
        WriteAddr += 4;
        pBuffer++;
    }

    __HAL_FLASH_DATA_CACHE_ENABLE();
    HAL_FLASH_Lock();
}

/* 从指定地址读取数据 */
void STMFLASH_Read(uint32_t ReadAddr, uint32_t *pBuffer, uint32_t NumToRead)
{
    for (uint32_t i = 0; i < NumToRead; i++)
    {
        pBuffer[i] = STMFLASH_ReadWord(ReadAddr);
        ReadAddr += 4;
    }
}
