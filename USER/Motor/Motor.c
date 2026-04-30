/*
 * @note Motor.c 栏杆控制部分
 * @author 适配：万胜栏杆控制器（蓝色）
 */

#include "Motor.h"
#include "RS485.h"
#include "cmd.h"
#include "cmsis_os2.h"

osEventFlagsId_t MotorCtrl_Event;

osSemaphoreId_t MotorData_Semaphore;

osMessageQueueId_t MotorData_Queue;

typedef enum
{
    MoStatus_init, // 初始化
    MoStatus_same, // 状态对齐
    MoStatus_diff, // 状态不对齐
} MoCtrl_t;

static const uint8_t ctrl_up[8] = {0x01, 0x06, 0x00, 0xB5, 0x00, 0x01, 0x59, 0xEC};
static const uint8_t ctrl_down[8] = {0x01, 0x06, 0x00, 0xB5, 0x00, 0x02, 0x19, 0xED};

static MoCtrl_t GetStatus(uint32_t flags)
{
    uint8_t sendbuf[8] = {0x01, 0x03, 0xD0, 0X1A, 0X00, 0X02, 0XDD, 0X0C}; // 读取状态指令
    RS485_Send(sendbuf, sizeof(sendbuf));

    MoData_msg_t msg = {0};

    if (osMessageQueueGet(MotorData_Queue, &msg, NULL, 1000) == osOK)
    {
        if (flags == Motor_UP && msg.data == Motor_Status_UP)
            return MoStatus_same;
        else if (flags == Motor_DOWN && msg.data == Motor_Status_DOWN)
            return MoStatus_same;
        else
            return MoStatus_diff;
    }

    return MoStatus_init;
}

/*
 * @brief 获取栏杆机状态
 * @return 状态(落杆:0x00, 抬杆:0x01, 未知:0xff)
 * @note 用于网络层协议调用
 */
uint8_t MOGetStatus(void)
{
    MoCtrl_t cache = GetStatus(Motor_UP);
    if (cache == MoStatus_same)
        return true;
    else if (cache == MoStatus_diff)
        return false;
    else
        return 0xff;
}

static MoCtrl_t SetStatus(uint32_t flags)
{
    uint8_t sendbuf[32] = {0};
    if (flags == Motor_UP)
        memcpy(sendbuf, ctrl_up, sizeof(ctrl_up));
    else if (flags == Motor_DOWN)
        memcpy(sendbuf, ctrl_down, sizeof(ctrl_down));

    RS485_Send(sendbuf, 8);
    return MoStatus_init;
}

static void Comm_Response(uint32_t flags)
{
    if (flags == Motor_UP)
        motor_Response(Motor_Status_UP);
    else if (flags == Motor_DOWN)
        motor_Response(Motor_Status_DOWN);
}

/**
 * @brief 清空消息队列（逐个读取直到为空）
 * @param queue_id 队列句柄
 * @param buf      临时缓存（大小 >= 队列消息大小）
 * @param buf_size buf大小（用于安全检查，可选）
 * @return 实际清空的消息个数
 */
uint32_t OS_MessageQueueClear(osMessageQueueId_t queue_id, void *buf, uint32_t buf_size)
{
    if (queue_id == NULL || buf == NULL)
        return 0;

    uint32_t count = 0;

    /* 获取消息大小（用于防御性检查） */
    uint32_t msg_size = osMessageQueueGetMsgSize(queue_id);
    if (buf_size < msg_size)
        return 0;

    /* 非阻塞读取直到队列为空 */
    while (osMessageQueueGet(queue_id, buf, NULL, 0) == osOK)
    {
        count++;
    }

    return count;
}

void MotorCtrl_Task(void *argument)
{
    MotorCtrl_Event = osEventFlagsNew(NULL);
    MoCtrl_t status = MoStatus_init;
    uint8_t fixnum = 0; // 修正次数
    for (;;)
    {
        uint32_t flags = osEventFlagsWait(
            MotorCtrl_Event,
            Motor_UP | Motor_DOWN,
            osFlagsWaitAny,
            osWaitForever);

        status = MoStatus_init;
        for (;;)
        {
            osDelay(200);

            if (fixnum > 10)
            {
                fixnum = 0;
                break;
            }

            if (status == MoStatus_init)
            {
                status = GetStatus(flags);
                continue;
            }

            if (status == MoStatus_diff)
            {
                fixnum++;
                status = SetStatus(flags);
                continue;
            }

            if (status == MoStatus_same)
            {
                Comm_Response(flags);
                break;
            }
        }
    }
}

uint16_t Modbus_CRC16(const uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF;

    for (uint16_t i = 0; i < len; i++)
    {
        crc ^= data[i]; // 与当前字节异或

        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001; // 多项式 0xA001
            else
                crc >>= 1;
        }
    }

    return crc; // 注意：返回值（高字节在前的表示）
}

static void motor_getstatus(uint8_t *data)
{
    MoData_msg_t msg = {0};
    if (*data == 0x0C)
    {
        msg.data = Motor_Status_UP;
        osMessageQueuePut(MotorData_Queue, &msg, 0, osWaitForever);
    }
    else if (*data == 0x0A)
    {
        msg.data = Motor_Status_DOWN;
        osMessageQueuePut(MotorData_Queue, &msg, 0, osWaitForever);
    }
}

static void MotorData_Handle(void)
{
    while (RB_GetAvailable(&RS485_BUFFER) >= 8)
    {
        osDelay(1);
        uint8_t addr = 0;

        /* 查找帧头 0xFF */
        if (!RB_PeekByte(&RS485_BUFFER, 0, &addr))
            break;

        if (addr != 0x01)
        {
            /* 丢弃一个字节继续找 */
            RB_SkipBytes(&RS485_BUFFER, 1);
            continue;
        }

        /* 读取命令字段 */
        uint8_t cmd = 0;
        if (!RB_PeekByte(&RS485_BUFFER, 1, &cmd))
            break;

        uint8_t pktLen = 0;

        if (cmd == 0x03)
            pktLen = 9;
        else if (cmd == 0x06)
            pktLen = 8;
        else
        {
            /* 丢弃一个字节继续找 */
            RB_SkipBytes(&RS485_BUFFER, 1);
            continue;
        }

        /* 取出完整帧*/
        uint8_t frame[256] = {0};
        RB_PeekBlock(&RS485_BUFFER, 0, frame, pktLen);

        /* 校验 */
        uint16_t crc = Modbus_CRC16(frame, pktLen - 2);
        uint16_t recvXor = (frame[pktLen - 1] << 8) | frame[pktLen - 2];
        if (recvXor != crc)
        {
            RB_SkipBytes(&RS485_BUFFER, 1);
            continue;
        }

        switch (cmd)
        {
        case 0x03: {
            motor_getstatus(&frame[3]);
        }
        break;
        case 0x06:
            break;
        default:
            break;
        }

        /* 消费该帧 */
        RB_SkipBytes(&RS485_BUFFER, pktLen);
    }
}

void MotorData_Task(void *argument)
{
    MotorData_Semaphore = osSemaphoreNew(1, 0, NULL);
    MotorData_Queue = osMessageQueueNew(10, sizeof(MoData_msg_t), NULL);

    for (;;)
    {
        osSemaphoreAcquire(MotorData_Semaphore, osWaitForever);
        // 处理数据
        MotorData_Handle();

        osDelay(5);
    }
}