#ifndef __RS232_H
#define __RS232_H

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "queue.h"
#include "semphr.h"

/* ================= 配置 ================= */

#define RS232_PORT_NUM   2
#define RS232_RX_DMA_LEN 256

typedef enum {
    RS232_PORT_USART3 = 0,
    RS232_PORT_USART6 = 1,
} RS232_Port_t;

/* ================= 接口 ================= */

void RS232_Init(void);
void RS232_Send(RS232_Port_t port, uint8_t *buf, uint16_t len);
void RS232_Task(void *argument);

#endif
