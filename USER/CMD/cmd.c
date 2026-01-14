/**
 ******************************************************************************
 * @file    cmd.c
 * @brief   协议命令处理与设备控制
 *
 * 功能说明：
 *  1. 显示、清屏、亮度、字体控制
 *  2. 通行灯、黄闪、全屏点亮
 *  3. IP/端口配置与 Flash 保存
 *  4. 语音播报控制
 *  5. APP / IAP 网络协议解析
 *  6. 网络环形缓冲区数据处理
 *
 ******************************************************************************
 */

#include "cmd.h"
#include "RS232.h"
#include "display.h"
#include "func.h"
#include "stdbool.h"
#include "stdint.h"
#include "string.h"
#include "tcp_server.h"
#include "voice.h"
#include "w25qxx.h"

extern u8 lightLev; // 亮度等级

/*==============================================================================
 * 通用命令应答
 *============================================================================*/

/**
 * @brief  根据当前通信端口返回应答数据
 */
void cmdnack(u8 *inbuf, u16 len)
{
    Net_Send(inbuf, len);
}

/*==============================================================================
 * 显示与清屏控制
 *============================================================================*/

/**
 * @brief  显示控制（全屏 / 单行）
 */
void cmd_disPlayAll_ctrl(u8 *inbuf, u16 len)
{
    u8 ZTColor = RED;

    if (inbuf[6] <= 2)
    {
        ZTColor = inbuf[6] + 1;
    }
    else
    {
        ZTColor = RED;
    }

    fontColor = ZTColor;

    if (inbuf[5] < 3)
    {
        fontSize = inbuf[5];
    }
    else
    {
        fontSize = FONT16;
    }

    if (inbuf[4] == 0x00)
    {
        app_funcs_makefonttolatt_all(&inbuf[7], len - 9, 0, 0, ZTColor);
    }
    else if (inbuf[4] <= 17)
    {
        app_funcs_makefonttolatt_oneline(&inbuf[7], len - 9, 0, 0,
                                         inbuf[4] - 1, ZTColor);
    }

    cmdnack(inbuf, len);
}

/**
 * @brief  清屏控制
 */
void cmd_clear_ctrl(u8 *inbuf, u16 len)
{
    if (inbuf[4] == 0x00)
    {
        app_funcs_clear(255);
    }
    else if (inbuf[4] <= 17)
    {
        app_funcs_clear(inbuf[4] - 1);
    }

    cmdnack(inbuf, len);
}

/*==============================================================================
 * 亮度控制
 *============================================================================*/

/**
 * @brief  亮度设置（1~8 级）
 */
void cmd_setLight_ctrl(u8 *inbuf, u16 len)
{
    if ((inbuf[4] > 0) && (inbuf[4] <= 8))
    {
        lightLev = inbuf[4];
    }

    cmdnack(inbuf, len);

    u8 LIGHT_CHECK_MSG[9] = "setlight";
    BSP_W25Qx_EraseWrite(&hw25q64, LIGHT_CHECK_MSG, LIGHTADDR, 8);
    BSP_W25Qx_EraseWrite(&hw25q64, &inbuf[4], LIGHTADDR + 8, 1);
}

/*==============================================================================
 * 通行灯 / 黄闪控制
 *============================================================================*/

/**
 * @brief  通行灯控制
 */
void cmd_lamp_ctrl(u8 *inbuf, u16 len)
{
    if (inbuf[4] == 0)
    {
        LAMP = 0;
    }
    else if (inbuf[4] == 1)
    {
        LAMP = 1;
    }

    RS232_Send(RS232_PORT_USART3, inbuf, len);

    cmdnack(inbuf, len);
}

/**
 * @brief  黄闪控制
 */
void cmd_hs_ctrl(u8 *inbuf, u16 len)
{
    if (inbuf[4] == 0)
    {
        HS = 0;
    }
    else if (inbuf[4] == 1)
    {
        HS = 1;
    }

    cmdnack(inbuf, len);
}

/*==============================================================================
 * 全屏点亮
 *============================================================================*/

/**
 * @brief  设置全屏点亮颜色
 */
void cmd_setfullscreen_ctrl(u8 *inbuf, u16 len)
{
    if (inbuf[4] == 0x00)
    {
        app_funcs_fill(RED);
    }
    else if (inbuf[4] == 0x01)
    {
        app_funcs_fill(GREEN);
    }
    else if (inbuf[4] == 0x02)
    {
        app_funcs_fill(YELLOW);
    }

    cmdnack(inbuf, len);
}

