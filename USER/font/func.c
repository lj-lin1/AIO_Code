#include "func.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "display.h"
#include "w25qxx.h"

extern osThreadId_t Display_TaskHandle;

void app_funcs_fill(uint8_t color)
{
    for (uint32_t k = 0; k < 28800; k++)
    { // 清空原始点阵区
        sourceBuf[k] = color;
    }
    osThreadFlagsSet(Display_TaskHandle, 0x01);
}

void app_funcs_clear(u8 row) // 清屏函数
{
    u32 j, addr;

    if (row == 255) // 清全屏
    {
        for (j = 0; j < 28800; j++) // 清空原始点阵区
        {
            sourceBuf[j] = 0;
        }
    }
    else
    {
        switch (fontSize) //	120*240
        {
        case FONT16:
            if (row < 15) // 240/16=15行，行号0-14
            {
                addr = row * 1920;         // row*16*120，每行16像素高，120列宽
                for (j = 0; j < 1920; j++) // 16*120=1920字节
                {
                    if ((addr + j) < 28800) // 边界检查，防止数组越界
                    {
                        sourceBuf[addr + j] = 0;
                    }
                }
            }
            break;

        case FONT20:
            if (row < 12) // 240/20=12行，行号0-11
            {
                addr = row * 2400;         // row*20*120，每行20像素高，120列宽
                for (j = 0; j < 2400; j++) // 20*120=2400字节
                {
                    if ((addr + j) < 28800) // 边界检查，防止数组越界
                    {
                        sourceBuf[addr + j] = 0;
                    }
                }
            }
            break;

        case FONT24:
            if (row < 10) // 240/24=10行，行号0-9
            {
                addr = row * 2880;         // row*24*120，每行24像素高，120列宽
                for (j = 0; j < 2880; j++) // 24*120=2880字节
                {
                    if ((addr + j) < 28800) // 边界检查，防止数组越界
                    {
                        sourceBuf[addr + j] = 0;
                    }
                }
            }
            break;
        }
    }
    osThreadFlagsSet(Display_TaskHandle, 0x01);
}
/*
 *	W25Q64	64MBilt/8M字节
 *	每页/256字节
 *	每扇区4096字节
 */
u8 font_buf[128] = {0};
u8 fontType = 0;
u8 fontSize = 0;
void app_funcs_makefonttolatt_all(u8 *fontstr, u32 length, u8 x, u8 y, u8 color) // 全屏显示分发函数
{
    u8 tempLen = 0;

    switch (fontSize)
    {

    case FONT16:
        tempLen = length;
        if (tempLen >= (120 / 16 * 2) * (240 / 16))
            tempLen = (120 / 16 * 2) * (240 / 16);
        MakeSixteenLattAll(fontstr, tempLen, x, y, color); // 显示16点阵数据
        break;

    case FONT20:
        tempLen = length;
        if (tempLen >= (120 / 20 * 2) * (240 / 20))
            tempLen = (120 / 20 * 2) * (240 / 20);
        MakeTwentyLattAll(fontstr, tempLen, x, y, color); // 显示20点阵数据
        break;

    case FONT24:
        tempLen = length;
        if (tempLen >= (120 / 24 * 2) * (240 / 24))
            tempLen = (120 / 24 * 2) * (240 / 24);
        MakeTwentyFourLattAll(fontstr, tempLen, x, y, color); // 显示24点阵数据
        break;
    }
}

void app_funcs_makefonttolatt_oneline(u8 *fontstr, u32 length, u8 x, u8 y, u8 row, u8 color) // 单行显示分发函数
{
    u8 tempLen = 0;
    switch (fontSize)
    {
    case FONT16:
        if (row < 240 / 16) // 240/16=15行，行号0-14
        {
            if (tempLen > 120 / 16 * 2)
                tempLen = 120 / 16 * 2;
            else
                tempLen = length;
            MakeSixteenLattOneLine(fontstr, tempLen, x, y, row, color); // 显示16点阵数据
        }
        break;

    case FONT20:
        if (row < 240 / 20) // 240/20=12行，行号0-11
        {
            if (tempLen > 120 / 20 * 2)
                tempLen = 120 / 20 * 2;
            else
                tempLen = length;
            MakeTwentyOneLine(fontstr, tempLen, x, y, row, color); // 显示20点阵数据
        }
        break;

    case FONT24:
        if (row < 240 / 10) // 240/24=10行，行号0-9
        {
            if (tempLen > 120 / 24 * 2)
                tempLen = 120 / 24 * 2;
            else
                tempLen = length;
            MakeTwentyFourLattOneLine(fontstr, tempLen, x, y, row, color); // 显示24点阵数据
        }
        break;
    }
}

void MakeSixteenLattAll(u8 *fontstr, u32 length, u8 x, u8 y, u8 color) // 16点阵全屏显示
{
    u32 i, k;
    u16 rowcnt, colcnt;

    fontColor = color;

    for (k = 0; k < 28800; k++) // 清空原始点阵区
    {
        sourceBuf[k] = 0;
    }

    rowcnt = 0;
    colcnt = 0;

    for (i = 0; i < length;)
    {
        if (rowcnt + 16 > 240) // 超出屏幕范围
            break;

        if ((fontstr[i] == 0x0A) && (fontstr[i + 1] == 0x0D)) // 是换行符号，系统换行
        {
            if (colcnt != 0)
            {
                rowcnt += 16;
                colcnt = 0;
            }
            i += 2;
        }

        else if ((fontstr[i] == 0x5C) && (fontstr[i + 1] == 0x6E)) // 是换行符号，系统换行
        {
            if (colcnt != 0)
            {
                rowcnt += 16;
                colcnt = 0;
            }
            i += 2;
        }

        else if ((fontstr[i] >= 0x20) && (fontstr[i] <= 0x7F)) // 判断字符是否为ASCII码
        {
            u8 *returnBuf = readSixteenAsciiData(fontstr[i], rowcnt, colcnt, 1);
            i++;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA1) && // 第01区 A1A0-A1FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readSixteenHzData(&fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA2) && // 第02区 A2A0-A2FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readSixteenHzData(&fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA3) && // 第03区 A3A0-A3FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readSixteenHzData(&fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA4) && // 第04区 A4A0-A4FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readSixteenHzData(&fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA5) && // 第05区 A5A0-A5FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readSixteenHzData(&fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA6) && // 第06区 A6A0-A6FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readSixteenHzData(fontstr, rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA7) && // 第07区 A7A0-A7FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readSixteenHzData(&fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA8) && // 第08区 A8A0-A8FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readSixteenHzData(&fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA9) && // 第09区 A9A0-A9FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readSixteenHzData(&fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] >= 0xB0) && (fontstr[i] <= 0xF7) && // 判断字符是否为GB2312汉字内码
                 (fontstr[i + 1] >= 0xA1) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readSixteenHzData(&fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
        }

        else
        {
            i++;
        }
    }
    osThreadFlagsSet(Display_TaskHandle, 0x01);
}

