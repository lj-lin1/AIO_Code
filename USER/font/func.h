#ifndef __FUNC_H
#define __FUNC_H
#include "main.h"
#include "stdbool.h"

#define FONTST 0 // 宋体
#define FONTFS 1 // 仿宋
#define FONTKT 2 // 楷体
#define FONTHT 3 // 黑体

#define FONT16 0 // 16点阵字库
#define FONT20 1 // 32点阵字库
#define FONT24 2 // 24点阵字库

/*16点阵*/
#define ST_ASCII_16_ADDRESS  5424
#define FS_ASCII_16_ADDRESS  0
#define KT_ASCII_16_ADDRESS  3616
#define HT_ASCII_16_ADDRESS  1808

#define ST_GB2312_16_ADDRESS 749504
#define FS_GB2312_16_ADDRESS 7232
#define KT_GB2312_16_ADDRESS 502080
#define HT_GB2312_16_ADDRESS 254656

#define ST_ASCII_24_ADDRESS  1139456 / 4096
#define FS_ASCII_24_ADDRESS  1145632 / 4096
#define KT_ASCII_24_ADDRESS  1151808 / 4096
#define HT_ASCII_24_ADDRESS  1157984 / 4096

#define ST_GB2312_24_ADDRESS 1164160 / 4096
#define FS_GB2312_24_ADDRESS 1800384 / 4096
#define KT_GB2312_24_ADDRESS 2436608 / 4096
#define HT_GB2312_24_ADDRESS 3072832 / 4096

#define ST_ASCII_32_ADDRESS  3709056 / 4096
#define FS_ASCII_32_ADDRESS  3717280 / 4096
#define KT_ASCII_32_ADDRESS  3725504 / 4096
#define HT_ASCII_32_ADDRESS  3733728 / 4096

#define ST_GB2312_32_ADDRESS 3741952 / 4096
#define FS_GB2312_32_ADDRESS 4872992 / 4096
#define KT_GB2312_32_ADDRESS 6004032 / 4096
#define HT_GB2312_32_ADDRESS 7135072 / 4096

// 增加22号字体 2025-9-3
#define ST_GB2312_22_ADDRESS 0x009B1518 //   (7135072+1131040)/4096
#define FS_GB2312_22_ADDRESS 0x00806300
#define KT_GB2312_22_ADDRESS 0x00922F10
#define HT_GB2312_22_ADDRESS 0x00894908

#define ST_ASCII_22_ADDRESS  0x00804A40
#define FS_ASCII_22_ADDRESS  0x00800000
#define KT_ASCII_22_ADDRESS  0x00803180
#define HT_ASCII_22_ADDRESS  0x008018C0

// 1131040	8266112

#define IPADDR 8 * 1024 * 1024 - 20 // IP地址偏移地址
// #define PARAMADDR										8*1024*1024-20   // 屏体参数偏移地址
#define AGREEMENTADDR 8 * 1024 * 1024 - 40  // 协议设置
#define BUADADDR      8 * 1024 * 1024 - 60  // 波特率设置
#define FONTSIZEADDR  8 * 1024 * 1024 - 80  // 点阵大小
#define FONTTYPEADDR  8 * 1024 * 1024 - 100 // 字体类型
#define LIGHTADDR     8 * 1024 * 1024 - 200 // 亮度参数偏移地址

#define fontNoting    0
#define fontASCII     1
#define fontHZ        2
extern u8 fontType;
extern u8 fontSize;

void app_funcs_fill(uint8_t color);
void app_funcs_clear(u8 row);

void app_funcs_makefonttolatt_all(u8 *fontstr, u32 length, u8 x, u8 y, u8 color);             // 全屏显示分发程序
void app_funcs_makefonttolatt_oneline(u8 *fontstr, u32 length, u8 x, u8 y, u8 row, u8 color); // 单行显示分发程序
void MakeSixteenLattAll(u8 *fontstr, u32 length, u8 x, u8 y, u8 color);                       // 16点阵全屏显示
void MakeTwentyFourLattAll(u8 *fontstr, u32 length, u8 x, u8 y, u8 color);                    // 24点阵全屏显示
void MakeTwentytwoLattAll(u8 *fontstr, u32 length, u8 x, u8 y, u8 color);
void MakeThirtyTwoLattAll(u8 *fontstr, u32 length, u8 x, u8 y, u8 color);              // 32点阵全屏显示
void MakeSixteenLattOneLine(u8 *fontstr, u32 length, u8 x, u8 y, u8 row, u8 color);    // 16点阵单行显示
void MakeTwentyFourLattOneLine(u8 *fontstr, u32 length, u8 x, u8 y, u8 row, u8 color); // 24点阵单行显示
void MakeThirtyTwoLattOneLine(u8 *fontstr, u32 length, u8 x, u8 y, u8 row, u8 color);  // 32点阵单行显示
u8 *readSixteenAsciiData(u8 fontstr, u8 rowCnt, u8 colCnt, u8 lingOrAll);              // 读取16点阵ASCII码写入原始缓存区,有单行或者全屏显示的区别
u8 *readSixteenHzData(u8 *fontstr, u8 rowCnt, u8 colCnt, u8 lingOrAll);                // 读取16点阵Hz码写入原始缓存区,有单行或者全屏显示的区别

u8 *readTwentyFourAsciiData(u8 fontstr, u8 rowCnt, u8 colCnt, u8 asciiCnt, u8 lingOrAll);         // 读取24点阵ASCII码写入原始缓存区,有单行或者全屏显示的区别
u8 *readTwentyFourHzData(u8 *fontstr, u8 rowCnt, u8 colCnt, u8 asciiCnt, u8 shift, u8 lingOrAll); // 读取24点阵Hz码写入原始缓存区,有单行或者全屏显示的区别

u8 *readThirtyTwoAsciiData(u8 fontstr, u8 rowCnt, u8 colCnt, u8 lingOrAll); // 读取32点阵ASCII码写入原始缓存区,有单行或者全屏显示的区别
u8 *readThirtyTwoHzData(u8 *fontstr, u8 rowCnt, u8 colCnt, u8 lingOrAll);   // 读取32点阵Hz码写入原始缓存区,有单行或者全屏显示的区别

u8 *readTwentyTwoAsciiData(u8 fontstr, u8 rowCnt, u8 colCnt, u8 lingOrAll);                      // 读取22点阵ASCII码写入原始缓存区,有单行或者全屏显示的区别
u8 *readTwentyTwoHzData(u8 *fontstr, u8 rowCnt, u8 colCnt, u8 asciiCnt, u8 shift, u8 lingOrAll); // 读取22点阵Hz码写入原始缓存区,有单行或者全屏显示的区别

u8 *readTwentyAsciiData(u8 fontstr, u8 rowCnt, u8 colCnt, u8 lingOrAll);                      // 读取22点阵ASCII码写入原始缓存区,有单行或者全屏显示的区别
u8 *readTwentyHzData(u8 *fontstr, u8 rowCnt, u8 colCnt, u8 asciiCnt, u8 shift, u8 lingOrAll); // 读取22点阵Hz码写入原始缓存区,有单行或者全屏显示的区别

void MakeTwentyTwoLattOneLine(u8 *fontstr, u32 length, u8 x, u8 y, u8 row, u8 color);
void MakeTwentytwoLattAll(u8 *fontstr, u32 length, u8 x, u8 y, u8 color); // 22

void MakeTwentyLattAll(u8 *fontstr, u32 length, u8 x, u8 y, u8 color); // 20
void MakeTwentyOneLine(u8 *fontstr, u32 length, u8 x, u8 y, u8 row, u8 color);

#endif