/*==============================================================================
 * 字体设置
 *============================================================================*/

/**
 * @brief  保存字体大小与类型
 */
void cmd_setfontsize_ctrl(u8 *inbuf, u16 len)
{
    u8 fontSizeBuf[11] = "setfontsize";
    u8 tempFontSize = fontSize;
    u8 tempFontType = fontType;

    BSP_W25Qx_EraseWrite(&hw25q64, fontSizeBuf, FONTSIZEADDR, 11);
    BSP_W25Qx_EraseWrite(&hw25q64, &tempFontSize, FONTSIZEADDR + 11, 1);
    BSP_W25Qx_EraseWrite(&hw25q64, &tempFontType, FONTSIZEADDR + 12, 1);

    cmdnack(inbuf, len);
}

/*==============================================================================
 * IP 设置
 *============================================================================*/

/**
 * @brief  设置 IP / 掩码 / 网关 / 端口并重启
 */
void cmd_Setip_ctrl(u8 *inbuf, u16 len)
{
    cmdnack(inbuf, len);

    F407_IP[0] = inbuf[4];
    F407_IP[1] = inbuf[5];
    F407_IP[2] = inbuf[6];
    F407_IP[3] = inbuf[7];

    F407_NETMASK[0] = inbuf[8];
    F407_NETMASK[1] = inbuf[9];
    F407_NETMASK[2] = inbuf[10];
    F407_NETMASK[3] = inbuf[11];

    F407_WG[0] = inbuf[12];
    F407_WG[1] = inbuf[13];
    F407_WG[2] = inbuf[14];
    F407_WG[3] = inbuf[15];

    F407_PORT = (inbuf[16] << 8) + inbuf[17];

    u8 IP_CHECK_MSG[6] = "setip";

    BSP_W25Qx_EraseWrite(&hw25q64, IP_CHECK_MSG, IPADDR, 5);
    BSP_W25Qx_EraseWrite(&hw25q64, F407_IP, IPADDR + 5, 4);
    BSP_W25Qx_EraseWrite(&hw25q64, F407_NETMASK, IPADDR + 9, 4);
    BSP_W25Qx_EraseWrite(&hw25q64, F407_WG, IPADDR + 13, 4);
    BSP_W25Qx_EraseWrite(&hw25q64, &inbuf[16], IPADDR + 17, 1);
    BSP_W25Qx_EraseWrite(&hw25q64, &inbuf[17], IPADDR + 18, 1);

    // osDelay(100);
    // NVIC_SystemReset();
}

/*==============================================================================
 * 语音播报
 *============================================================================*/

/**
 * @brief  语音播报控制
 */
void cmd_YY_ctrl(u8 *inbuf, u16 len)
{
    voice_msg_t msg;

    msg.volume = (inbuf[4] <= 8) ? inbuf[4] : 8;
    msg.len = len - 7;

    if (msg.len > VOICE_MAX_LEN)
        msg.len = VOICE_MAX_LEN;

    memcpy(msg.data, &inbuf[5], msg.len);

    Voice_SendRequest(&msg);

    cmdnack(inbuf, len);
}

/*==============================================================================
 * CRC 校验
 *============================================================================*/

#define CRC_POLYNOM 0x8408
#define CRC_INIVAL 0xFFFF

u16 calc_crc16(u16 init_crc, u8 *crc_data, int len)
{
    u16 crc = init_crc;

    for (int cnt = 0; cnt < len; cnt++)
    {
        crc ^= crc_data[cnt];
        for (u8 k = 0; k < 8; k++)
        {
            if (crc & 0x1)
                crc = (crc >> 1) ^ CRC_POLYNOM;
            else
                crc >>= 1;
        }
    }

    return (~crc) & 0xFFFF;
}

/*==============================================================================
 * 命令编号解析
 *============================================================================*/

/**
 * @brief  根据协议命令字获取命令索引
 * @param  cmdstr 协议中的命令码
 * @return 命令函数表索引，255 表示无效命令
 */