u8 fontSta22;
void MakeTwentyLattAll(u8 *fontstr, u32 length, u8 x, u8 y, u8 color) // 显示20点阵数据
{

    u32 i, k;
    u16 rowcnt, colcnt;
    u8 tempASCIICnt = 0;
    u8 tempShift = 0;

    fontColor = color;

    for (k = 0; k < 28800; k++) // 清空原始点阵区
    {
        sourceBuf[k] = 0;
    }

    rowcnt = 0;
    colcnt = 0;

    for (i = 0; i < length;)
    {
        if (rowcnt + 20 > 240) // 超出屏幕范围
            break;

        if ((fontstr[i] == 0x0A) && (fontstr[i + 1] == 0x0D)) // 是换行符号，系统换行
        {
            if (colcnt != 0)
            {
                rowcnt += 24;
                colcnt = 0;
                tempASCIICnt = 0;
                tempShift = 0;
            }
            i += 2;
        }

        else if ((fontstr[i] == 0x5C) && (fontstr[i + 1] == 0x6E)) // 是换行符号，系统换行
        {
            if (colcnt != 0)
            {
                rowcnt += 24;
                colcnt = 0;
                tempASCIICnt = 0;
                tempShift = 0;
            }
            i += 2;
        }

        else if ((fontstr[i] >= 0x20) && (fontstr[i] <= 0x7F)) // 判断字符是否为ASCII码
        {
            u8 *returnBuf = readTwentyAsciiData(fontstr[i], rowcnt, colcnt, 1);
            i++;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
            fontSta22 = fontASCII;
        }

        else if ((fontstr[i] == 0xA1) && // 第01区 A1A0-A1FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyAsciiData(fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA2) && // 第02区 A2A0-A2FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyAsciiData(fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA3) && // 第03区 A3A0-A3FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA4) && // 第04区 A4A0-A4FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA5) && // 第05区 A5A0-A5FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA6) && // 第06区 A6A0-A6FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA7) && // 第07区 A7A0-A7FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA8) && // 第08区 A8A0-A8FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA9) && // 第09区 A9A0-A9FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] >= 0xB0) && (fontstr[i] <= 0xF7) && // 判断字符是否为GB2312汉字内码
                 (fontstr[i + 1] >= 0xA1) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else
        {
            i++;
        }
    }
    fontSta22 = fontNoting;
    osThreadFlagsSet(Display_TaskHandle, 0x01);
}

void MakeTwentytwoLattAll(u8 *fontstr, u32 length, u8 x, u8 y, u8 color) // 22点阵全屏显示
{
    u32 i, k;
    u16 rowcnt, colcnt;
    u8 tempASCIICnt = 0;
    u8 tempShift = 0;

    for (k = 0; k < 28800; k++) // 清空原始点阵区
    {
        sourceBuf[k] = 0;
    }

    rowcnt = 0;
    colcnt = 0;

    for (i = 0; i < length;)
    {
        if ((fontstr[i] == 0x0A) && (fontstr[i + 1] == 0x0D)) // 是换行符号，系统换行
        {
            if (colcnt != 0)
            {
                rowcnt += 24;
                colcnt = 0;
                tempASCIICnt = 0;
                tempShift = 0;
            }
            i += 2;
        }

        else if ((fontstr[i] == 0x5C) && (fontstr[i + 1] == 0x6E)) // 是换行符号，系统换行
        {
            if (colcnt != 0)
            {
                rowcnt += 24;
                colcnt = 0;
                tempASCIICnt = 0;
                tempShift = 0;
            }
            i += 2;
        }

        else if ((fontstr[i] >= 0x20) && (fontstr[i] <= 0x7F)) // 判断字符是否为ASCII码
        {
            u8 *returnBuf = readTwentyTwoAsciiData(fontstr[i], rowcnt, colcnt, 1);
            i++;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
            fontSta22 = fontASCII;
        }

        else if ((fontstr[i] == 0xA1) && // 第01区 A1A0-A1FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyTwoAsciiData(fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA2) && // 第02区 A2A0-A2FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyTwoAsciiData(fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA3) && // 第03区 A3A0-A3FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyTwoHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA4) && // 第04区 A4A0-A4FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyTwoHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA5) && // 第05区 A5A0-A5FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyTwoHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA6) && // 第06区 A6A0-A6FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyTwoHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA7) && // 第07区 A7A0-A7FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyTwoHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA8) && // 第08区 A8A0-A8FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyTwoHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA9) && // 第09区 A9A0-A9FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyTwoHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] >= 0xB0) && (fontstr[i] <= 0xF7) && // 判断字符是否为GB2312汉字内码
                 (fontstr[i + 1] >= 0xA1) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyTwoHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else
        {
            i++;
        }
    }
    fontSta22 = fontNoting;
    osThreadFlagsSet(Display_TaskHandle, 0x01);
}

u8 fontSta;
void MakeTwentyFourLattAll(u8 *fontstr, u32 length, u8 x, u8 y, u8 color) // 24点阵全屏显示
{
    u32 i, k;
    u16 rowcnt, colcnt;
    u8 tempASCIICnt = 0;
    u8 tempShift = 0;

    fontColor = color;

    for (k = 0; k < 28800; k++) // 清空原始点阵区
    {
        sourceBuf[k] = 0;
    }

    rowcnt = 0;
    colcnt = 0;

    for (i = 0; i < length;)
    {
        if (rowcnt + 24 > 240)
            break;

        if ((fontstr[i] == 0x0A) && (fontstr[i + 1] == 0x0D)) // 是换行符号，系统换行
        {
            if (colcnt != 0)
            {
                rowcnt += 24;
                colcnt = 0;
                tempASCIICnt = 0;
                tempShift = 0;
            }
            i += 2;
        }

        else if ((fontstr[i] == 0x5C) && (fontstr[i + 1] == 0x6E)) // 是换行符号，系统换行
        {
            if (colcnt != 0)
            {
                rowcnt += 24;
                colcnt = 0;
                tempASCIICnt = 0;
                tempShift = 0;
            }
            i += 2;
        }

        else if ((fontstr[i] >= 0x20) && (fontstr[i] <= 0x7F)) // 判断字符是否为ASCII码
        {
            u8 *returnBuf = readTwentyFourAsciiData(fontstr[i], rowcnt, colcnt, tempASCIICnt, 1);
            i++;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
            fontSta = fontASCII;
        }

        else if ((fontstr[i] == 0xA1) && // 第01区 A1A0-A1FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyFourHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA2) && // 第02区 A2A0-A2FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyFourHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA3) && // 第03区 A3A0-A3FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyFourHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA4) && // 第04区 A4A0-A4FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyFourHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA5) && // 第05区 A5A0-A5FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyFourHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA6) && // 第06区 A6A0-A6FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyFourHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA7) && // 第07区 A7A0-A7FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyFourHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA8) && // 第08区 A8A0-A8FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyFourHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA9) && // 第09区 A9A0-A9FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyFourHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] >= 0xB0) && (fontstr[i] <= 0xF7) && // 判断字符是否为GB2312汉字内码
                 (fontstr[i + 1] >= 0xA1) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyFourHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else
        {
            i++;
        }
    }
    fontSta = fontNoting;
    osThreadFlagsSet(Display_TaskHandle, 0x01);
}
void MakeThirtyTwoLattAll(u8 *fontstr, u32 length, u8 x, u8 y, u8 color) // 32点阵全屏显示
{
    u32 i, k;
    u16 rowcnt, colcnt;

    for (k = 0; k < 28800; k++) // 清空原始点阵区
    {
        sourceBuf[k] = 0;
    }

    rowcnt = 0;
    colcnt = 0;

    for (i = 0; i < length;)
    {
        if ((fontstr[i] == 0x0A) && (fontstr[i + 1] == 0x0D)) // 是换行符号，系统换行
        {
            if (colcnt != 0)
            {
                rowcnt += 32;
                colcnt = 0;
            }
            i += 2;
        }

        else if ((fontstr[i] == 0x5C) && (fontstr[i + 1] == 0x6E)) // 是换行符号，系统换行
        {
            if (colcnt != 0)
            {
                rowcnt += 32;
                colcnt = 0;
            }
            i += 2;
        }

        else if ((fontstr[i] >= 0x20) && (fontstr[i] <= 0x7F)) // 判断字符是否为ASCII码
        {
            u8 *returnBuf = readThirtyTwoAsciiData(fontstr[i], rowcnt, colcnt, 1);
            i++;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA1) && // 第01区 A1A0-A1FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readThirtyTwoHzData(&fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA2) && // 第02区 A2A0-A2FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readThirtyTwoHzData(&fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA3) && // 第03区 A3A0-A3FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readThirtyTwoHzData(&fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA4) && // 第04区 A4A0-A4FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readThirtyTwoHzData(&fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA5) && // 第05区 A5A0-A5FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readThirtyTwoHzData(&fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA6) && // 第06区 A6A0-A6FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readThirtyTwoHzData(&fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA7) && // 第07区 A7A0-A7FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readThirtyTwoHzData(&fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA8) && // 第08区 A8A0-A8FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readThirtyTwoHzData(&fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA9) && // 第09区 A9A0-A9FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readThirtyTwoHzData(&fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] >= 0xB0) && (fontstr[i] <= 0xF7) && // 判断字符是否为GB2312汉字内码
                 (fontstr[i + 1] >= 0xA1) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readThirtyTwoHzData(&fontstr[i], rowcnt, colcnt, 1);
            i += 2;
            rowcnt = returnBuf[0];
            colcnt = returnBuf[1];
        }

        else
        {
            i++;
        }
    }
    osThreadFlagsSet(Display_TaskHandle, 0x01);
}

void MakeSixteenLattOneLine(u8 *fontstr, u32 length, u8 x, u8 y, u8 row, u8 color) // 16点阵单行显示
{
    u32 i = 0;
    u16 rowcnt, colcnt;

    app_funcs_clear(row);

    fontColor = color;

    rowcnt = row * 16;
    colcnt = x; // 使用x参数设置起始列位置

    for (i = 0; i < length;)
    {
        if ((fontstr[i] >= 0x20) && (fontstr[i] <= 0x7F)) // 判断字符是否为ASCII码
        {
            // 检查是否超出屏幕边界（一行120像素，每个字符8像素）
            if (colcnt + 8 > 119)
                break; // 超出屏幕，停止显示
            u8 *returnBuf = readSixteenAsciiData(fontstr[i], rowcnt, colcnt, 0);
            i++;
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA1) && // 第01区 A1A0-A1FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：16点阵中文字符宽度16像素
            if (colcnt + 16 > 120)
                break;
            u8 *returnBuf = readSixteenHzData(&fontstr[i], rowcnt, colcnt, 0);
            i += 2;
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA2) && // 第02区 A2A0-A2FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：16点阵中文字符宽度16像素
            if (colcnt + 16 > 120)
                break;
            u8 *returnBuf = readSixteenHzData(&fontstr[i], rowcnt, colcnt, 0);
            i += 2;
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA3) && // 第03区 A3A0-A3FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：16点阵中文字符宽度16像素
            if (colcnt + 16 > 120)
                break;
            u8 *returnBuf = readSixteenHzData(&fontstr[i], rowcnt, colcnt, 0);
            i += 2;
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA4) && // 第04区 A4A0-A4FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：16点阵中文字符宽度16像素
            if (colcnt + 16 > 120)
                break;
            u8 *returnBuf = readSixteenHzData(&fontstr[i], rowcnt, colcnt, 0);
            i += 2;
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA5) && // 第05区 A5A0-A5FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：16点阵中文字符宽度16像素
            if (colcnt + 16 > 120)
                break;
            u8 *returnBuf = readSixteenHzData(&fontstr[i], rowcnt, colcnt, 0);
            i += 2;
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA6) && // 第06区 A6A0-A6FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：16点阵中文字符宽度16像素
            if (colcnt + 16 > 120)
                break;
            u8 *returnBuf = readSixteenHzData(&fontstr[i], rowcnt, colcnt, 0);
            i += 2;
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA7) && // 第07区 A7A0-A7FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：16点阵中文字符宽度16像素
            if (colcnt + 16 > 120)
                break;
            u8 *returnBuf = readSixteenHzData(&fontstr[i], rowcnt, colcnt, 0);
            i += 2;
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA8) && // 第08区 A8A0-A8FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：16点阵中文字符宽度16像素
            if (colcnt + 16 > 120)
                break;
            u8 *returnBuf = readSixteenHzData(&fontstr[i], rowcnt, colcnt, 0);
            i += 2;
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA9) && // 第09区 A9A0-A9FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：16点阵中文字符宽度16像素
            if (colcnt + 16 > 120)
                break;
            u8 *returnBuf = readSixteenHzData(&fontstr[i], rowcnt, colcnt, 0);
            i += 2;
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] >= 0xB0) && (fontstr[i] <= 0xF7) && // 判断字符是否为GB2312汉字内码
                 (fontstr[i + 1] >= 0xA1) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：16点阵中文字符宽度16像素
            if (colcnt + 16 > 120)
                break;
            u8 *returnBuf = readSixteenHzData(&fontstr[i], rowcnt, colcnt, 0);
            i += 2;
            colcnt = returnBuf[1];
        }

        else
        {
            i++;
        }
    }
    osThreadFlagsSet(Display_TaskHandle, 0x01);
}

