#include "display.h"
#include "func.h"
#include "hub75.h"
#include "stdio.h"
#include "string.h"
#include "w25qxx.h"
#include <assert.h>

u8 fontColor = YELLOW;
__attribute__((section(".ccmram"))) u8 fixBuf[28800] = {0};
u8 gx_u8CurrentScanRowAddr = 0;
u8 g_u8CurrentScanRowAddr = 0;

u8 brightCnt = 0;
u8 lightLev = 8;

u16 xiansibuff[4800] = {0};
__attribute__((section(".ccmram"))) u8 sourceBuf[28800] = {0};

#define LB PDout(4)
#define LC PDout(5)
#define LA PDout(6)
u32 BKData = 0x0B23 << 16;
void BK_Write_Byte(u8 i)
{
    if ((BKData << i) & 0x80)
        LC = 1;
    else
        LC = 0;
}

void scan_line(u8 LineScanOrder)
{
    u8 k;
    static u8 BKNum = 0;
    if (LineScanOrder == 0)
    {
        LA = 1;
        for (k = 0; k < 2; k++)
            ;
    }
    BK_Write_Byte(BKNum++);
    for (k = 0; k < 2; k++)
        ;
    LB = 1;
    for (k = 0; k < 2; k++)
        ;

    LB = 0;
    LA = 0;
}

void lamp_scan(void)
{
    u16 i, k, addr = 0;
    u8 tmpdr1, tmpdr2, tmpdr3, tmpdr4, tmpdr5, tmpdr6 = 0;

    {
        OE(0);
        for (i = 0; i < 480; i++)
        {
            addr = 480 * g_u8CurrentScanRowAddr + i;
            tmpdr1 = fixBuf[addr];
            tmpdr2 = fixBuf[addr + 4800];
            tmpdr3 = fixBuf[addr + 9600];
            tmpdr4 = fixBuf[addr + 14400];
            tmpdr5 = fixBuf[addr + 19200];
            tmpdr6 = fixBuf[addr + 24000];

            if (tmpdr1 == BLACK)
            {
                LED_R1(0);
                LED_G1(0);
                LED_B1(0);
            }
            else
            {
                switch (tmpdr1)
                {
                case RED:
                    LED_R1(1);
                    LED_G1(0);
                    LED_B1(0);
                    break;
                case GREEN:
                    LED_R1(0);
                    LED_G1(1);
                    LED_B1(0);
                    break;
                case YELLOW:
                    LED_R1(1);
                    LED_G1(1);
                    LED_B1(0);
                    break;
                }
            }

            if (tmpdr2 == BLACK)
            {
                LED_R2(0);
                LED_G2(0);
                LED_B2(0);
            }
            else
            {
                switch (tmpdr2)
                {
                case RED:
                    LED_R2(1);
                    LED_G2(0);
                    LED_B2(0);
                    break;
                case GREEN:
                    LED_R2(0);
                    LED_G2(1);
                    LED_B2(0);
                    break;
                case YELLOW:
                    LED_R2(1);
                    LED_G2(1);
                    LED_B2(0);
                    break;
                }
            }

            if (tmpdr3 == BLACK)
            {
                LED_R3(0);
                LED_G3(0);
                LED_B3(0);
            }
            else
            {
                switch (tmpdr3)
                {
                case RED:
                    LED_R3(1);
                    LED_G3(0);
                    LED_B3(0);
                    break;
                case GREEN:
                    LED_R3(0);
                    LED_G3(1);
                    LED_B3(0);
                    break;
                case YELLOW:
                    LED_R3(1);
                    LED_G3(1);
                    LED_B3(0);
                    break;
                }
            }

            if (tmpdr4 == BLACK)
            {
                LED_R4(0);
                LED_G4(0);
                LED_B4(0);
            }
            else
            {
                switch (tmpdr4)
                {
                case RED:
                    LED_R4(1);
                    LED_G4(0);
                    LED_B4(0);
                    break;
                case GREEN:
                    LED_R4(0);
                    LED_G4(1);
                    LED_B4(0);
                    break;
                case YELLOW:
                    LED_R4(1);
                    LED_G4(1);
                    LED_B4(0);
                    break;
                }
            }

            if (tmpdr5 == BLACK)
            {
                LED_R5(0);
                LED_G5(0);
                LED_B5(0);
            }
            else
            {
                switch (tmpdr5)
                {
                case RED:
                    LED_R5(1);
                    LED_G5(0);
                    LED_B5(0);
                    break;
                case GREEN:
                    LED_R5(0);
                    LED_G5(1);
                    LED_B5(0);
                    break;
                case YELLOW:
                    LED_R5(1);
                    LED_G5(1);
                    LED_B5(0);
                    break;
                }
            }

            if (tmpdr6 == BLACK)
            {
                LED_R6(0);
                LED_G6(0);
                LED_B6(0);
            }
            else
            {
                switch (tmpdr6)
                {
                case RED:
                    LED_R6(1);
                    LED_G6(0);
                    LED_B6(0);
                    break;
                case GREEN:
                    LED_R6(0);
                    LED_G6(1);
                    LED_B6(0);
                    break;
                case YELLOW:
                    LED_R6(1);
                    LED_G6(1);
                    LED_B6(0);
                    break;
                }
            }

            for (k = 0; k < 2; k++)
                ;
            SCK(1);

            for (k = 0; k < 2; k++)
                ;
            SCK(0);
        }
        LAT(1);
        for (k = 0; k < 2; k++)
            ;
        LAT(0);

        OE(1);
        scan_line(g_u8CurrentScanRowAddr);
        g_u8CurrentScanRowAddr++;
        if (g_u8CurrentScanRowAddr >= 10)
        {
            g_u8CurrentScanRowAddr = 0;
        }
    }
}

