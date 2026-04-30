/**
 ******************************************************************************
 * @file    app_cmd.c
 * @brief   协议命令处理与设备控制
 * @brief   协议:创迪一体机协议
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
#include "Motor.h"
#include "RS232.h"
#include "RingBuffer.h"
#include "cmsis_os2.h"
#include "display.h"
#include "flash.h"
#include "iap_cmd.h"
#include "render.h"
#include "string.h"
#include "tcp_server.h"
#include "voice.h"
#include "w25qxx.h"

extern volatile uint8_t light_level;      // 亮度等级
const char product_info[] = {"V1.0.0.0"}; // 产品信息

extern RingBuffer netRecvBuf; // 网络接收缓冲区

osSemaphoreId_t netRxSem;

/*==============================================================================
 * 通用命令应答
 *============================================================================*/

/**
 * @brief  根据当前通信端口返回应答数据
 */
void cmdnack(uint8_t *inbuf, uint16_t len)
{
    // 缺少网络返回接口，例如：Net_Send(inbuf, len);
    Net_Send(inbuf, len);
}

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

/*==============================================================================
 * 显示与清屏控制
 *============================================================================*/

/**
 * @brief  显示控制（全屏 / 单行）
 */
void cmd_disPlayAll_ctrl(uint8_t *inbuf, uint16_t len)
{
    u8 ZTColor = 1;
    if (inbuf[6] <= 2) // 判断字体颜色
    {
        ZTColor = inbuf[6] + 1;
    }
    else
        ZTColor = red;
    fontColor = ZTColor;

    if (inbuf[5] < 3)
    {
        fontSize = inbuf[5] + 1; // 判断字体大小(加一原因是本设备不用14号字体)
    }
    else
        fontSize = font_16;

    uint32_t row_offset = 0;
    switch (inbuf[5] + 1)
    {
    case font_16:
        row_offset = font16;
        break;
    case font_20:
        row_offset = font20;
        break;
    case font_24:
        row_offset = font24;
        break;
    default:
        row_offset = font16;
        break;
    }

    if (inbuf[4] == 0x00) // 全屏
    {
        RenderString(0, 0, &inbuf[7], len - 9, (DispColor_t)fontColor, (FontSize_t)fontSize, (FontType_t)fontType, true);
    }
    else if (inbuf[4] <= 17) // 单行
    {
        RenderString(0, (inbuf[4] - 1) * row_offset, &inbuf[7], len - 9, (DispColor_t)fontColor, (FontSize_t)fontSize, (FontType_t)fontType, false);
    }
    cmdnack(inbuf, len);
}

/**
 * @brief  清屏控制
 */
void cmd_clear_ctrl(uint8_t *inbuf, uint16_t len)
{
    if (inbuf[4] == 0x00) // 全屏
    {
        Disp_Fill(black, 0);
    }
    else if (inbuf[4] <= 17) // 单行
    {
        switch (fontSize)
        {
        case font_16:
            Disp_Fill(black, font16 * (inbuf[4] - 1) * SCREEN_PIXEL_ROW);
            break;
        case font_20:
            Disp_Fill(black, font20 * (inbuf[4] - 1) * SCREEN_PIXEL_ROW);
            break;
        case font_24:
            Disp_Fill(black, font24 * (inbuf[4] - 1) * SCREEN_PIXEL_ROW);
            break;
        default:
            Disp_Fill(black, 0);
            break;
        }
    }
    cmdnack(inbuf, len);
}

/*==============================================================================
 * 亮度控制
 *============================================================================*/

/**
 * @brief  亮度设置（1~8 级）
 */
void cmd_setLight_ctrl(uint8_t *inbuf, uint16_t len)
{
    if ((inbuf[4] > 0) && (inbuf[4] <= 8))
    {
        light_level = inbuf[4];
    }

    cmdnack(inbuf, len);

    uint8_t LIGHT_CHECK_MSG[64] = {0};
    snprintf((char *)LIGHT_CHECK_MSG, sizeof(LIGHT_CHECK_MSG), "%s%d", "setlight", light_level);
    // BSP_W25Qx_EraseWrite(&hw25q256, LIGHT_CHECK_MSG, LIGHTADDR, 64); // 保存亮度等级到 Flash
}

/*==============================================================================
 * 通行灯 / 黄闪控制
 *============================================================================*/

/**
 * @brief  通行灯控制
 */
void cmd_lamp_ctrl(uint8_t *inbuf, uint16_t len)
{
    if (inbuf[4] == 0)
    {
        LAMP = 0;
    }
    else if (inbuf[4] == 1)
    {
        LAMP = 1;
    }

    cmdnack(inbuf, len);
}

/**
 * @brief  黄闪控制
 */
void cmd_hs_ctrl(uint8_t *inbuf, uint16_t len)
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
void cmd_setfullscreen_ctrl(uint8_t *inbuf, uint16_t len)
{
    if (inbuf[4] == 0x00)
    {
        // 全屏红
        Disp_Fill(red, 0);
    }
    else if (inbuf[4] == 0x01)
    {
        // 全屏绿
        Disp_Fill(green, 0);
    }
    else if (inbuf[4] == 0x02)
    {
        // 全屏黄
        Disp_Fill(yellow, 0);
    }

    cmdnack(inbuf, len);
}

