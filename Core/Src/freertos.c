/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "main.h"
#include "task.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "IOCtrl.h"
#include "RS232.h"
#include "RS485.h"
#include "crc.h"
#include "display.h"
#include "iwdg.h"
#include "key.h"
#include "lwip/netif.h"
#include "render.h"
#include "rtc.h"
#include "stm32f4xx_hal.h"
#include "string.h"
#include "tcp_server.h"
#include "udp_app.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
osThreadId_t ledTaskHandle;
const osThreadAttr_t ledTask_attributes = {
    .name = "ledTask",
    .stack_size = 256,
    .priority = (osPriority_t)osPriorityRealtime1,
};
extern void Led_Task(void *argument);

osThreadId_t IOCtrlTaskHandle;
const osThreadAttr_t IOCtrlTask_attributes = {
    .name = "IOCtrlTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityLow2,
};

osThreadId_t TestKeyTaskHandle;
const osThreadAttr_t TestKeyTask_attributes = {
    .name = "TestKeyTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityLow2,
};

osThreadId_t Display_TaskHandle;
const osThreadAttr_t Display_Task_attributes = {
    .name = "Display_Task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityLow3,
};
extern void Display_Task(void *argument);

osThreadId_t MotorCtrl_TaskHandle;
const osThreadAttr_t MotorCtrl_Task_attributes = {
    .name = "MotorCtrl_Task",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityRealtime1,
};
extern void MotorCtrl_Task(void *argument);

osThreadId_t MotorData_TaskHandle;
const osThreadAttr_t MotorData_Task_attributes = {
    .name = "MotorData_Task",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityRealtime2,
};
extern void MotorData_Task(void *argument);

osThreadId_t TCP_TaskHandle;
const osThreadAttr_t TCP_Task_attributes = {
    .name = "TCP_Task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityRealtime4,
};
extern void TCP_Manager_Task(void *argument);

osThreadId_t Udp_TaskHandle;
const osThreadAttr_t Udp_Task_attributes = {
    .name = "Udp_Task",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityRealtime4,
};
extern void UDP_Manager_Task(void *argument);

osThreadId_t TcpParse_TaskHandle;
const osThreadAttr_t TcpParse_Task_attributes = {
    .name = "TcpParse_Task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityRealtime5,
};
extern void TcpParse_Task(void *argument);

osThreadId_t Iap_TaskHandle;
const osThreadAttr_t IapTask_attributes = {
    .name = "iap_handle_task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityRealtime5,
};
extern void Iap_Task(void *argument);

osThreadId_t RS485_TaskHandle;
const osThreadAttr_t RS485_Task_attributes = {
    .name = "RS485_Task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityRealtime5,
};
extern void RS485_Task(void *argument);

osThreadId_t RS232_TaskHandle;
const osThreadAttr_t RS232_Task_attributes = {
    .name = "RS232_Task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityRealtime5,
};
extern void RS232_Task(void *argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of defaultTask */
    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    ledTaskHandle = osThreadNew(Led_Task, NULL, &ledTask_attributes);
    configASSERT(ledTaskHandle != NULL);
    IOCtrlTaskHandle = osThreadNew(BSP_S123_Task, NULL, &IOCtrlTask_attributes);
    configASSERT(IOCtrlTaskHandle != NULL);
    TestKeyTaskHandle = osThreadNew(TestKey_Task, NULL, &TestKeyTask_attributes);
    configASSERT(TestKeyTaskHandle != NULL);
    Display_TaskHandle = osThreadNew(Display_Task, NULL, &Display_Task_attributes);
    configASSERT(Display_TaskHandle != NULL);
    MotorCtrl_TaskHandle = osThreadNew(MotorCtrl_Task, NULL, &MotorCtrl_Task_attributes);
    configASSERT(MotorCtrl_TaskHandle != NULL);
    MotorData_TaskHandle = osThreadNew(MotorData_Task, NULL, &MotorData_Task_attributes);
    configASSERT(MotorData_TaskHandle != NULL);
    RS485_Init();
    RS485_TaskHandle = osThreadNew(RS485_Task, NULL, &RS485_Task_attributes);
    configASSERT(RS485_TaskHandle != NULL);
    RS232_Init();
    RS232_TaskHandle = osThreadNew(RS232_Task, NULL, &RS232_Task_attributes);
    configASSERT(RS232_TaskHandle != NULL);
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
    /* init code for LWIP */
    MX_LWIP_Init();
    /* USER CODE BEGIN StartDefaultTask */
    TCP_Server_Init();
    TCP_TaskHandle = osThreadNew(TCP_Manager_Task, NULL, &TCP_Task_attributes);
    configASSERT(TCP_TaskHandle != NULL);
    UDP_Init();
    Udp_TaskHandle = osThreadNew(UDP_Manager_Task, NULL, &Udp_Task_attributes);
    configASSERT(Udp_TaskHandle != NULL);
    TcpParse_TaskHandle = osThreadNew(TcpParse_Task, NULL, &TcpParse_Task_attributes);
    configASSERT(TcpParse_TaskHandle != NULL);
    Iap_TaskHandle = osThreadNew(Iap_Task, NULL, &IapTask_attributes);
    configASSERT(Iap_TaskHandle != NULL);
    /* Infinite loop */
    for (;;)
    {
        osDelay(1);
    }
    /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void Led_Task(void *argument)
{

    uint32_t run_time = 0;

    for (;;)
    {
        if (run_time >= 60)
            HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0);
        else
            run_time++;

        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_9);

        osDelay(500);
    }
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    while (1)
        ;
}
/* USER CODE END Application */
