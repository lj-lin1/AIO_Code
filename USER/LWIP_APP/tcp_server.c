#include "tcp_server.h"
#include "RingBuffer.h"
#include "cmsis_os2.h"
#include "iap_cmd.h"
#include "lwip/tcpip.h"
#include "stm32f4xx.h"
#include "w25qxx.h"
#include <string.h>

#define TCP_KEEPIDLE_DEFAULT 60000  // 60s 无数据开始探测
#define TCP_KEEPINTVL_DEFAULT 10000 // 每10s发一次探测
#define TCP_KEEPCNT_DEFAULT 5       // 最多5次

NET_Parameter_t net_param = {
    .IP = {0},
    .NETMASK = {0},
    .WG = {0},
    .PORT = 10028,
    .MAC = {0},
};

/* 接收缓存 */
RingBuffer netRecvBuf = {0};

extern uint32_t g_ms_tick;
extern osEventFlagsId_t g_netEvent;

extern void NetMgr_OnConnected(void);

/* 事件位 */
#define NET_EVT_RX (1U << 0)
#define NET_EVT_CLOSE (1U << 1)

/* 当前会话 */
tcp_session_t *g_session = NULL;

/* ================= recv 回调 ================= */
static err_t tcp_server_recv(void *arg,
                             struct tcp_pcb *pcb,
                             struct pbuf *p,
                             err_t err)
{
    tcp_session_t *s = (tcp_session_t *)arg;

    /* 对端正常关闭 */
    if (p == NULL)
    {
        tcp_close(pcb);
        free(s);
        g_session = NULL;
        tcp_arg(pcb, NULL);

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

/* ================= err 回调（唯一释放点） ================= */
static void tcp_server_err(void *arg, err_t err)
{
    tcp_session_t *s = (tcp_session_t *)arg;

    if (s)
        free(s);

    g_session = NULL;

    /* 通知 NetManager 回到监听态 */
    osEventFlagsSet(g_netEvent, NET_EVT_CLOSE);
}

/* ================= accept 回调 ================= */
static void tcp_server_kill_session(void)
{
    if (g_session && g_session->pcb)
    {
        /* 解绑回调，防止野指针 */
        tcp_arg(g_session->pcb, NULL);
        tcp_recv(g_session->pcb, NULL);
        tcp_err(g_session->pcb, NULL);

        /* 强制断开 */
        tcp_abort(g_session->pcb);

        free(g_session);
        g_session = NULL;

        osEventFlagsSet(g_netEvent, NET_EVT_CLOSE);
    }
}

static err_t tcp_server_accept(void *arg,
                               struct tcp_pcb *pcb,
                               err_t err)
{
    /* 只允许一个连接 */
    if (g_session)
    {
        tcp_server_kill_session();
        return ERR_ABRT;
    }

    tcp_session_t *s = malloc(sizeof(tcp_session_t));
    if (!s)
        return ERR_MEM;

    memset(s, 0, sizeof(*s));
    s->pcb = pcb;
    s->connected = true;
    s->last_rx_time = g_ms_tick;
    s->last_tx_time = g_ms_tick;

    g_session = s;

    tcp_arg(pcb, s);
    tcp_recv(pcb, tcp_server_recv);
    tcp_err(pcb, tcp_server_err);

    /* 开启KeepAlive */
    pcb->so_options |= SOF_KEEPALIVE;

    pcb->keep_idle = TCP_KEEPIDLE_DEFAULT;   // 60s
    pcb->keep_intvl = TCP_KEEPINTVL_DEFAULT; // 10s
    pcb->keep_cnt = TCP_KEEPCNT_DEFAULT;

    NetMgr_OnConnected();
    return ERR_OK;
}

/* ================= Server Init ================= */
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

/* ================= Net_Send ================= */

typedef struct
{
    struct tcp_pcb *pcb;
    uint8_t *data;
    uint16_t len;
} net_send_ctx_t;

static void net_send_cb(void *arg)
{
    net_send_ctx_t *ctx = (net_send_ctx_t *)arg;

    if (ctx && ctx->pcb &&
        g_session &&
        g_session->pcb == ctx->pcb &&
        g_session->connected)
    {
        tcp_nagle_disable(ctx->pcb);
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
    if (!buf || len == 0)
        return false;

    if (!g_session || !g_session->connected || !g_session->pcb)
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

void PowerOnIpSet(void)
{
    uint32_t sn0 = 0;
    NET_Parameter_t net_param_tmp = {0};
    uint8_t IP_CHECK_MSG[6] = "setip";
    sn0 = *(__IO uint32_t *)(0x1FFF7A10); // 获取STM32的唯一ID的前24位作为MAC地址后三字节
    // MAC地址设置(高三字节固定为:2.0.0,低三字节用STM32唯一ID)
    net_param.MAC[0] = 0xD0; // 高三字节(IEEE称之为组织唯一ID,OUI)地址固定为:2.0.0
    net_param.MAC[1] = 0xE0; // 高三位是苹果电脑的MAC地址
    net_param.MAC[2] = 0x40;
    net_param.MAC[3] = (sn0 >> 16) & 0XFF; // 低三字节用STM32的唯一ID
    net_param.MAC[4] = (sn0 >> 8) & 0XFFF;
    net_param.MAC[5] = sn0 & 0XFF;

    // 更改为flash
    W25Q256_Read(W25QXXIPADDR, (uint8_t *)&net_param_tmp, sizeof(NET_Parameter_t));

    if (memcmp(net_param_tmp.MAGIC, IP_CHECK_MSG, 5) == 0)
    {
        Git_info_To_mainAPP(&net_param_tmp);
        memcpy(&net_param, &net_param_tmp, sizeof(NET_Parameter_t));
    }
    else
    {
        Git_info_To_mainAPP(&net_param);
        memcpy(&net_param.MAGIC, IP_CHECK_MSG, 6);
        W25Q256_WriteAutoErase(W25QXXIPADDR, (uint8_t *)&net_param, sizeof(NET_Parameter_t));
    }
}