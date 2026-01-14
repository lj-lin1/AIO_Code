#ifndef __VOICE_H__
#define __VOICE_H__

#include <stdint.h>
#include <stdbool.h>

#define VOICE_MAX_LEN 256

typedef struct
{
    uint8_t volume;
    uint8_t data[VOICE_MAX_LEN];
    uint16_t len;
} voice_msg_t;

bool Voice_SendRequest(const voice_msg_t *msg);

#endif
