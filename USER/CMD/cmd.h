#ifndef CMD_H
#define CMD_H

#include "stdbool.h"
#include "stdint.h"

#define LAMP PDout(14)
#define HS   PDout(15)

void cmd_check(uint8_t *buf, uint16_t len);

#endif