/*==============================================================================
 * 字体设置
 *============================================================================*/

/**
 * @brief  保存字体大小与类型
 */
void cmd_setfontsize_ctrl(uint8_t *inbuf, uint16_t len)
{
    uint8_t tempFontSize = fontSize;
    uint8_t tempFontType = fontType;

    uint8_t FONT_CHECK_MSG[64] = {0};
    snprintf((char *)FONT_CHECK_MSG, sizeof(FONT_CHECK_MSG), "%s%d%d", "setfontsize", tempFontSize, tempFontType);

    // BSP_W25Qx_EraseWrite(&hw25q256, FONT_CHECK_MSG, FONTSIZEADDR, sizeof(FONT_CHECK_MSG));

    cmdnack(inbuf, len);
}

/*==============================================================================
 * IP 设置
 *============================================================================*/

/**
 * @brief  设置 IP / 掩码 / 网关 / 端口并重启
 */
void cmd_Setip_ctrl(uint8_t *inbuf, uint16_t len)
{
    cmdnack(inbuf, len);

    u8 IP_CHECK_MSG[6] = "setip";

    net_param.IP[0] = inbuf[4];
    net_param.IP[1] = inbuf[5];
    net_param.IP[2] = inbuf[6];
    net_param.IP[3] = inbuf[7];

    net_param.NETMASK[0] = inbuf[8];
    net_param.NETMASK[1] = inbuf[9];
    net_param.NETMASK[2] = inbuf[10];
    net_param.NETMASK[3] = inbuf[11];

    net_param.WG[0] = inbuf[12];
    net_param.WG[1] = inbuf[13];
    net_param.WG[2] = inbuf[14];
    net_param.WG[3] = inbuf[15];

    net_param.PORT = (inbuf[16] << 8) + inbuf[17];

    Git_mianAPP_To_info(&net_param);

    uint8_t f_buf[32] = {0};
    W25Q256_Read(W25QXXIPADDR, f_buf, 32);
    memcpy(f_buf, IP_CHECK_MSG, 6);
    memcpy(f_buf + 6, net_param.IP, 4);
    memcpy(f_buf + 10, net_param.NETMASK, 4);
    memcpy(f_buf + 14, net_param.WG, 4);
    memcpy(f_buf + 18, &inbuf[17], 1); // 大端序
    memcpy(f_buf + 19, &inbuf[16], 1);

    W25Q256_WriteAutoErase(W25QXXIPADDR, f_buf, 32);

    osDelay(500);
    NVIC_SystemReset();
}

/*==============================================================================
 * 语音播报
 *============================================================================*/

/**
 * @brief  语音播报控制
 */
void cmd_YY_ctrl(uint8_t *inbuf, uint16_t len)
{
    voice_msg_t msg;

    msg.volume = (inbuf[4] <= 8) ? inbuf[4] : 8;
    msg.len = len - 7;

    if (msg.len > VOICE_MAX_LEN)
        msg.len = VOICE_MAX_LEN;

    memcpy(msg.data, &inbuf[5], msg.len);

    Voice_Send(msg.data, msg.len);

    cmdnack(inbuf, len);
}

/*==============================================================================
 * CRC 校验
 *============================================================================*/

#define CRC_POLYNOM 0x8408
#define CRC_INIVAL 0xFFFF

