#pragma once

#include "main.h"
#include "cmsis_os2.h"

typedef enum {
    Motor_UP   = UINT32_C(0x01), // Ì§¸Ë
    Motor_DOWN = UINT32_C(0x10), // Âä¸Ë
} MoEvent_t;

typedef enum {
    Motor_Status_UP   = UINT32_C(0x01), // Ì§¸Ë
    Motor_Status_DOWN = UINT32_C(0x02), // Âä¸Ë
} MoStatus_t;

typedef struct
{
    MoStatus_t data; // µç»ú×´Ì¬
} MoData_msg_t;

extern osEventFlagsId_t MotorCtrl_Event;

extern osSemaphoreId_t MotorData_Semaphore;

extern osMessageQueueId_t MotorData_Queue;

uint32_t OS_MessageQueueClear(osMessageQueueId_t queue_id, void *buf, uint32_t buf_size);
uint8_t MOGetStatus(void);