#include "iap.h"
#include "RingBuffer.h"
#include "cmsis_os2.h"
#include "config_info.h"
#include "crc.h"
#include "iap_cmd.h"
#include "udp_app.h"

extern RingBuffer udpRecvBuf;
extern osSemaphoreId_t udpRxSem;
extern osMessageQueueId_t udpRxQueue;

/**
 * @brief 将缓冲区中的数据进行协议解析
 *
 */
void UDP_IAP_CmdHandler(udp_peer_t *peer)
{
    uint8_t frame_buf[FRAME_MAX_LEN * 4] = {0};

    while (RB_GetAvailable(&udpRecvBuf) >= (FRAME_MIN_LEN * 4))
    {
        uint32_t available = RB_GetAvailable(&udpRecvBuf);

        /* ---------- 帧头 ---------- */
        uint8_t head_buf[4];
        RB_PeekBlock(&udpRecvBuf, 0, head_buf, 4);

        uint32_t head =
            ((uint32_t)head_buf[0]) |
            ((uint32_t)head_buf[1] << 8) |
            ((uint32_t)head_buf[2] << 16) |
            ((uint32_t)head_buf[3] << 24);

        if (head != FRAME_HEAD)
        {
            RB_SkipBytes(&udpRecvBuf, 1);
            continue;
        }

        /* ---------- 长度 ---------- */
        if (available < (FRAME_LEN_OFFSET + 1) * 4)
            return;

        uint8_t len_buf[4];
        RB_PeekBlock(&udpRecvBuf, FRAME_LEN_OFFSET * 4, len_buf, 4);

        uint32_t payload_len =
            ((uint32_t)len_buf[0]) |
            ((uint32_t)len_buf[1] << 8) |
            ((uint32_t)len_buf[2] << 16) |
            ((uint32_t)len_buf[3] << 24);

        if (payload_len > 256)
        {
            RB_SkipBytes(&udpRecvBuf, 4);
            continue;
        }

        uint32_t frame_len = (payload_len + FRAME_MIN_LEN) * 4;

        /* ---------- 半包 ---------- */
        if (available < frame_len)
        {
            for (uint32_t i = 1; i <= available - 4; i++)
            {
                uint8_t tmp[4];
                RB_PeekBlock(&udpRecvBuf, i, tmp, 4);

                uint32_t next =
                    ((uint32_t)tmp[0]) |
                    ((uint32_t)tmp[1] << 8) |
                    ((uint32_t)tmp[2] << 16) |
                    ((uint32_t)tmp[3] << 24);

                if (next == FRAME_HEAD)
                {
                    RB_SkipBytes(&udpRecvBuf, i);
                    break;
                }
            }
            return;
        }

        /* ---------- 拷贝 ---------- */
        if (frame_len > sizeof(frame_buf))
        {
            RB_SkipBytes(&udpRecvBuf, 4);
            continue;
        }

        RB_PeekBlock(&udpRecvBuf, 0, frame_buf, frame_len);

        iap_frame_t *frame = (iap_frame_t *)frame_buf;

        /* ---------- CRC ---------- */
        uint32_t crc = HAL_CRC_Calculate(&hcrc,
                                         (uint32_t *)frame,
                                         frame->len + 4);

        if (crc != frame->data_crc[frame->len])
        {
            RB_SkipBytes(&udpRecvBuf, 1);
            continue;
        }

        /* ---------- 消费 ---------- */
        RB_SkipBytes(&udpRecvBuf, frame_len);

        /* ---------- CMD 分发 ---------- */
        uint8_t cmd = frame->cmd & 0xFF;

        g_iap_cmd_table[cmd](peer, frame);
    }
}

void Iap_Task(void *argument)
{
    osDelay(1000);

    static udp_peer_t peer = {0};

    for (;;)
    {
        osSemaphoreAcquire(udpRxSem, osWaitForever);
        memset(&peer, 0, sizeof(udp_peer_t));
        osMessageQueueGet(udpRxQueue, &peer, NULL, osWaitForever);
        UDP_IAP_CmdHandler(&peer);
        osDelay(5);
    }
}