u8 getCmdNo(u8 cmdstr)
{
    u8 tempCmd;

    switch (cmdstr)
    {
    case 0x80: /* 显示 */
        tempCmd = 0;
        break;

    case 0x94: /* 清屏 */
        tempCmd = 1;
        break;

    case 0x96: /* 设置亮度 */
        tempCmd = 2;
        break;

    case 0x99: /* 通行灯控制 */
        tempCmd = 3;
        break;

    case 0x98: /* 黄闪控制 */
        tempCmd = 4;
        break;

    case 0x01: /* 修改 IP */
        tempCmd = 5;
        break;

    case 0x03: /* 全屏点亮 */
        tempCmd = 6;
        break;

    case 0x04: /* 获取版本号 */
        tempCmd = 7;
        break;

    case 0x05: /* 设置字体大小、类型 */
        tempCmd = 8;
        break;

    case 0x06: /* 语音播报 */
        tempCmd = 9;
        break;

    case 0x08: /* 设备复位 */
        tempCmd = 10;
        break;

    default:
        tempCmd = 255;
        break;
    }

    return tempCmd;
}

/*==============================================================================
 * 语音播报
 *============================================================================*/

/**
 * @brief  获取设备版本号
 */

void cmd_getedition_ctrl(u8 *inbuf, u16 len)
{
}

/*==============================================================================
 * 语音播报
 *============================================================================*/

/**
 * @brief  设备复位
 */

void cmd_reset_ctrl(u8 *inbuf, u16 len)
{
    NVIC_SystemReset();
}

/*==============================================================================
 * 命令函数表
 *============================================================================*/

/**
 * @brief  命令处理函数指针表
 *         索引值由 getCmdNo() 返回
 */
void (*cmd_functions[24])(u8 *inbuf, u16 len) =
    {
        cmd_disPlayAll_ctrl,    /* 0 显示 */
        cmd_clear_ctrl,         /* 1 清屏 */
        cmd_setLight_ctrl,      /* 2 设置亮度 */
        cmd_lamp_ctrl,          /* 3 通行灯控制 */
        cmd_hs_ctrl,            /* 4 黄闪控制 */
        cmd_Setip_ctrl,         /* 5 修改 IP */
        cmd_setfullscreen_ctrl, /* 6 全屏点亮 */
        cmd_getedition_ctrl,    /* 7 获取版本号 */
        cmd_setfontsize_ctrl,   /* 8 设置字体 */
        cmd_YY_ctrl,            /* 9 语音播报 */
        cmd_reset_ctrl          /* 10 软件复位 */
};

/*==============================================================================
 * APP / IAP 协议处理(升级程序)
 *============================================================================*/

extern u32 IPreadbuff[128];

/**
 * @brief  APP 层特殊协议处理（IAP / 搜索 / 版本查询）
 */
// void cmd_APP(u8 *appbudd)
// {
//     if ((appbudd[0] == 0xff) && (appbudd[1] == 0xff) &&
//         (appbudd[2] == 0) && (appbudd[3] == 0) &&
//         (appbudd[4] == 0) && (appbudd[5] == 0) &&
//         (appbudd[6] == 0) && (appbudd[7] == 2))
//     {
//         u8 banbenbuff[] = {"信路威P4-120x240-3833024-V11"};
//         u16 banbenlen = strlen((char *)banbenbuff);

//         u8 TXbuff[30] =
//             {
//                 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F};

//         /*---------------- IAP 重启指令 ----------------*/
//         if ((appbudd[8] == 7) &&
//             (appbudd[9] == 0x31) &&
//             (appbudd[10] == 0x5B) &&
//             (appbudd[11] == 0xC9))
//         {
//             IPreadbuff[0] = 0x49415000;
//             IPreadbuff[1] = (F407_IP[0] << 24) |
//                             (F407_IP[1] << 16) |
//                             (F407_IP[2] << 8) |
//                             (F407_IP[3]);
//             IPreadbuff[2] = (F407_NETMASK[0] << 24) |
//                             (F407_NETMASK[1] << 16) |
//                             (F407_NETMASK[2] << 8) |
//                             (F407_NETMASK[3]);
//             IPreadbuff[3] = (F407_WG[0] << 24) |
//                             (F407_WG[1] << 16) |
//                             (F407_WG[2] << 8) |
//                             (F407_WG[3]);
//             IPreadbuff[4] = F407_PORT & 0xFFFF;

//             STMFLASH_Write(0x080E0000, IPreadbuff, 128);

//             TXbuff[7] = 0x02;
//             TXbuff[8] = 0xA7;
//             TXbuff[9] = 0x31;
//             TXbuff[10] = 0xF4;
//             TXbuff[11] = 0x36;

