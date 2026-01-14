#ifndef FLASH_H
#define FLASH_H

#include "stm32f4xx.h"
#include "main.h"

extern u32 IPreadbuff[];

void STMFLASH_Write(uint32_t WriteAddr, uint32_t *pBuffer, uint32_t NumToWrite);
void STMFLASH_Read(uint32_t ReadAddr, uint32_t *pBuffer, uint32_t NumToRead);

#endif