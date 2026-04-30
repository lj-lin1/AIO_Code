#ifndef __IOCTRL_H
#define __IOCTRL_H

#include "main.h"
#include "cmsis_os2.h"

enum {
    S1_EVENT = 1,
    S2_EVENT = 2,
    S3_EVENT = 4,
};

extern osEventFlagsId_t S123_Event;

void BSP_HSCtrl(bool HSEnable);
void BSP_LILCtrl(bool LILEnable);

__weak void S1Ctrl(void);
__weak void S2Ctrl(void);
__weak void S3Ctrl(void);
void BSP_S123_Task(void *argument);

#endif