//             myUdpSendDtat(TXbuff, 12);
//             delay_ms(100);
//             NVIC_SystemReset();
//         }
//         /*---------------- IP 搜索 ----------------*/
//         else if ((appbudd[8] == 5) &&
//                  (appbudd[9] == 0) &&
//                  (appbudd[10] == 0x48) &&
//                  (appbudd[11] == 0x73))
//         {
//             TXbuff[7] = 0x0F;
//             TXbuff[8] = 0xA5;

//             TXbuff[9] = F407_IP[0];
//             TXbuff[10] = F407_IP[1];
//             TXbuff[11] = F407_IP[2];
//             TXbuff[12] = F407_IP[3];

//             TXbuff[13] = F407_NETMASK[0];
//             TXbuff[14] = F407_NETMASK[1];
//             TXbuff[15] = F407_NETMASK[2];
//             TXbuff[16] = F407_NETMASK[3];

//             TXbuff[17] = F407_WG[0];
//             TXbuff[18] = F407_WG[1];
//             TXbuff[19] = F407_WG[2];
//             TXbuff[20] = F407_WG[3];

//             TXbuff[21] = 0x27;
//             TXbuff[22] = 0x1B;

//             u16 CRC1 = calc_crc16(0xFFFF,
//                                   &TXbuff[ringbuffer.headPosition + 2],
//                                   21);
//             TXbuff[23] = CRC1 >> 8;
//             TXbuff[24] = CRC1;

//             myUdpSendDtatBroadcast(TXbuff, 25);
//         }
//         /*---------------- 版本查询 ----------------*/
//         else if ((appbudd[8] == 0) &&
//                  (appbudd[9] == 0) &&
//                  (appbudd[10] == 0x36) &&
//                  (appbudd[11] == 0xCB))
//         {
//             TXbuff[7] = banbenlen + 1;
//             TXbuff[8] = 0xA0;

//             for (u16 k = 0; k < banbenlen; k++)
//             {
//                 TXbuff[9 + k] = banbenbuff[k];
//             }

//             u16 CRC1 = calc_crc16(0xFFFF,
//                                   &TXbuff[ringbuffer.headPosition + 2],
//                                   banbenlen + 7);
//             TXbuff[banbenlen + 9] = CRC1 >> 8;
//             TXbuff[banbenlen + 10] = CRC1;

//             myUdpSendDtat(TXbuff, banbenlen + 11);
//         }
//     }
// }

/*==============================================================================
 * RTOS 版协议解析入口
 *============================================================================*/

/**
 * @brief  计算异或校验
 * @param  buf 数据起始地址
 * @param  len 需要异或的字节数
 * @return 异或校验结果
 */
uint8_t xorCheck(uint8_t *buf, uint16_t len)
{
    uint8_t xor = 0;
    for (uint16_t i = 0; i < len; i++)
    {
        xor ^= buf[i];
    }
    return xor;
}

/**
 * @brief  协议数据解析（支持粘包 / 分包）
 * @note   由 Net_HandleRx() 调用
 */
void cmd_check(uint8_t *buf, uint16_t len)
{
    static uint8_t frameBuf[512] = {0};
    static uint16_t frameLen = 0;

    /* 数据拼接 */
    if (frameLen + len > sizeof(frameBuf))
    {
        frameLen = 0;
        return;
    }

    memcpy(&frameBuf[frameLen], buf, len);
    frameLen += len;

    while (frameLen >= 6)
    {
        /* 帧头 */
        if (frameBuf[0] != 0xFF)
        {
            memmove(frameBuf, frameBuf + 1, --frameLen);
            continue;
        }

        uint16_t pktLen = frameBuf[1];

        if (pktLen > frameLen)
            break;

        /* 帧尾 */
        if (frameBuf[pktLen - 1] != 0xFF)
        {
            memmove(frameBuf, frameBuf + 1, --frameLen);
            continue;
        }

        uint8_t recvXor = frameBuf[pktLen - 2];
        uint8_t calcXor = xorCheck(frameBuf, pktLen - 2);

        if (recvXor == calcXor)
        {
            uint8_t cmdNo = getCmdNo(frameBuf[2]);

            if (cmdNo != 255)
            {
                cmd_functions[cmdNo](frameBuf, pktLen);
            }
        }

        memmove(frameBuf, frameBuf + pktLen, frameLen - pktLen);
        frameLen -= pktLen;
    }
}