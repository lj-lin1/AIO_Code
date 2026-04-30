#include "udp_manager.h"
#include "udp_app.h"

extern osEventFlagsId_t iap_netEvent;
extern osSemaphoreId_t udpRxSem;
extern osMessageQueueId_t udpRxQueue;

void UDP_Manager_Task(void *argument)
{
    iap_netEvent = osEventFlagsNew(NULL);
    udpRxSem = osSemaphoreNew(1, 0, NULL);
    udpRxQueue = osMessageQueueNew(10, sizeof(udp_peer_t), NULL);

    for (;;)
    {
        uint32_t flags = osEventFlagsWait(
            iap_netEvent,
            NET_EVT_UDP_RX,
            osFlagsWaitAny,
            200);

        if (flags & NET_EVT_UDP_RX)
        {
            osSemaphoreRelease(udpRxSem);
        }

        osDelay(10);
    }
}