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
#include "RS232.h"
#include "RS485.h"
#include "display.h"
#include "dp83848.h"
#include "func.h"
#include "iwdg.h"
#include "lwip/netif.h"
#include "stm32f4xx_hal.h"
#include "string.h"
#include "voice.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern dp83848_Object_t DP83848;
extern struct netif gnetif;
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

osThreadId_t IWDGTaskHandle;
const osThreadAttr_t IWDGTask_attributes = {
    .name = "IWDGTask",
    .stack_size = 256,
    .priority = (osPriority_t)osPriorityNormal,
};
extern void IWDG_Task(void *argument);

osThreadId_t VoiceTaskHandle;
const osThreadAttr_t Voice_Task_attributes = {
    .name = "VoiceTask",
    .stack_size = 512 * 2,
    .priority = (osPriority_t)osPriorityRealtime3,
};
extern void Voice_Task(void *argument);

osThreadId_t Display_TaskHandle;
const osThreadAttr_t Display_Task_attributes = {
    .name = "Display_Task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityRealtime2,
};
extern void Display_Task(void *argument);

osThreadId_t PHY_TaskHandle;
const osThreadAttr_t PHY_Task_attributes = {
    .name = "PHY_Task",
    .stack_size = 512,
    .priority = (osPriority_t)osPriorityRealtime1,
};
extern void Phy_Link_Task(void *argument);

osThreadId_t NET_TaskHandle;
const osThreadAttr_t NET_Task_attributes = {
    .name = "NET_Task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityRealtime2,
};
extern void Net_Manager_Task(void *argument);

osThreadId_t RS485_TaskHandle;
const osThreadAttr_t RS485_Task_attributes = {
    .name = "RS485_Task",
    .stack_size = 512 * 2,
    .priority = (osPriority_t)osPriorityRealtime2,
};
extern void RS485_Task(void *argument);

osThreadId_t RS232_TaskHandle;
const osThreadAttr_t RS232_Task_attributes = {
    .name = "RS232_Task",
    .stack_size = 512 * 2,
    .priority = (osPriority_t)osPriorityRealtime2,
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
    IWDGTaskHandle = osThreadNew(IWDG_Task, NULL, &IWDGTask_attributes);
    VoiceTaskHandle = osThreadNew(Voice_Task, NULL, &Voice_Task_attributes);
    init_hub75();
    HAL_TIM_Base_Start_IT(&htim3);
    HAL_TIM_Base_Start_IT(&htim4);
    Display_TaskHandle = osThreadNew(Display_Task, NULL, &Display_Task_attributes);
    RS485_Init();
    RS485_TaskHandle = osThreadNew(RS485_Task, NULL, &RS485_Task_attributes);
    RS232_Init();
    RS232_TaskHandle = osThreadNew(RS232_Task, NULL, &RS232_Task_attributes);
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
    DP83848_EnableIT(&DP83848, DP83848_LINK_DOWN_IT);
    PHY_TaskHandle = osThreadNew(Phy_Link_Task, NULL, &PHY_Task_attributes);
    NET_TaskHandle = osThreadNew(Net_Manager_Task, NULL, &NET_Task_attributes);
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
    for (;;)
    {
        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_9);
        osDelay(1000);
    }
}

void IWDG_Task(void *argument)
{
    for (;;)
    {
        HAL_IWDG_Refresh(&hiwdg);
        osDelay(3000);
    }
}

void Display_Task(void *argument)
{
    fontSize = FONT24;

    app_funcs_fill(BLACK);

    displaydataupdate();

    PowerOnDisplay();

    osDelay(1000);

    for (;;)
    {
        osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);

        displaydataupdate();
    }
}

void PHY_LinkProcess(void)
{
    static uint32_t last_state = 0;

    uint32_t state = DP83848_GetLinkState(&DP83848);

    if (state != last_state)
    {
        if (last_state == DP83848_STATUS_100MBITS_FULLDUPLEX && state == DP83848_STATUS_LINK_DOWN)
        {
            NVIC_SystemReset();
        }
        last_state = state;
    }
}

void Phy_Link_Task(void *argument)
{
    for (;;)
    {
        PHY_LinkProcess();
        osDelay(500);
    }
}
/* USER CODE END Application */
