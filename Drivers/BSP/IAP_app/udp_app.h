#pragma once

#include "main.h"

#include "lwip.h"
#include "cmsis_os2.h"
#include "RingBuffer.h"

typedef struct
{
    ip_addr_t addr;
    uint16_t port;
} udp_peer_t;

/* ÊÂ¼þ */
#define NET_EVT_UDP_RX (1U << 4)

#define UDP_PORT       (10011U)

bool UDP_Init(void);
bool UDP_Send(const ip_addr_t *ip, uint16_t port, const uint8_t *buf, uint16_t len);
bool UDP_SendBroadcast(uint16_t port, const uint8_t *buf, uint16_t len);
