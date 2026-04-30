#ifndef NET_MANAGER_H
#define NET_MANAGER_H

#include "cmsis_os2.h"

/* 事件位 */
#define NET_EVT_RX        (1U << 0)
#define NET_EVT_CLOSE     (1U << 1)
#define NET_EVT_LINK_UP   (1U << 2)
#define NET_EVT_LINK_DOWN (1U << 3)

typedef enum {
    NET_STATE_LINK_DOWN = 0,
    NET_STATE_WAIT_ACCEPT,
    NET_STATE_CONNECTED,
} net_state_t;

extern osEventFlagsId_t g_netEvent;
extern net_state_t g_netState;

void TCP_Manager_Task(void *argument);

/* TCP_Server 回调 */
void NetMgr_OnConnected(void);

#endif