uint16_t calc_crc16(uint16_t init_crc, uint8_t *crc_data, int len)
{
    uint16_t crc = init_crc;

    for (int cnt = 0; cnt < len; cnt++)
    {
        crc ^= crc_data[cnt];
        for (uint8_t k = 0; k < 8; k++)
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
uint8_t getCmdNo(uint8_t cmdstr)
{
    uint8_t tempCmd;

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

    case 0x07: /* 栏杆控制 */
        tempCmd = 11;
        break;

    case 0x08: /* 设备复位 */
        tempCmd = 11;
        break;

    case 0x10: /* 获取栏杆状态 */
        tempCmd = 12;
        break;

    default:
        tempCmd = 255;
        break;
    }

    return tempCmd;
}

/**
 * @brief  获取设备版本号
 */
void cmd_getedition_ctrl(uint8_t *inbuf, uint16_t len)
{
    uint8_t rebuff[128] = {0};
    rebuff[0] = 0xFF;
    rebuff[1] = 6 + strlen(product_info);
    rebuff[2] = 0x04;
    rebuff[3] = 0x00;
    memcpy(&rebuff[4], product_info, strlen(product_info));
    rebuff[4 + strlen(product_info)] = xorCheck(rebuff, 4 + strlen(product_info));
    rebuff[4 + strlen(product_info) + 1] = 0xFF;

    // 网络发送
    cmdnack(rebuff, 4 + strlen(product_info) + 2);
}

/**
 * @brief  设备复位
 */
void cmd_reset_ctrl(uint8_t *inbuf, uint16_t len)
{
    NVIC_SystemReset();
}

/**
 * @brief  栏杆控制
 */
void cmd_motor_ctrl(uint8_t *inbuf, uint16_t len)
{
    MoData_msg_t msg = {0};
    OS_MessageQueueClear(MotorData_Queue, &msg, sizeof(MoData_msg_t));
    if (inbuf[4] == 0x00) // 落杆
        osEventFlagsSet(MotorCtrl_Event, Motor_DOWN);
    else if (inbuf[4] == 0x01) // 抬杆
        osEventFlagsSet(MotorCtrl_Event, Motor_UP);
}

static const uint8_t cmd_motor_up[7] = {0xff, 0x07, 0x08, 0x00, 0x01, 0xf1, 0xff};
static const uint8_t cmd_motor_down[7] = {0xff, 0x07, 0x08, 0x00, 0x00, 0xf0, 0xff};

void motor_Response(MoStatus_t status)
{
    uint8_t rebuff[32] = {0};
    if (status == Motor_Status_UP)
        memcpy(rebuff, cmd_motor_up, 7);
    else if (status == Motor_Status_DOWN)
        memcpy(rebuff, cmd_motor_down, 7);

    cmdnack(rebuff, 7);
}

static const uint8_t get_re_up[7] = {0xff, 0x07, 0x10, 0x00, 0x01, 0xe9, 0xff};
static const uint8_t get_re_down[7] = {0xff, 0x07, 0x10, 0x00, 0x00, 0xe8, 0xff};

/*
 * @brief  获取栏杆状态
 */
void cmd_motor_get_status(uint8_t *inbuf, uint16_t len)
{
    MoData_msg_t msg = {0};
    OS_MessageQueueClear(MotorData_Queue, &msg, sizeof(MoStatus_t));
    uint8_t rebuff[32] = {0};
    uint8_t status = MOGetStatus();
    if (status == 0x01)
        memcpy(rebuff, get_re_up, 7);
    else if (status == 0x00)
        memcpy(rebuff, get_re_down, 7);

    cmdnack(rebuff, 7);
}

/*==============================================================================
 * 命令函数表
 *============================================================================*/

/**
 * @brief  命令处理函数指针表
 *         索引值由 getCmdNo() 返回
 */
void (*cmd_functions[24])(uint8_t *inbuf, uint16_t len) =
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
        cmd_reset_ctrl,         /* 10 软件复位 */
        cmd_motor_ctrl,         /* 11 栏杆控制 */
        cmd_motor_get_status,   /* 12 获取栏杆状态 */
};

/*==============================================================================
 * RTOS 版协议解析入口
 *============================================================================*/

/**
 * @brief  协议数据解析（支持粘包 / 分包）
 */
void Net_CmdHandler(void)
{
    uint16_t available = 0;

    /* 循环处理所有可解析数据 */
    while (1)
    {
        osDelay(1);
        available = RB_GetAvailable(&netRecvBuf);

        /* 至少需要最小帧长度 */
        if (available < 6)
            break;

        uint8_t head;

        /* 查找帧头 0xFF */
        if (!RB_PeekByte(&netRecvBuf, 0, &head))
            break;

        if (head != 0xFF)
        {
            /* 丢弃一个字节继续找 */
            RB_SkipBytes(&netRecvBuf, 1);
            continue;
        }

        /* 读取长度字段 */
        uint8_t pktLen;
        if (!RB_PeekByte(&netRecvBuf, 1, &pktLen))
            break;

        /* 长度合法性保护 */
        if (pktLen < 6 || pktLen > 512)
        {
            /* 非法帧，丢弃帧头 */
            RB_SkipBytes(&netRecvBuf, 1);
            continue;
        }

        /* 数据还不完整 */
        if (available < pktLen)
            break;

        /* 检查帧尾 */
        uint8_t tail;
        RB_PeekByte(&netRecvBuf, pktLen - 1, &tail);

        if (tail != 0xFF)
        {
            /* 帧错误，丢弃帧头 */
            RB_SkipBytes(&netRecvBuf, 1);
            continue;
        }

        /* 取出完整帧*/
        uint8_t frame[512] = {0};
        RB_PeekBlock(&netRecvBuf, 0, frame, pktLen);

        /* 校验 */
        uint8_t recvXor = frame[pktLen - 2];
        uint8_t calcXor = xorCheck(frame, pktLen - 2);

        if (recvXor == calcXor)
        {
            uint8_t cmdNo = getCmdNo(frame[2]);

            if (cmdNo != 255)
            {
                cmd_functions[cmdNo](frame, pktLen);
            }
        }

        /* 消费该帧 */
        RB_SkipBytes(&netRecvBuf, pktLen);
    }
}

void TcpParse_Task(void *argument)
{
    osDelay(1000);
    netRxSem = osSemaphoreNew(1, 1, NULL);

    for (;;)
    {
        // 等待新数据
        osSemaphoreAcquire(netRxSem, osWaitForever);

        // 尽可能多解析
        Net_CmdHandler();
        osDelay(5);
    }
}