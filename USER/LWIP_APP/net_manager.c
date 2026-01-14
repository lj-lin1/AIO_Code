#include "net_manager.h"
#include "RingBuffer.h"
#include "cmd.h"
#include "cmsis_os2.h"
#include "tcp_server.h"
#include <string.h>

/* ================= 参数 ================= */

#define NET_EVT_RX (1U << 0)
#define NET_EVT_CLOSE (1U << 1)

#define HEARTBEAT_TIMEOUT_MS 15000

extern RingBuffer netRecvBuf;
extern uint32_t g_ms_tick;
extern tcp_session_t *g_session;

osEventFlagsId_t g_netEvent;

/* ================= 状态机 ================= */

typedef enum
{
    NET_STATE_IDLE = 0,
    NET_STATE_CONNECTED,
    NET_STATE_LOST
} net_state_t;

static net_state_t g_netState = NET_STATE_IDLE;

/* ================= 业务处理 ================= */

static void Net_HandleRx(uint8_t *buf, uint16_t len)
{
    /* 心跳包 */
    if (len == 6 && memcmp(buf, "PING\r\n", 6) == 0)
    {
        Net_Send((uint8_t *)"PONG\r\n", 6);
        return;
    }

    cmd_check(buf, len);
    Net_Send(buf, len); // 回显
}

/* ================= 状态机驱动 ================= */

static void Net_StateMachine(uint32_t flags)
{
    /* -------- TCP 断开（唯一入口） -------- */
    if (flags & NET_EVT_CLOSE)
    {
        g_netState = NET_STATE_IDLE;
        return;
    }

    /* -------- TCP 已存在 -------- */
    if (g_session)
    {
        /* 有数据 → 一定恢复 CONNECTED */
        if (flags & NET_EVT_RX)
        {
            g_netState = NET_STATE_CONNECTED;

            uint8_t buf[256] = {0};
            while (!RB_IsEmpty(&netRecvBuf))
            {
                uint16_t len = RB_GetByte_Bulk(&netRecvBuf,
                                               buf,
                                               sizeof(buf));
                Net_HandleRx(buf, len);
            }
        }

        /* 心跳检测（不阻断） */
        if (g_ms_tick - g_session->last_rx_time > HEARTBEAT_TIMEOUT_MS)
        {
            if (g_netState == NET_STATE_CONNECTED)
            {
                g_netState = NET_STATE_LOST;
            }
        }
    }
    else
    {
        g_netState = NET_STATE_IDLE;
    }
}

/* ================= 任务入口 ================= */

void Net_Manager_Task(void *argument)
{
    g_netEvent = osEventFlagsNew(NULL);
    TCP_Server_Init();

    for (;;)
    {
        uint32_t flags = osEventFlagsWait(
            g_netEvent,
            NET_EVT_RX | NET_EVT_CLOSE,
            osFlagsWaitAny,
            100);

        Net_StateMachine(flags);
    }
}
