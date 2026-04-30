#include "voice.h"
#include "RS232.h"
#include "cmsis_os2.h"
#include "usart.h"
#include <string.h>

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

void Voice_Send(uint8_t *data, uint16_t len)
{
    voice_msg_t msg = {0};
    uint8_t vol_cmd[4] = {0x5B, 0x76, 0x30, 0x5D};

    vol_cmd[2] = msg.volume + 0x31;
    TxVoce(vol_cmd, 4);

    osDelay(100);
    memcpy(msg.data, data, len);

    TxVoce(msg.data, msg.len);
}
