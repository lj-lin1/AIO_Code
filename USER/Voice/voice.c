#include "voice.h"
#include "RS232.h"
#include "cmsis_os2.h"
#include "usart.h"
#include <string.h>

uint8_t Frame_Info[256] = {0};

/**
 * @brief  语音模块数据发送
 */
void TxVoce(uint8_t *inbuf, uint16_t len)
{
    memset(Frame_Info, 0, sizeof(Frame_Info));

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
    static uint8_t voi_buf[128] = {0};
    memset(voi_buf, 0, sizeof(voi_buf));

    uint8_t vol_cmd[4] = {0x5B, 0x76, 0x30, 0x5D};

    if (data[4] <= 8)
        vol_cmd[2] = data[4] + 0x31;
    else
        vol_cmd[2] = 0x39;

    TxVoce(vol_cmd, 4);

    osDelay(100);
    memcpy(voi_buf, &data[5], len - 7);

    TxVoce(voi_buf, len - 7);
}
