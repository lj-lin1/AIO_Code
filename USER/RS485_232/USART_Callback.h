#ifndef USART_CALLBACK_H
#define USART_CALLBACK_H

#include "stm32f4xx_hal.h"

/* RS232 通知接口 */
void RS232_RxEventFromISR(UART_HandleTypeDef *huart,
                          uint16_t Size);

/* RS485 通知接口 */
void RS485_RxEventFromISR(UART_HandleTypeDef *huart,
                          uint16_t Size);

#endif