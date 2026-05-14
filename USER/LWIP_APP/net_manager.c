#include "net_manager.h"
#include "RingBuffer.h"
#include "tcp_server.h"

osEventFlagsId_t g_netEvent;
net_state_t g_netState = NET_STATE_LINK_DOWN;

extern RingBuffer netRecvBuf;
extern osSemaphoreId_t netRxSem;

void NetMgr_OnConnected(void)
{
    g_netState = NET_STATE_CONNECTED;
}

void TCP_Manager_Task(void *argument)
{
    g_netEvent = osEventFlagsNew(NULL);

    for (;;)
    {
        uint32_t flags = osEventFlagsWait(
            g_netEvent,
            NET_EVT_RX | NET_EVT_CLOSE | NET_EVT_LINK_DOWN | NET_EVT_LINK_UP,
            osFlagsWaitAny,
            200);

        /* ---------- 物理链路断开 ---------- */
        if (flags == NET_EVT_LINK_DOWN)
        {
            g_netState = NET_STATE_LINK_DOWN;

            if (g_session && g_session->pcb)
                tcp_close(g_session->pcb);
        }

        /* ---------- 物理链路恢复 ---------- */
        if (flags == NET_EVT_LINK_UP)
        {
            if (g_netState == NET_STATE_LINK_DOWN)
            {
                g_netState = NET_STATE_WAIT_ACCEPT;
                TCP_Server_Init();
                RB_SkipBytes(&netRecvBuf, RB_GetAvailable(&netRecvBuf)); // 清空接收缓冲区
            }
        }

        /* ---------- 连接被动断开 ---------- */
        if (flags == NET_EVT_CLOSE)
        {
            if (g_netState == NET_STATE_CONNECTED)
            {
                RB_SkipBytes(&netRecvBuf, RB_GetAvailable(&netRecvBuf)); // 清空接收缓冲区
                g_netState = NET_STATE_WAIT_ACCEPT;
            }
        }

        /* ---------- 接收数据 ---------- */
        if (flags == NET_EVT_RX)
        {
            if (g_netState == NET_STATE_CONNECTED)
                osSemaphoreRelease(netRxSem);
        }

        osDelay(10);
    }
}