void displaydataupdate(void)
{
    u32 k, addr, addr2 = 0;
    for (k = 0; k < 4800; k++)
    {
        addr = 0;
        addr2 = (((k % 240) + 1) * 120) - 1 - (k / 240);
        fixBuf[xiansibuff[k]] = sourceBuf[addr2];
        addr += 4800;
        addr2 = ((((addr + k) % 240) + 1) * 120) - 1 - ((addr + k) / 240);
        fixBuf[xiansibuff[k] + addr] = sourceBuf[addr2];
        addr += 4800;
        addr2 = ((((addr + k) % 240) + 1) * 120) - 1 - ((addr + k) / 240);
        fixBuf[xiansibuff[k] + addr] = sourceBuf[addr2];
        addr += 4800;
        addr2 = ((((addr + k) % 240) + 1) * 120) - 1 - ((addr + k) / 240);
        fixBuf[xiansibuff[k] + addr] = sourceBuf[addr2];
        addr += 4800;
        addr2 = ((((addr + k) % 240) + 1) * 120) - 1 - ((addr + k) / 240);
        fixBuf[xiansibuff[k] + addr] = sourceBuf[addr2];
        addr += 4800;
        addr2 = ((((addr + k) % 240) + 1) * 120) - 1 - ((addr + k) / 240);
        fixBuf[xiansibuff[k] + addr] = sourceBuf[addr2];
    }
}

void PowerOnDisplay(void)
{
    u8 LIGHT_CHECK_MSG[9] = "setlight";
    u8 lightBuf[9];
    BSP_W25Qx_ReadDMA(&hw25q64, lightBuf, LIGHTADDR, 9);
    if (memcmp(lightBuf, LIGHT_CHECK_MSG, 8) == 0)
    {
        if ((lightBuf[8] <= 8) && (lightBuf[8] != 0))
            lightLev = lightBuf[8];
        else
            lightLev = 8;
    }
    else
    {
        lightLev = 8;
    }

    app_funcs_makefonttolatt_oneline((u8 *)"欢迎行驶", strlen("欢迎行驶"), 0, 0, 0, GREEN);
    app_funcs_makefonttolatt_oneline((u8 *)"高速公路", strlen("高速公路"), 0, 0, 1, GREEN);
    app_funcs_makefonttolatt_oneline((u8 *)"祝您路途愉快！", strlen("祝您路途愉快！"), 0, 0, 2, GREEN);
}