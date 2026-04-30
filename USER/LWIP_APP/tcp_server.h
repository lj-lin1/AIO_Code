#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "lwip/tcp.h"
#include <stdbool.h>
#include <stdint.h>

/*********************开发板ip及MAC定义*****************************/
typedef struct __attribute__((packed)) {
    uint8_t MAGIC[6];
    uint8_t IP[4];
    uint8_t NETMASK[4];
    uint8_t WG[4];
    uint16_t PORT;
    uint8_t MAC[6];
} NET_Parameter_t;

extern NET_Parameter_t net_param;

#define IMT407G_IP       net_param.IP[0], net_param.IP[1], net_param.IP[2], net_param.IP[3]                       // 开发板ip
#define IMT407G_NETMASK  net_param.NETMASK[0], net_param.NETMASK[1], net_param.NETMASK[2], net_param.NETMASK[3]   // 开发板子网掩码
#define IMT407G_WG       net_param.WG[0], net_param.WG[1], net_param.WG[2], net_param.WG[3]                       // 开发板子网关
#define IMT407G_MAC_ADDR net_param.MAC[0], net_param.MAC[1], net_param.MAC[3], net_param.MAC[4], net_param.MAC[5] // 开发板MAC地址
#define TCP_Server_PORT  net_param.PORT

typedef struct
{
    struct tcp_pcb *pcb;
    uint32_t last_rx_time;
    uint32_t last_tx_time;
    bool connected;
} tcp_session_t;

/* TCP Server 接口 */
void TCP_Server_Init(void);
bool Net_Send(const uint8_t *buf, uint16_t len);

/* 当前会话（只读） */
extern tcp_session_t *g_session;

void PowerOnIpSet(void);

#endif