void MakeTwentyOneLine(u8 *fontstr, u32 length, u8 x, u8 y, u8 row, u8 color)
{

    u32 i = 0;
    u8 rowcnt = 0, colcnt = 0;
    u8 tempASCIICnt = 0;
    u8 tempShift = 0;

    app_funcs_clear(row);

    fontColor = color;

    rowcnt = row * 20;
    colcnt = x; // 设置起始列位置

    for (i = 0; i < length;)
    {

        if ((fontstr[i] >= 0x20) && (fontstr[i] <= 0x7F)) // 判断字符是否为ASCII码
        {
            // 边界检查：20点阵字符宽度20像素，字间距10像素，允许字符部分超出屏幕
            if (colcnt + 10 > 119)
                break;
            u8 *returnBuf = readTwentyAsciiData(fontstr[i], rowcnt, colcnt, 0);
            i++;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
            fontSta = fontASCII;
        }

        else if ((fontstr[i] == 0xA1) && // 第01区 A1A0-A1FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：20点阵汉字宽度20像素，字间距10像素
            if (colcnt + 20 > 120)
                break;
            u8 *returnBuf = readTwentyHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA2) && // 第02区 A2A0-A2FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：20点阵汉字宽度20像素，字间距10像素
            if (colcnt + 20 > 120)
                break;
            u8 *returnBuf = readTwentyHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA3) && // 第03区 A3A0-A3FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：20点阵汉字宽度20像素，字间距10像素
            if (colcnt + 20 > 120)
                break;
            u8 *returnBuf = readTwentyHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA4) && // 第04区 A4A0-A4FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：20点阵汉字宽度20像素，字间距10像素
            if (colcnt + 20 > 120)
                break;
            u8 *returnBuf = readTwentyHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA5) && // 第05区 A5A0-A5FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：20点阵汉字宽度20像素，字间距10像素
            if (colcnt + 20 > 120)
                break;
            u8 *returnBuf = readTwentyHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA6) && // 第06区 A6A0-A6FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：20点阵汉字宽度20像素，字间距10像素
            if (colcnt + 20 > 120)
                break;
            u8 *returnBuf = readTwentyHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA7) && // 第07区 A7A0-A7FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：20点阵汉字宽度20像素，字间距10像素
            if (colcnt + 20 > 120)
                break;
            u8 *returnBuf = readTwentyHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA8) && // 第08区 A8A0-A8FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：20点阵汉字宽度20像素，字间距10像素
            if (colcnt + 20 > 120)
                break;
            u8 *returnBuf = readTwentyHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA9) && // 第09区 A9A0-A9FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：20点阵汉字宽度20像素，字间距10像素
            if (colcnt + 20 > 120)
                break;
            u8 *returnBuf = readTwentyHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] >= 0xB0) && (fontstr[i] <= 0xF7) && // 判断字符是否为GB2312汉字内码
                 (fontstr[i + 1] >= 0xA1) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：20点阵汉字宽度20像素，字间距10像素
            if (colcnt + 20 > 120)
                break;
            u8 *returnBuf = readTwentyHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else
        {
            i++;
        }
    }
    fontSta = fontNoting;
    osThreadFlagsSet(Display_TaskHandle, 0x01);
}

