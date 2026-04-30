#include "udp_app.h"
#include "RingBuffer.h"
#include "cmsis_os2.h"
#include "lwip/ip_addr.h"
#include "lwip/tcpip.h"
#include "lwip/udp.h"
#include <stdlib.h>
#include <string.h>

/* ================= 变量 ================= */
osEventFlagsId_t iap_netEvent;
osSemaphoreId_t udpRxSem;
osMessageQueueId_t udpRxQueue;
RingBuffer udpRecvBuf = {0};

/* UDP PCB */
static struct udp_pcb *g_udp_pcb = NULL;

/* 发送上下文 */
typedef struct
{
    ip_addr_t addr;
    uint16_t port;
    uint8_t *data;
    uint16_t len;
} udp_send_ctx_t;

/* ================= 接收回调 ================= */
static void udp_recv_cb(void *arg,
                        struct udp_pcb *pcb,
                        struct pbuf *p,
                        const ip_addr_t *addr,
                        u16_t port)
{
    if (!p)
        return;

    /* 写入环形缓冲区 */
    for (struct pbuf *q = p; q; q = q->next)
    {
        RB_PutByte_Bulk(&udpRecvBuf,
                        (uint8_t *)q->payload,
                        q->len);
    }

    pbuf_free(p);

    /* 通知上层 */
    osEventFlagsSet(iap_netEvent, NET_EVT_UDP_RX);
    udp_peer_t peer = {*addr, port};
    osMessageQueuePut(udpRxQueue, &peer, 0, 0);
}

/* ================= 初始化 ================= */
bool UDP_Init(void)
{
    g_udp_pcb = udp_new();
    if (!g_udp_pcb)
        return false;

    if (udp_bind(g_udp_pcb, IP_ADDR_ANY, UDP_PORT) != ERR_OK)
    {
        udp_remove(g_udp_pcb);
        g_udp_pcb = NULL;
        return false;
    }

    udp_recv(g_udp_pcb, udp_recv_cb, NULL);

    return true;
}

/* ================= 发送 ================= */
static void udp_send_cb(void *arg)
{
    udp_send_ctx_t *ctx = (udp_send_ctx_t *)arg;

    if (!ctx || !g_udp_pcb)
    {
        if (ctx)
        {
            free(ctx->data);
            free(ctx);
        }
    }

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, ctx->len, PBUF_RAM);
    if (!p)
    {
        if (ctx)
        {
            free(ctx->data);
            free(ctx);
        }
    }

    memcpy(p->payload, ctx->data, ctx->len);

    udp_sendto(g_udp_pcb, p, &ctx->addr, ctx->port);

    pbuf_free(p);
}

bool UDP_Send(const ip_addr_t *ip, uint16_t port,
              const uint8_t *buf, uint16_t len)
{
    if (!ip || !buf || len == 0 || !g_udp_pcb)
        return false;

    udp_send_ctx_t *ctx = malloc(sizeof(udp_send_ctx_t));
    if (!ctx)
        return false;

    ctx->data = malloc(len);
    if (!ctx->data)
    {
        free(ctx);
        return false;
    }

    memcpy(ctx->data, buf, len);
    ctx->len = len;

    /* 直接拷贝 ip_addr_t */
    ip_addr_copy(ctx->addr, *ip);

    ctx->port = port;

    if (tcpip_callback(udp_send_cb, ctx) != ERR_OK)
    {
        free(ctx->data);
        free(ctx);
        return false;
    }

    return true;
}

bool UDP_SendBroadcast(uint16_t port,
                       const uint8_t *buf, uint16_t len)
{
    if (!g_udp_pcb)
        return false;

    ip_set_option(g_udp_pcb, SOF_BROADCAST);

    ip_addr_t broadcast_ip;
    ip_addr_set_ip4_u32(&broadcast_ip, PP_HTONL(0xFFFFFFFF));

    return UDP_Send(&broadcast_ip, port, buf, len);
}