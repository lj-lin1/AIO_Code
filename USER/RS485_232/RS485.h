#ifndef __RS485_H
#define __RS485_H

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "queue.h"
#include "semphr.h"

/* ================= 用户配置 ================= */

#define RS485_UART       huart1
#define RS485_RX_DMA_LEN 256

/* RS485 DE 引脚（按你的硬件修改） */
#define RS485_DE_GPIO_Port GPIOA
#define RS485_DE_Pin       GPIO_PIN_8

/* ================= 接口 ================= */

void RS485_Init(void);
void RS485_Send(uint8_t *buf, uint16_t len);

/* FreeRTOS 接收任务 */
void RS485_Task(void *argument);

#endif