void MakeTwentyTwoLattOneLine(u8 *fontstr, u32 length, u8 x, u8 y, u8 row, u8 color)
{

    u32 i = 0;
    u8 rowcnt = 0, colcnt = 0;
    u8 tempASCIICnt = 0;
    u8 tempShift = 0;

    app_funcs_clear(row);

    rowcnt = row * 24;
    colcnt = 0;

    for (i = 0; i < length;)
    {

        if ((fontstr[i] >= 0x20) && (fontstr[i] <= 0x7F)) // 判断字符是否为ASCII码
        {
            u8 *returnBuf = readTwentyTwoAsciiData(fontstr[i], rowcnt, colcnt, tempASCIICnt);
            i++;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
            fontSta = fontASCII;
        }

        else if ((fontstr[i] == 0xA1) && // 第01区 A1A0-A1FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyTwoHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA2) && // 第02区 A2A0-A2FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyTwoHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA3) && // 第03区 A3A0-A3FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyTwoHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA4) && // 第04区 A4A0-A4FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyTwoHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA5) && // 第05区 A5A0-A5FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyTwoHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA6) && // 第06区 A6A0-A6FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyTwoHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA7) && // 第07区 A7A0-A7FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyTwoHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA8) && // 第08区 A8A0-A8FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyTwoHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA9) && // 第09区 A9A0-A9FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyTwoHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] >= 0xB0) && (fontstr[i] <= 0xF7) && // 判断字符是否为GB2312汉字内码
                 (fontstr[i + 1] >= 0xA1) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readTwentyTwoHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else
        {
            i++;
        }
    }
    fontSta = fontNoting;
    osThreadFlagsSet(Display_TaskHandle, 0x01);
}

void MakeTwentyFourLattOneLine(u8 *fontstr, u32 length, u8 x, u8 y, u8 row, u8 color) // 24点阵单行显示
{
    u32 i = 0;
    u8 rowcnt = 0, colcnt = 0;
    u8 tempASCIICnt = 0;
    u8 tempShift = 0;

    app_funcs_clear(row);

    fontColor = color;

    rowcnt = row * 24;
    colcnt = x; // 设置起始列位置

    for (i = 0; i < length;)
    {
        if ((fontstr[i] >= 0x20) && (fontstr[i] <= 0x7F)) // 判断字符是否为ASCII码
        {
            // 边界检查：24点阵字符宽度24像素，字间距12像素，允许字符部分超出屏幕
            if (colcnt + 12 > 119)
                break;
            u8 *returnBuf = readTwentyFourAsciiData(fontstr[i], rowcnt, colcnt, tempASCIICnt, 0);
            i++;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
            fontSta = fontASCII;
        }

        else if ((fontstr[i] == 0xA1) && // 第01区 A1A0-A1FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：24点阵汉字宽度24像素，字间距12像素
            if (colcnt + 24 > 120)
                break;
            u8 *returnBuf = readTwentyFourHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA2) && // 第02区 A2A0-A2FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：24点阵汉字宽度24像素，字间距12像素
            if (colcnt + 24 > 120)
                break;
            u8 *returnBuf = readTwentyFourHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA3) && // 第03区 A3A0-A3FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：24点阵汉字宽度24像素，字间距12像素
            if (colcnt + 24 > 120)
                break;
            u8 *returnBuf = readTwentyFourHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA4) && // 第04区 A4A0-A4FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：24点阵汉字宽度24像素，字间距12像素
            if (colcnt + 24 > 120)
                break;
            u8 *returnBuf = readTwentyFourHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA5) && // 第05区 A5A0-A5FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：24点阵汉字宽度24像素，字间距12像素
            if (colcnt + 24 > 120)
                break;
            u8 *returnBuf = readTwentyFourHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA6) && // 第06区 A6A0-A6FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：24点阵汉字宽度24像素，字间距12像素
            if (colcnt + 24 > 120)
                break;
            u8 *returnBuf = readTwentyFourHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA7) && // 第07区 A7A0-A7FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：24点阵汉字宽度24像素，字间距12像素
            if (colcnt + 24 > 120)
                break;
            u8 *returnBuf = readTwentyFourHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA8) && // 第08区 A8A0-A8FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：24点阵汉字宽度24像素，字间距12像素
            if (colcnt + 24 > 120)
                break;
            u8 *returnBuf = readTwentyFourHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] == 0xA9) && // 第09区 A9A0-A9FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：24点阵汉字宽度24像素，字间距12像素
            if (colcnt + 24 > 120)
                break;
            u8 *returnBuf = readTwentyFourHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else if ((fontstr[i] >= 0xB0) && (fontstr[i] <= 0xF7) && // 判断字符是否为GB2312汉字内码
                 (fontstr[i + 1] >= 0xA1) && (fontstr[i + 1] <= 0xFE))
        {
            // 边界检查：24点阵汉字宽度24像素，字间距12像素
            if (colcnt + 24 > 120)
                break;
            u8 *returnBuf = readTwentyFourHzData(&fontstr[i], rowcnt, colcnt, tempASCIICnt, tempShift, 0);
            i += 2;
            colcnt = returnBuf[1];
            tempASCIICnt = returnBuf[2];
            tempShift = returnBuf[3];
        }

        else
        {
            i++;
        }
    }
    fontSta = fontNoting;
    osThreadFlagsSet(Display_TaskHandle, 0x01);
}

void MakeThirtyTwoLattOneLine(u8 *fontstr, u32 length, u8 x, u8 y, u8 row, u8 color) // 32点阵单行显示
{
    u32 i = 0;
    u16 rowcnt = 0, colcnt = 0;

    app_funcs_clear(row);

    rowcnt = row * 32;
    colcnt = 0;

    for (i = 0; i < length;)
    {
        if ((fontstr[i] >= 0x20) && (fontstr[i] <= 0x7F)) // 判断字符是否为ASCII码
        {
            u8 *returnBuf = readThirtyTwoAsciiData(fontstr[i], rowcnt, colcnt, 0);
            i++;
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA1) && // 第01区 A1A0-A1FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readThirtyTwoHzData(&fontstr[i], rowcnt, colcnt, 0);
            i += 2;
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA2) && // 第02区 A2A0-A2FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readThirtyTwoHzData(&fontstr[i], rowcnt, colcnt, 0);
            i += 2;
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA3) && // 第03区 A3A0-A3FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readThirtyTwoHzData(&fontstr[i], rowcnt, colcnt, 0);
            i += 2;
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA4) && // 第04区 A4A0-A4FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readThirtyTwoHzData(&fontstr[i], rowcnt, colcnt, 0);
            i += 2;
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA5) && // 第05区 A5A0-A5FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readThirtyTwoHzData(&fontstr[i], rowcnt, colcnt, 0);
            i += 2;
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA6) && // 第06区 A6A0-A6FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readThirtyTwoHzData(&fontstr[i], rowcnt, colcnt, 0);
            i += 2;
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA7) && // 第07区 A7A0-A7FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readThirtyTwoHzData(&fontstr[i], rowcnt, colcnt, 0);
            i += 2;
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA8) && // 第08区 A8A0-A8FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readThirtyTwoHzData(&fontstr[i], rowcnt, colcnt, 0);
            i += 2;
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] == 0xA9) && // 第09区 A9A0-A9FE
                 (fontstr[i + 1] >= 0xA0) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readThirtyTwoHzData(&fontstr[i], rowcnt, colcnt, 0);
            i += 2;
            colcnt = returnBuf[1];
        }

        else if ((fontstr[i] >= 0xB0) && (fontstr[i] <= 0xF7) && // 判断字符是否为GB2312汉字内码
                 (fontstr[i + 1] >= 0xA1) && (fontstr[i + 1] <= 0xFE))
        {
            u8 *returnBuf = readThirtyTwoHzData(&fontstr[i], rowcnt, colcnt, 0);
            i += 2;
            colcnt = returnBuf[1];
        }

        else
        {
            i++;
        }
    }
    osThreadFlagsSet(Display_TaskHandle, 0x01);
}

