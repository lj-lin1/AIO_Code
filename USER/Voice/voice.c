#include "voice.h"
#include "RS232.h"
#include "cmsis_os2.h"
#include "usart.h"
#include <string.h>

static osMessageQueueId_t voiceQueue;

u8 Frame_Info[512] = {0};

/**
 * @brief  语音模块数据发送
 */
void TxVoce(u8 *inbuf, u16 len)
{
    Frame_Info[0] = 0xFD;
    Frame_Info[1] = 0x00;
    Frame_Info[2] = len + 2;
    Frame_Info[3] = 0x01;
    Frame_Info[4] = 0x01;

    memcpy(&Frame_Info[5], inbuf, len);

    RS232_Send(RS232_PORT_USART6, Frame_Info, 5 + len);
}

void Voice_Task(void *argument)
{
    voiceQueue = osMessageQueueNew(4, sizeof(voice_msg_t), NULL);

    voice_msg_t msg;
    uint8_t vol_cmd[4] = {0x5B, 0x76, 0x30, 0x5D};

    for (;;)
    {
        if (osMessageQueueGet(voiceQueue, &msg, NULL, osWaitForever) == osOK)
        {
            vol_cmd[2] = msg.volume + 0x31;
            TxVoce(vol_cmd, 4);

            osDelay(500);

            TxVoce(msg.data, msg.len);
        }
    }
}

bool Voice_SendRequest(const voice_msg_t *msg)
{
    if (voiceQueue == NULL)
        return false;

    return (osMessageQueuePut(voiceQueue, msg, 0, 0) == osOK);
}
