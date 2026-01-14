#include "rs485.h"
#include <string.h>
#include "cmd.h"

/* ================= 外部句柄 ================= */

extern UART_HandleTypeDef huart1;

/* ================= 静态资源 ================= */

static uint8_t rs485_rx_dma_buf[RS485_RX_DMA_LEN];
static uint8_t rs485_frame_buf[RS485_RX_DMA_LEN];

static QueueHandle_t rs485_rx_queue;
static SemaphoreHandle_t rs485_tx_sem;

/* ================= RS485 DE 控制 ================= */

static inline void RS485_DE_Tx(void)
{
    HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET);
}

static inline void RS485_DE_Rx(void)
{
    HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET);
}

/* ================= 初始化 ================= */

void RS485_Init(void)
{
    /* 默认接收状态 */
    RS485_DE_Rx();

    /* 队列：ISR → 任务（传帧长度） */
    rs485_rx_queue = xQueueCreate(4, sizeof(uint16_t));

    /* 发送互斥 */
    rs485_tx_sem = xSemaphoreCreateBinary();
    xSemaphoreGive(rs485_tx_sem);

    /* 启动 DMA + IDLE 接收（HAL 官方方式） */
    HAL_UARTEx_ReceiveToIdle_DMA(&RS485_UART,
                                 rs485_rx_dma_buf,
                                 RS485_RX_DMA_LEN);

    /* 重要：关闭 DMA 半传输中断 */
    __HAL_DMA_DISABLE_IT(RS485_UART.hdmarx, DMA_IT_HT);
}

/* ================= HAL 接收完成回调 ================= */
/* HAL 内部在 IDLE / 满缓冲 时调用 */

void RS485_RxEventFromISR(UART_HandleTypeDef *huart,
                          uint16_t Size)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (huart == &huart1)
    {
        xQueueSendFromISR(rs485_rx_queue,
                          &Size,
                          &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* ================= FreeRTOS 接收任务 ================= */

void RS485_Task(void *argument)
{
    uint16_t len = 0;

    for (;;)
    {
        if (xQueueReceive(rs485_rx_queue, &len, portMAX_DELAY) == pdPASS)
        {
            /* 拷贝一整帧 */
            memcpy(rs485_frame_buf, rs485_rx_dma_buf, len);

            /* 重新启动接收（HAL 要求每帧重启） */
            HAL_UARTEx_ReceiveToIdle_DMA(&RS485_UART,
                                         rs485_rx_dma_buf,
                                         RS485_RX_DMA_LEN);

            __HAL_DMA_DISABLE_IT(RS485_UART.hdmarx, DMA_IT_HT);

            /* ================= 协议解析区 ================= */
            /* rs485_frame_buf[0..len-1] */
            /* 例如：
               Modbus_RTU_Parse(rs485_frame_buf, len);
            */
        }
    }
}

/* ================= RS485 DMA 发送（阻塞到 TC） ================= */

void RS485_Send(uint8_t *buf, uint16_t len)
{
    /* 串行化发送 */
    xSemaphoreTake(rs485_tx_sem, portMAX_DELAY);

    RS485_DE_Tx();

    HAL_UART_Transmit_DMA(&RS485_UART, buf, len);

    /* 等待真正发完（包括停止位） */
    while (__HAL_UART_GET_FLAG(&RS485_UART, UART_FLAG_TC) == RESET)
        ;

    RS485_DE_Rx();

    xSemaphoreGive(rs485_tx_sem);
}