// 读取16点阵ASCII码写入原始缓存区,有单行或者全屏显示的区别	//读取16点阵ASCII码写入原始缓存区,有单行或者全屏显示的区别	//读取16点阵ASCII码写入原始缓存区,有单行或者全屏显示的区别	//读取16点阵ASCII码写入原始缓存区,有单行或者全屏显示的区别

u16 DZaddr;
#define DZAmax 28800
u8 *readSixteenAsciiData(u8 fontstr, u8 rowCnt, u8 colCnt, u8 lingOrAll) // 读取16点阵ASCII码写入原始缓存区,有单行或者全屏显示的区别
{
    u32 readaddr = 0;
    u32 FonfAddr = 0;
    u8 j = 0;
    u8 rowcnt = 0, colcnt = 0;
    static u8 returnBuf[2] = {0};

    rowcnt = rowCnt;
    colcnt = colCnt;

    FonfAddr = ((uint32_t)fontstr - 32) * 32;

    switch (fontType)
    {
    case FONTST:
        readaddr = 0x0000B880 + FonfAddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 32);
        break;

    case FONTFS:
        readaddr = 0x00000000 + FonfAddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 32);
        break;

    case FONTKT:
        readaddr = 0x00007B00 + FonfAddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 32);
        break;

    case FONTHT:
        readaddr = 0x00003D80 + FonfAddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 32);
        break;
    }

    // 16点阵字体：每行2字节(16位)，但实际只显示前8像素（字符宽度8像素）
    // 地址计算：rowcnt*120 + colcnt + rowInChar*120 + byteInRow*8
    // 只写入第一个字节（byteInRow==0），第二个字节（byteInRow==1）不写入
    for (j = 0; j < 32; j++)
    {
        u8 byteInRow = j % 2; // 当前字节在行中的位置(0,1)
        u8 rowInChar = j / 2; // 当前行在字符中的位置(0-15)

        // 只写入第一个字节（前8像素），第二个字节不写入
        if (byteInRow == 0)
        {
            DZaddr = rowcnt * 120 + colcnt + rowInChar * 120 + byteInRow * 8;

            if (DZaddr < DZAmax)
            {
                for (u8 i = 0; i < 8; i++)
                {
                    if ((DZaddr + i) < DZAmax) // 边界检查，防止数组越界
                    {
                        if (((font_buf[j] << i) & 0x80) == 0)
                            sourceBuf[DZaddr + i] = BLACK;
                        else
                            sourceBuf[DZaddr + i] = fontColor;
                    }
                }
            }
        }
    }

    colcnt += 8; // 字间距，16点阵字体实际显示宽度8像素（每行2字节，但字符宽度8像素）

    if ((colcnt > 112) && (lingOrAll == 1)) // 全屏显示的时候需要换行，单行显示的时候不用
    {
        // 120-8=112，如果colcnt>112，下一个字符会超出屏幕，需要换行
        rowcnt += 16; // 行间距，16点阵字体高度16像素
        colcnt = 0;
    }

    returnBuf[0] = rowcnt;
    returnBuf[1] = colcnt;

    return returnBuf;
}

u8 *readSixteenHzData(u8 *fontstr, u8 rowCnt, u8 colCnt, u8 lingOrAll) // 读取16点阵Hz码写入原始缓存区
{
    u32 readaddr = 0;
    u32 FonfAddr = 0;
    u8 j = 0;
    u8 rowcnt = 0, colcnt = 0;
    static u8 returnBuf[2] = {0};

    rowcnt = rowCnt;
    colcnt = colCnt;

    FonfAddr = (94 * (fontstr[0] - 0xA1) + (fontstr[1] - 0xA1)) * 32;
    switch (fontType)
    {
    case FONTST:
        readaddr = 0x004CE5D0 + FonfAddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 32);
        break;

    case FONTFS:
        readaddr = 0x000A9020 + FonfAddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 32);
        break;

    case FONTKT:
        readaddr = 0x0036C940 + FonfAddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 32);
        break;

    case FONTHT:
        readaddr = 0x0020ACB0 + FonfAddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 32);
        break;
    }

    if (colcnt > 106) // 全屏显示的时候需要换行，单行显示的时候不用
    {
        if (lingOrAll == 1)
        {
            if ((fontstr[0] >= 0xB0) && (fontstr[0] <= 0xF7) && // 判断字符是否为GB2312汉字内码
                (fontstr[1] >= 0xA1) && (fontstr[1] <= 0xFE))
            {
                rowcnt += 14;
            }
            colcnt = 0;
        }
        else
        {
            returnBuf[0] = rowcnt;
            returnBuf[1] = colcnt;
            return returnBuf;
        }
    }

    for (j = 0; j < 32; j++)
    {
        DZaddr = rowcnt * 120 + colcnt + j / 2 * 120 + (j % 2) * 8;
        if (DZaddr < DZAmax)
        {
            for (u8 i = 0; i < 8; i++)
            {
                //				if((j%2)&&(i>6));
                //				else
                if (((font_buf[j] << i) & 0x80) == 0)
                    sourceBuf[DZaddr + i] = BLACK;
                else
                    sourceBuf[DZaddr + i] = fontColor;
            }
        }
    }

    colcnt += 16;

    if ((colcnt > 106) && (lingOrAll == 1)) // 全屏显示的时候需要换行，单行显示的时候不用
    {
        rowcnt += 16;
        colcnt = 0;
    }

    returnBuf[0] = rowcnt;
    returnBuf[1] = colcnt;

    return returnBuf;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 读取20点阵ASCII码写入原始缓存区,有单行或者全屏显示的区别

u8 *readTwentyAsciiData(u8 fontstr, u8 rowCnt, u8 colCnt, u8 lingOrAll) // 读取20点阵ASCII码写入原始缓存区,有单行或者全屏显示的区别
{

    u32 readaddr = 0;
    u32 FonfAddr = 0;
    u8 j = 0;
    u8 rowcnt = 0, colcnt = 0;
    static u8 returnBuf[4] = {0}; // 修改为4个元素，以匹配调用者的使用

    rowcnt = rowCnt;
    colcnt = colCnt;

    FonfAddr = ((uint32_t)fontstr - 32) * 60; // 统一计算方式：先计算偏移量

    switch (fontType)
    {
    case FONTST:
        readaddr = 0x0000C480 + FonfAddr;                    //(ST_ASCII_20_ADDRESS+sectoraddr) * 4096 + pageaddr * 256 + byteaddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 60); // 20点阵字体：20行×3字节=60字节
        break;

    case FONTFS:
        readaddr = 0x00000C00 + FonfAddr;                    //(FS_ASCII_20_ADDRESS+sectoraddr) * 4096 + pageaddr * 256 + byteaddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 60); // 20点阵字体：20行×3字节=60字节
        break;

    case FONTKT:
        readaddr = 0x00008700 + FonfAddr;                    //(KT_ASCII_20_ADDRESS+sectoraddr) * 4096 + pageaddr * 256 + byteaddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 60); // 20点阵字体：20行×3字节=60字节
        break;

    case FONTHT:
        readaddr = 0x00004980 + FonfAddr;                    //(HT_ASCII_20_ADDRESS+sectoraddr) * 4096 + pageaddr * 256 + byteaddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 60); // 20点阵字体：20行×3字节=60字节
        break;
    }

    for (j = 0; j < 60; j++)
    {
        u8 byteInRow = j % 3; // 0,1,2
        u8 rowInChar = j / 3; // 0~19

        // 20点阵 ASCII，半宽显示 = 10 像素
        // byte0: 8位
        // byte1: 2位
        // byte2: 不显示
        if (byteInRow >= 2)
        {
            continue; // 第三字节直接丢弃
        }

        u8 bitLimit = (byteInRow == 0) ? 8 : 2;

        DZaddr = rowcnt * 120 + colcnt + rowInChar * 120 + byteInRow * 8;

        if (DZaddr < DZAmax)
        {
            for (u8 i = 0; i < bitLimit; i++)
            {
                if ((DZaddr + i) < DZAmax)
                {
                    if (((font_buf[j] << i) & 0x80) == 0)
                        sourceBuf[DZaddr + i] = BLACK;
                    else
                        sourceBuf[DZaddr + i] = fontColor;
                }
            }
        }
    }

    colcnt += 10; // 字间距，20点阵字体宽度是20像素（字间距等于字符宽度，字符之间无空隙）

    if ((colcnt > 110) && (lingOrAll == 1)) // 全屏显示的时候需要换行，单行显示的时候不用
    {
        rowcnt += 20; // 换行间距
        colcnt = 0;
    }

    returnBuf[0] = rowcnt;
    returnBuf[1] = colcnt;
    returnBuf[2] = 0; // tempASCIICnt，单行显示时不需要
    returnBuf[3] = 0; // tempShift，单行显示时不需要

    return returnBuf;
}

