#include "rs232.h"
#include "cmd.h"
#include <string.h>


/* ================= 外部 UART 句柄 ================= */

extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;

/* ================= 端口描述结构 ================= */

typedef struct
{
    UART_HandleTypeDef *huart;
    uint8_t rx_dma_buf[RS232_RX_DMA_LEN];
    uint8_t frame_buf[RS232_RX_DMA_LEN];
    QueueHandle_t rx_queue;
    SemaphoreHandle_t tx_sem;
} RS232_Handle_t;

/* ================= 所有 RS232 端口 ================= */

static RS232_Handle_t rs232_ports[RS232_PORT_NUM] =
    {
        [RS232_PORT_USART3] = {.huart = &huart3},
        [RS232_PORT_USART6] = {.huart = &huart6},
};

/* ================= 初始化 ================= */

void RS232_Init(void)
{
    for (int i = 0; i < RS232_PORT_NUM; i++)
    {
        rs232_ports[i].rx_queue = xQueueCreate(4, sizeof(uint16_t));
        rs232_ports[i].tx_sem = xSemaphoreCreateBinary();
        xSemaphoreGive(rs232_ports[i].tx_sem);

        HAL_UARTEx_ReceiveToIdle_DMA(rs232_ports[i].huart,
                                     rs232_ports[i].rx_dma_buf,
                                     RS232_RX_DMA_LEN);

        __HAL_DMA_DISABLE_IT(rs232_ports[i].huart->hdmarx, DMA_IT_HT);
    }
}

void RS232_RxEventFromISR(UART_HandleTypeDef *huart,
                          uint16_t Size)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    for (int i = 0; i < RS232_PORT_NUM; i++)
    {
        if (huart == rs232_ports[i].huart)
        {
            xQueueSendFromISR(rs232_ports[i].rx_queue,
                              &Size,
                              &xHigherPriorityTaskWoken);
            break;
        }
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void RS232_Task(void *argument)
{
    uint16_t len = 0;

    for (;;)
    {
        for (int i = 0; i < RS232_PORT_NUM; i++)
        {
            if (xQueueReceive(rs232_ports[i].rx_queue,
                              &len,
                              0) == pdPASS)
            {
                memcpy(rs232_ports[i].frame_buf,
                       rs232_ports[i].rx_dma_buf,
                       len);

                HAL_UARTEx_ReceiveToIdle_DMA(rs232_ports[i].huart,
                                             rs232_ports[i].rx_dma_buf,
                                             RS232_RX_DMA_LEN);

                __HAL_DMA_DISABLE_IT(rs232_ports[i].huart->hdmarx,
                                     DMA_IT_HT);

                /* ===== 在这里区分端口处理协议 ===== */
                if (i == RS232_PORT_USART3)
                {
                    /* USART3 协议 */
                    RS232_Send(RS232_PORT_USART3, rs232_ports[i].frame_buf, len);
                    cmd_check(rs232_ports[i].frame_buf, len);
                    memset(rs232_ports[i].frame_buf, 0, len);
                }
                else if (i == RS232_PORT_USART6)
                {
                    /* USART6 协议 */
                    RS232_Send(RS232_PORT_USART6, rs232_ports[i].frame_buf, len);
                    cmd_check(rs232_ports[i].frame_buf, len);
                    memset(rs232_ports[i].frame_buf, 0, len);
                }
            }
        }

        osDelay(1);
    }
}

void RS232_Send(RS232_Port_t port, uint8_t *buf, uint16_t len)
{
    RS232_Handle_t *p = &rs232_ports[port];

    xSemaphoreTake(p->tx_sem, portMAX_DELAY);

    HAL_UART_Transmit_DMA(p->huart, buf, len);

    while (__HAL_UART_GET_FLAG(p->huart, UART_FLAG_TC) == RESET)
        ;

    xSemaphoreGive(p->tx_sem);
}
