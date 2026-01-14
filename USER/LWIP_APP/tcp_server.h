#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "lwip/tcp.h"
#include <stdbool.h>
#include <stdint.h>

/*********************开发板ip及MAC定义*****************************/
extern u8 F407_IP[4];
extern u8 F407_NETMASK[4];
extern u8 F407_WG[4];
extern u16 F407_PORT;
extern u8 F407_MAC[6];
#define IMT407G_IP       F407_IP[0], F407_IP[1], F407_IP[2], F407_IP[3]                               // 开发板ip
#define IMT407G_NETMASK  F407_NETMASK[0], F407_NETMASK[1], F407_NETMASK[2], F407_NETMASK[3]           // 开发板子网掩码
#define IMT407G_WG       F407_WG[0], F407_WG[1], F407_WG[2], F407_WG[3]                               // 开发板子网关
#define IMT407G_MAC_ADDR F407_MAC[0], F407_MAC[1], F407_MAC[2], F407_MAC[3], F407_MAC[4], F407_MAC[5] // 开发板MAC地址
#define TCP_Server_PORT  F407_PORT
typedef __IO uint32_t vu32;

typedef struct
{
    struct tcp_pcb *pcb;
    uint32_t last_rx_time;
    uint32_t last_tx_time;
    bool connected;
} tcp_session_t;

/* TCP Server */
void TCP_Server_Init(void);

/* 业务层安全发送接口 */
bool Net_Send(const uint8_t *buf, uint16_t len);

/* 当前会话 */
extern tcp_session_t *g_session;

/*上电ip设置*/
void PowerOnIpSet(void);

#endif