u8 *readTwentyHzData(u8 *fontstr, u8 rowCnt, u8 colCnt, u8 asciiCnt, u8 shift, u8 lingOrAll) // 读取22点阵Hz码写入原始缓存区,有单行或者全屏显示的区别
{
    u32 readaddr = 0;
    u32 FonfAddr = 0;
    u8 j = 0, tempASCIICnt = 0, tempShift = 0;
    u8 rowcnt = 0, colcnt = 0;
    static u8 returnBuf[4];

    rowcnt = rowCnt;
    colcnt = colCnt;
    tempASCIICnt = asciiCnt;
    tempShift = shift;
    switch (fontType)
    {
    case FONTST:

        FonfAddr = 0x00513650 + (94 * (fontstr[0] - 0xA1) + (fontstr[1] - 0xA1)) * 60;
        readaddr = FonfAddr;

        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, FonfAddr, 60);

        break;

    case FONTFS:

        FonfAddr = 0x000EE0A0 + (94 * (fontstr[0] - 0xA1) + (fontstr[1] - 0xA1)) * 60;
        readaddr = FonfAddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 60);
        break;

    case FONTHT:
        FonfAddr = 0x0024FD30 + (94 * (fontstr[0] - 0xA1) + (fontstr[1] - 0xA1)) * 60;
        readaddr = FonfAddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 60);
        break;

    case FONTKT:
        FonfAddr = 0x003B19C0 + (94 * (fontstr[0] - 0xA1) + (fontstr[1] - 0xA1)) * 60;
        readaddr = FonfAddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 60);
        break;
    }
    ///////////////////////////////////////
    if ((colcnt >= 100 + 2) && (lingOrAll == 1)) // 全屏显示的时候需要换行，单行显示的时候不用
    {
        if ((fontstr[0] >= 0xb0) && (fontstr[0] <= 0xf7) && // 判断字符是否为GB2312汉字内码
            (fontstr[1] >= 0xa1) && (fontstr[1] <= 0xfe))
        {
            rowcnt += 20;
        }
        colcnt = 0;
        tempASCIICnt = 0;
        tempShift = 0;
    }

    for (j = 0; j < 60; j++)
    {
        // 20点阵字体：每行3字节(24位)，但只用20位
        // 地址计算：rowcnt*120 + colcnt + (j/3)*120 + (j%3)*8
        // 但是，如果j%3==2（第三字节），只使用前4位（20-16=4），需要限制写入范围
        u8 byteInRow = j % 3; // 当前字节在行中的位置(0,1,2)
        u8 rowInChar = j / 3; // 当前行在字符中的位置(0-19)

        if (byteInRow == 2 && rowInChar < 20) // 第三字节，只使用前4位
        {
            // 第三字节只使用前4位（20像素宽度，前两字节16位，第三字节4位）
            DZaddr = rowcnt * 120 + colcnt + rowInChar * 120 + byteInRow * 8;
            if (DZaddr < DZAmax)
            {
                for (u8 i = 0; i < 4; i++) // 只写入4个像素
                {
                    if ((DZaddr + i) < DZAmax)
                    {
                        if (((font_buf[j] << i) & 0x80) == 0)
                            sourceBuf[DZaddr + i] = BLACK;
                        else
                            sourceBuf[DZaddr + i] = fontColor;
                    }
                }
            }
        }
        else
        {
            // 第一字节和第二字节，写入全部8位
            DZaddr = rowcnt * 120 + colcnt + rowInChar * 120 + byteInRow * 8;
            if (DZaddr < DZAmax)
            {
                for (u8 i = 0; i < 8; i++)
                {
                    if ((DZaddr + i) < DZAmax)
                    {
                        if (((font_buf[j] << i) & 0x80) == 0)
                            sourceBuf[DZaddr + i] = BLACK;
                        else
                            sourceBuf[DZaddr + i] = fontColor;
                    }
                }
            }
        }
    }

    colcnt += 20; // 字间距，20点阵字体宽度是20像素（字间距等于字符宽度，字符之间无空隙）

    if ((colcnt > 100 + 2) && (lingOrAll == 1))
    {

        rowcnt += 20;
        colcnt = 0;
        tempASCIICnt = 0;
        tempShift = 0;
    }

    returnBuf[0] = rowcnt;
    returnBuf[1] = colcnt;
    returnBuf[2] = tempASCIICnt;
    returnBuf[3] = tempShift;

    return returnBuf;
}

// 读取22点阵ASCII码写入原始缓存区,有单行或者全屏显示的区别

u8 *readTwentyTwoAsciiData(u8 fontstr, u8 rowCnt, u8 colCnt, u8 lingOrAll) // 读取22点阵ASCII码写入原始缓存区,有单行或者全屏显示的区别
{

    u32 readaddr = 0;
    u32 FonfAddr = 0;
    u8 j = 0;
    u8 rowcnt = 0, colcnt = 0;
    static u8 returnBuf[2] = {0};

    rowcnt = rowCnt;
    colcnt = colCnt;

    switch (fontType)
    {
    case FONTST:
        FonfAddr = 0x009B1518 + (uint32_t)(fontstr - 32) * 66;
        readaddr = FonfAddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 66);
        break;

    case FONTFS:
        FonfAddr = 0x00BF1038 + (uint32_t)fontstr * 66;
        readaddr = FonfAddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 16);
        break;

    case FONTKT:
        FonfAddr = 0x00BF41B8 + (uint32_t)(fontstr - 32) * 66;
        readaddr = FonfAddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 16);
        break;

    case FONTHT:
        FonfAddr = 0x00BF28F8 + (uint32_t)(fontstr - 32) * 66;
        readaddr = FonfAddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 16);
        break;
    }

    for (j = 0; j < 66; j++)
    {

        DZaddr = rowcnt * 120 + colcnt + j / 3 * 120 + (j % 3) * 8;

        if (DZaddr < DZAmax)
        {
            for (u8 i = 0; i < 8; i++)
            {
                if (((font_buf[j] << i) & 0x80) == 0)
                    sourceBuf[DZaddr + i] = BLACK;
                else
                    sourceBuf[DZaddr + i] = fontColor;
            }
        }
    }

    colcnt += 11;

    if ((colcnt > 113) && (lingOrAll == 1)) // 全屏显示的时候需要换行，单行显示的时候不用
    {
        rowcnt += 22;
        colcnt = 0;
    }

    returnBuf[0] = rowcnt;
    returnBuf[1] = colcnt;

    return returnBuf;
}

