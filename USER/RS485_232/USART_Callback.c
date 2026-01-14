#include "USART_Callback.h"

/* ================= 唯一 HAL 回调 ================= */

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart,
                                uint16_t Size)
{
    /* RS232 分发 */
    RS232_RxEventFromISR(huart, Size);

    /* RS485 分发 */
    RS485_RxEventFromISR(huart, Size);
}