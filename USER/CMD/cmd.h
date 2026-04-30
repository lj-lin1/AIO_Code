#ifndef CMD_H
#define CMD_H

#include "stdbool.h"
#include "stdint.h"
#include "Motor.h"

#define LAMP PDout(14)
#define HS   PDout(15)

void motor_Response(MoStatus_t status);

#endif