u8 *readTwentyTwoHzData(u8 *fontstr, u8 rowCnt, u8 colCnt, u8 asciiCnt, u8 shift, u8 lingOrAll) // 读取22点阵Hz码写入原始缓存区,有单行或者全屏显示的区别
{
    u32 readaddr = 0;
    u32 FonfAddr = 0;
    u8 j = 0, tempASCIICnt = 0, tempShift = 0;
    u8 rowcnt = 0, colcnt = 0;
    static u8 returnBuf[4];

    rowcnt = rowCnt;
    colcnt = colCnt;
    tempASCIICnt = asciiCnt;
    tempShift = shift;
    switch (fontType)
    {
    case FONTST:

        FonfAddr = 0x00B62A30 + (94 * (fontstr[0] - 0xA1) + (fontstr[1] - 0xA1)) * 66;
        readaddr = FonfAddr;

        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, FonfAddr, 66);

        break;

    case FONTFS:

        FonfAddr = 0x00BF5A78 + (94 * (fontstr[0] - 0xA1) + (fontstr[1] - 0xA1)) * 66;
        readaddr = FonfAddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 72);
        break;

    case FONTHT:
        FonfAddr = 0x00C84080 + (94 * (fontstr[0] - 0xA1) + (fontstr[1] - 0xA1)) * 66;
        readaddr = FonfAddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 72);
        break;

    case FONTKT:
        FonfAddr = 0x00D12688 + (94 * (fontstr[0] - 0xA1) + (fontstr[1] - 0xA1)) * 66;
        readaddr = FonfAddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 72);
        break;
    }

    if ((colcnt > 96) && (lingOrAll == 1)) // 全屏显示的时候需要换行，单行显示的时候不用
    {
        if ((fontstr[0] >= 0xb0) && (fontstr[0] <= 0xf7) && // 判断字符是否为GB2312汉字内码
            (fontstr[1] >= 0xa1) && (fontstr[1] <= 0xfe))
        {
            rowcnt += 22;
        }
        colcnt = 0;
        tempASCIICnt = 0;
        tempShift = 0;
    }

    for (j = 0; j < 66; j++)
    {
        DZaddr = rowcnt * 120 + colcnt + j / 3 * 120 + (j % 3) * 8;
        if (DZaddr < DZAmax)
        {
            for (u8 i = 0; i < 8; i++)
            {
                if (((font_buf[j] << i) & 0x80) == 0)
                    sourceBuf[DZaddr + i] = BLACK;
                else
                    sourceBuf[DZaddr + i] = fontColor;
            }
        }
    }

    colcnt += 22;

    if ((colcnt > 96) && (lingOrAll == 1))
    {

        rowcnt += 22;
        colcnt = 0;
        tempASCIICnt = 0;
        tempShift = 0;
    }

    returnBuf[0] = rowcnt;
    returnBuf[1] = colcnt;
    returnBuf[2] = tempASCIICnt;
    returnBuf[3] = tempShift;

    return returnBuf;
}

u8 *readTwentyFourAsciiData(u8 fontstr, u8 rowCnt, u8 colCnt, u8 asciiCnt, u8 lingOrAll) // 读取24点阵ASCII码写入原始缓存区,有单行或者全屏显示的区别
{
    u32 readaddr = 0;
    u32 FonfAddr = 0;
    u8 j = 0, tempASCIICnt = 0, tempShift = 0;
    u8 rowcnt = 0, colcnt = 0;
    static u8 returnBuf[4] = {0};

    rowcnt = rowCnt;
    colcnt = colCnt;
    tempASCIICnt = asciiCnt;

    FonfAddr = ((uint32_t)fontstr - 32) * 72;
    switch (fontType)
    {
    case FONTST:

        readaddr = 0x00630260 + FonfAddr; //(ST_ASCII_24_ADDRESS+sectoraddr) * 4096 + (pageaddr+3) * 256 + byteaddr;

        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 72);
        break;

    case FONTFS:
        readaddr = 0x00002280 + FonfAddr; //(FS_ASCII_24_ADDRESS+sectoraddr) * 4096 + (pageaddr+12) * 256 + byteaddr-224;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 72);
        break;

    case FONTKT:
        readaddr = 0x00009D80 + FonfAddr; // (KT_ASCII_24_ADDRESS+sectoraddr) * 4096 + (pageaddr+3) * 256 + byteaddr+64;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 72);
        break;

    case FONTHT:
        readaddr = 0x00006000 + FonfAddr; //(HT_ASCII_24_ADDRESS+sectoraddr) * 4096 + (pageaddr+12) * 256 + byteaddr-160;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 72);
        break;
    }

    for (j = 0; j < 72; j++)
    {
        u8 byteInRow = j % 3; // 0,1,2
        u8 rowInChar = j / 3; // 0~23

        // 只显示左 12 像素：
        // 第 0 字节：8 像素
        // 第 1 字节：只要前 4 像素
        // 第 2 字节：不要
        if (byteInRow >= 2)
        {
            continue; // 直接跳过第 3 个字节
        }

        DZaddr = rowcnt * 120 + colcnt + rowInChar * 120 + byteInRow * 8;

        if (DZaddr < DZAmax)
        {
            u8 bitLimit = (byteInRow == 0) ? 8 : 4; // 第2字节只画4点

            for (u8 i = 0; i < bitLimit; i++)
            {
                if ((DZaddr + i) < DZAmax)
                {
                    if (((font_buf[j] << i) & 0x80) == 0)
                        sourceBuf[DZaddr + i] = BLACK;
                    else
                        sourceBuf[DZaddr + i] = fontColor;
                }
            }
        }
    }

    colcnt += 12; // 字间距，24点阵ASCII字符实际显示宽度12像素（字体数据24像素，但实际显示12像素）

    tempASCIICnt++;

    if ((colcnt > 108) && (lingOrAll == 1)) // 全屏显示的时候需要换行，单行显示的时候不用
    {
        colcnt = 0;
        rowcnt += 24;
        tempASCIICnt = 0;
        tempShift = 0;
    }

    returnBuf[0] = rowcnt;
    returnBuf[1] = colcnt;
    returnBuf[2] = tempASCIICnt;
    returnBuf[3] = tempShift;

    return returnBuf;
}

