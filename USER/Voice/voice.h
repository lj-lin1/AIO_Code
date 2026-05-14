#ifndef __VOICE_H__
#define __VOICE_H__

#include <stdint.h>
#include <stdbool.h>

#define VOICE_MAX_LEN 128

void TxVoce(uint8_t *inbuf, uint16_t len);
void Voice_Send(uint8_t *data, uint16_t len);

#endif
