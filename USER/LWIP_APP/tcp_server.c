#include "tcp_server.h"
#include "RingBuffer.h"
#include "cmsis_os2.h"
#include "func.h"
#include "lwip/tcpip.h"
#include "stdlib.h"
#include "string.h"
#include "tcp_priv.h"
#include "w25qxx.h"

extern uint32_t g_ms_tick;
extern osEventFlagsId_t g_netEvent;

#define NET_EVT_RX (1U << 0)
#define NET_EVT_CLOSE (1U << 1)

/* 全局会话（单连接） */
tcp_session_t *g_session = NULL;

/* 接收环形缓冲 */
RingBuffer netRecvBuf = {0};

/* ================= TCP ERR 回调 ================= */

static void tcp_server_err(void *arg, err_t err)
{
    tcp_session_t *s = (tcp_session_t *)arg;

    if (s)
    {
        free(s);
        g_session = NULL;
    }

    osEventFlagsSet(g_netEvent, NET_EVT_CLOSE);
}

/* ================= TCP RECV 回调 ================= */

static err_t tcp_server_recv(void *arg,
                             struct tcp_pcb *pcb,
                             struct pbuf *p,
                             err_t err)
{
    tcp_session_t *s = (tcp_session_t *)arg;

    if (!p)
    {
        tcp_close(pcb);
        tcp_arg(pcb, NULL);

        if (s)
        {
            free(s);
            g_session = NULL;
        }

        osEventFlagsSet(g_netEvent, NET_EVT_CLOSE);
        return ERR_OK;
    }

    s->last_rx_time = g_ms_tick;

    for (struct pbuf *q = p; q; q = q->next)
    {
        RB_PutByte_Bulk(&netRecvBuf,
                        (uint8_t *)q->payload,
                        q->len);
    }

    tcp_recved(pcb, p->tot_len);
    pbuf_free(p);

    osEventFlagsSet(g_netEvent, NET_EVT_RX);
    return ERR_OK;
}

/* ================= TCP ACCEPT ================= */

static err_t tcp_server_sent(void *arg,
                             struct tcp_pcb *pcb,
                             u16_t len)
{
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(pcb);
    LWIP_UNUSED_ARG(len);
    return ERR_OK;
}

static err_t tcp_server_accept(void *arg,
                               struct tcp_pcb *pcb,
                               err_t err)
{
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);

    if (g_session)
    {
        tcp_abort(pcb);
        return ERR_ABRT;
    }

    tcp_session_t *s = malloc(sizeof(tcp_session_t));
    if (!s)
    {
        tcp_abort(pcb);
        return ERR_ABRT;
    }

    memset(s, 0, sizeof(*s));
    s->pcb = pcb;
    s->connected = true;
    s->last_rx_time = g_ms_tick;
    s->last_tx_time = g_ms_tick;
    g_session = s;

    tcp_arg(pcb, s);
    tcp_recv(pcb, tcp_server_recv);
    tcp_sent(pcb, tcp_server_sent);
    tcp_err(pcb, tcp_server_err);

    tcp_setprio(pcb, TCP_PRIO_NORMAL);

    pcb->so_options |= SOF_KEEPALIVE;

    return ERR_OK;
}

/* ================= TCP INIT ================= */

void TCP_Server_Init(void)
{
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb)
        return;

    if (tcp_bind(pcb, IP_ADDR_ANY, TCP_Server_PORT) != ERR_OK)
    {
        tcp_close(pcb);
        return;
    }

    pcb = tcp_listen(pcb);
    tcp_accept(pcb, tcp_server_accept);
}

/* ================= SEND（线程安全） ================= */

typedef struct
{
    struct tcp_pcb *pcb;
    uint8_t *data;
    uint16_t len;
} net_send_ctx_t;

static void net_send_cb(void *arg)
{
    net_send_ctx_t *ctx = (net_send_ctx_t *)arg;

    if (ctx && ctx->pcb)
    {
        if (tcp_write(ctx->pcb,
                      ctx->data,
                      ctx->len,
                      TCP_WRITE_FLAG_COPY) == ERR_OK)
        {
            tcp_output(ctx->pcb);
        }
    }

    if (ctx)
    {
        free(ctx->data);
        free(ctx);
    }
}

bool Net_Send(const uint8_t *buf, uint16_t len)
{
    if (!g_session || !g_session->connected || len == 0)
        return false;

    net_send_ctx_t *ctx = malloc(sizeof(net_send_ctx_t));
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
    ctx->pcb = g_session->pcb;

    if (tcpip_callback(net_send_cb, ctx) != ERR_OK)
    {
        free(ctx->data);
        free(ctx);
        return false;
    }

    g_session->last_tx_time = g_ms_tick;
    return true;
}

u8 F407_IP[4] = {0};
u8 F407_NETMASK[4] = {0};
u8 F407_WG[4] = {0};
u16 F407_PORT = 10028;
u8 F407_MAC[6] = {0};
void PowerOnIpSet(void)
{
    u32 sn0 = 0;
    u8 temoIPFlag[20] = {0};
    u8 IP_CHECK_MSG[6] = "setip";
    sn0 = *(vu32 *)(0x1FFF7A10); // 获取STM32的唯一ID的前24位作为MAC地址后三字节
    // MAC地址设置(高三字节固定为:2.0.0,低三字节用STM32唯一ID)
    F407_MAC[0] = 0xD0; // 高三字节(IEEE称之为组织唯一ID,OUI)地址固定为:2.0.0
    F407_MAC[1] = 0xE0; // 高三位是苹果电脑的MAC地址
    F407_MAC[2] = 0x40;
    F407_MAC[3] = (sn0 >> 16) & 0XFF; // 低三字节用STM32的唯一ID
    F407_MAC[4] = (sn0 >> 8) & 0XFFF;
    F407_MAC[5] = sn0 & 0XFF;

    BSP_W25Qx_ReadDMA(&hw25q64, temoIPFlag, IPADDR, 20);
    if (memcmp(temoIPFlag, IP_CHECK_MSG, 5) == 0)
    {
        F407_IP[0] = temoIPFlag[5];
        F407_IP[1] = temoIPFlag[6];
        F407_IP[2] = temoIPFlag[7];
        F407_IP[3] = temoIPFlag[8];

        F407_NETMASK[0] = temoIPFlag[9];
        F407_NETMASK[1] = temoIPFlag[10];
        F407_NETMASK[2] = temoIPFlag[11];
        F407_NETMASK[3] = temoIPFlag[12];

        F407_WG[0] = temoIPFlag[13];
        F407_WG[1] = temoIPFlag[14];
        F407_WG[2] = temoIPFlag[15];
        F407_WG[3] = temoIPFlag[16];

        F407_PORT = (temoIPFlag[17] << 8) + temoIPFlag[18];
    }
    else
    {
        F407_IP[0] = 192;
        F407_IP[1] = 168;
        F407_IP[2] = 1;
        F407_IP[3] = 220;

        F407_NETMASK[0] = 255;
        F407_NETMASK[1] = 255;
        F407_NETMASK[2] = 255;
        F407_NETMASK[3] = 0;

        F407_WG[0] = 192;
        F407_WG[1] = 168;
        F407_WG[2] = 1;
        F407_WG[3] = 1;

        F407_PORT = 10028;
    }
}