u8 *readTwentyFourHzData(u8 *fontstr, u8 rowCnt, u8 colCnt, u8 asciiCnt, u8 shift, u8 lingOrAll) // 读取24点阵Hz码写入原始缓存区,有单行或者全屏显示的区别
{
    u32 readaddr = 0;
    u32 FonfAddr = 0;
    u8 j = 0, tempASCIICnt = 0, tempShift = 0;
    u8 rowcnt = 0, colcnt = 0;
    static u8 returnBuf[4] = {0};

    rowcnt = rowCnt;
    colcnt = colCnt;
    tempASCIICnt = asciiCnt;
    tempShift = shift;

    FonfAddr = (94 * (fontstr[0] - 0xA1) + (fontstr[1] - 0xA1)) * 72;
    //	sectoraddr  = FonfAddr/4096;
    //	pageaddr    = (FonfAddr%4096)/256;
    //	byteaddr    = (FonfAddr%4096)%256;

    switch (fontType)
    {
    case FONTST:
        readaddr = 0x00594D40 + FonfAddr; //(ST_GB2312_24_ADDRESS+sectoraddr+1) * 4096 + (pageaddr-12) * 256 + byteaddr-128;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 72);
        break;

    case FONTFS:
        readaddr = 0x0016F790 + FonfAddr; //(FS_GB2312_24_ADDRESS+sectoraddr+1) * 4096 + (pageaddr-7) * 256 + byteaddr-64;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 72);
        break;

    case FONTKT:
        readaddr = 0x004330B0 + FonfAddr; //(KT_GB2312_24_ADDRESS+sectoraddr+1) * 4096 + (pageaddr-2) * 256 + byteaddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 72);
        break;

    case FONTHT:
        readaddr = 0x002D1420 + FonfAddr; //(HT_GB2312_24_ADDRESS+sectoraddr+1) * 4096 + (pageaddr-12) * 256 + byteaddr-192;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 72);
        break;
    }

    if ((colcnt > 96) && (lingOrAll == 1)) // 全屏显示的时候需要换行，单行显示的时候不用
    {
        if ((fontstr[0] >= 0xb0) && (fontstr[0] <= 0xf7) && // 判断字符是否为GB2312汉字内码
            (fontstr[1] >= 0xa1) && (fontstr[1] <= 0xfe))
        {
            rowcnt += 24;
        }
        colcnt = 0;
        tempASCIICnt = 0;
        tempShift = 0;
    }

    for (j = 0; j < 72; j++)
    {
        // 24点阵字体：每行3字节(24位)，正好24像素宽度，完全匹配
        // 地址计算：rowcnt*120 + colcnt + (j/3)*120 + (j%3)*8
        u8 byteInRow = j % 3; // 当前字节在行中的位置(0,1,2)
        u8 rowInChar = j / 3; // 当前行在字符中的位置(0-23)

        DZaddr = rowcnt * 120 + colcnt + rowInChar * 120 + byteInRow * 8;
        if (DZaddr < DZAmax)
        {
            for (u8 i = 0; i < 8; i++)
            {
                if ((DZaddr + i) < DZAmax) // 边界检查，防止数组越界
                {
                    if (((font_buf[j] << i) & 0x80) == 0)
                        sourceBuf[DZaddr + i] = BLACK;
                    else
                        sourceBuf[DZaddr + i] = fontColor;
                }
            }
        }
    }

    colcnt += 24;

    if ((colcnt > 96) && (lingOrAll == 1))
    {
        rowcnt += 24;
        colcnt = 0;
        tempASCIICnt = 0;
        tempShift = 0;
    }

    returnBuf[0] = rowcnt;
    returnBuf[1] = colcnt;
    returnBuf[2] = tempASCIICnt;
    returnBuf[3] = tempShift;

    return returnBuf;
}

u8 *readThirtyTwoAsciiData(u8 fontstr, u8 rowCnt, u8 colCnt, u8 lingOrAll) // 读取32点阵ASCII码写入原始缓存区,有单行或者全屏显示的区别
{
    u32 readaddr;
    u32 FonfAddr = 0;
    u32 sectoraddr, pageaddr, byteaddr;
    u8 j;
    u8 rowcnt, colcnt;
    static u8 returnBuf[2];

    rowcnt = rowCnt;
    colcnt = colCnt;

    FonfAddr = ((uint32_t)fontstr) * 64;
    sectoraddr = FonfAddr / 4096;
    pageaddr = (FonfAddr % 4096) / 256;
    byteaddr = (FonfAddr % 4096) % 256;
    switch (fontType)
    {
    case FONTST:
        readaddr = (ST_ASCII_32_ADDRESS + sectoraddr + 1) * 4096 + (pageaddr - 7) * 256 + byteaddr - 128;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 64);
        break;

    case FONTFS:
        readaddr = (FS_ASCII_32_ADDRESS + sectoraddr + 1) * 4096 + (pageaddr - 7) * 256 + byteaddr - 96;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 64);
        break;

    case FONTKT:
        readaddr = (KT_ASCII_32_ADDRESS + sectoraddr + 1) * 4096 + (pageaddr - 7) * 256 + byteaddr - 64;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 64);
        break;

    case FONTHT:
        readaddr = (HT_ASCII_32_ADDRESS + sectoraddr + 1) * 4096 + (pageaddr - 7) * 256 + byteaddr - 32;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 64);
        break;
    }

    for (j = 0; j < 64; j++)
    {
        DZaddr = rowcnt * 120 + colcnt + j / 2 * 120 + (j % 2) * 8;
        if (DZaddr < DZAmax)
        {
            for (u8 i = 0; i < 8; i++)
            {
                if (((font_buf[j] << i) & 0x80) == 0)
                    sourceBuf[DZaddr + i] = BLACK;
                else
                    sourceBuf[DZaddr + i] = fontColor;
            }
        }
    }

    colcnt += 16;
    if ((colcnt > 104) && (lingOrAll == 1)) // 全屏显示的时候需要换行，单行显示的时候不用
    {
        colcnt = 0;
        rowcnt += 32;
    }

    returnBuf[0] = rowcnt;
    returnBuf[1] = colcnt;

    return returnBuf;
}

u8 *readThirtyTwoHzData(u8 *fontstr, u8 rowCnt, u8 colCnt, u8 lingOrAll) // 读取32点阵Hz码写入原始缓存区,有单行或者全屏显示的区别
{
    u32 readaddr;
    u32 FonfAddr = 0;
    u32 sectoraddr, pageaddr, byteaddr;
    u8 j;
    u8 rowcnt, colcnt;
    static u8 returnBuf[2];

    rowcnt = rowCnt;
    colcnt = colCnt;

    FonfAddr = (94 * (fontstr[0] - 0xA1) + (fontstr[1] - 0xA1)) * 128;
    sectoraddr = FonfAddr / 4096;
    pageaddr = (FonfAddr % 4096) / 256;
    byteaddr = (FonfAddr % 4096) % 256;

    switch (fontType)
    {
    case FONTST:
        readaddr = (ST_GB2312_32_ADDRESS + sectoraddr + 1) * 4096 + (pageaddr - 7) * 256 + byteaddr;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 128);
        break;

    case FONTFS:
        readaddr = (FS_GB2312_32_ADDRESS + sectoraddr + 1) * 4096 + (pageaddr - 5) * 256 + byteaddr + 32;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 128);
        break;

    case FONTKT:
        readaddr = (KT_GB2312_32_ADDRESS + sectoraddr + 1) * 4096 + (pageaddr - 3) * 256 + byteaddr + 64;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 128);
        break;

    case FONTHT:
        readaddr = (HT_GB2312_32_ADDRESS + sectoraddr + 1) * 4096 + (pageaddr - 1) * 256 + byteaddr + 96;
        BSP_W25Qx_ReadDMA(&hw25q64, font_buf, readaddr, 128);
        break;
    }

    if ((colcnt > 88) && (lingOrAll == 1)) // 全屏显示的时候需要换行，单行显示的时候不用
    {
        if ((fontstr[0] >= 0xb0) && (fontstr[0] <= 0xf7) && // 判断字符是否为GB2312汉字内码
            (fontstr[1] >= 0xa1) && (fontstr[1] <= 0xfe))
        {
            rowcnt += 32;
        }
        colcnt = 0;
    }

    for (j = 0; j < 128; j++)
    {
        DZaddr = rowcnt * 120 + colcnt + j / 4 * 120 + (j % 4) * 8;
        if (DZaddr < DZAmax)
        {
            for (u8 i = 0; i < 8; i++)
            {
                if (((font_buf[j] << i) & 0x80) == 0)
                    sourceBuf[DZaddr + i] = BLACK;
                else
                    sourceBuf[DZaddr + i] = fontColor;
            }
        }
    }

    colcnt += 32;

    if ((colcnt > 88) && (lingOrAll == 1)) // 全屏显示的时候需要换行，单行显示的时候不用
    {
        rowcnt += 32;
        colcnt = 0;
    }

    returnBuf[0] = rowcnt;
    returnBuf[1] = colcnt;

    return returnBuf;
}
