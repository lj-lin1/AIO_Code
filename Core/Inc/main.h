/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdint.h"
#include "stdbool.h"
#include "hub75.h"
#include "stm32f4xx.h"
#include "string.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define W25QXXIPADDR 32 * 1024 * 1024 - 4096
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_G4_Pin            GPIO_PIN_2
#define LED_G4_GPIO_Port      GPIOE
#define LED_B4_Pin            GPIO_PIN_3
#define LED_B4_GPIO_Port      GPIOE
#define LED_R5_Pin            GPIO_PIN_4
#define LED_R5_GPIO_Port      GPIOE
#define LED_G5_Pin            GPIO_PIN_5
#define LED_G5_GPIO_Port      GPIOE
#define LED_B5_Pin            GPIO_PIN_6
#define LED_B5_GPIO_Port      GPIOE
#define LED_R6_Pin            GPIO_PIN_13
#define LED_R6_GPIO_Port      GPIOC
#define LED_G6_Pin            GPIO_PIN_14
#define LED_G6_GPIO_Port      GPIOC
#define LED_B6_Pin            GPIO_PIN_15
#define LED_B6_GPIO_Port      GPIOC
#define LED_R7_Pin            GPIO_PIN_0
#define LED_R7_GPIO_Port      GPIOF
#define LED_G7_Pin            GPIO_PIN_1
#define LED_G7_GPIO_Port      GPIOF
#define LED_B7_Pin            GPIO_PIN_2
#define LED_B7_GPIO_Port      GPIOF
#define LED_R8_Pin            GPIO_PIN_3
#define LED_R8_GPIO_Port      GPIOF
#define LED_G8_Pin            GPIO_PIN_4
#define LED_G8_GPIO_Port      GPIOF
#define LED_B8_Pin            GPIO_PIN_5
#define LED_B8_GPIO_Port      GPIOF
#define LED_R9_Pin            GPIO_PIN_6
#define LED_R9_GPIO_Port      GPIOF
#define LED_G9_Pin            GPIO_PIN_7
#define LED_G9_GPIO_Port      GPIOF
#define LED_B9_Pin            GPIO_PIN_8
#define LED_B9_GPIO_Port      GPIOF
#define LED_R10_Pin           GPIO_PIN_9
#define LED_R10_GPIO_Port     GPIOF
#define LED_G10_Pin           GPIO_PIN_10
#define LED_G10_GPIO_Port     GPIOF
#define LED_B10_Pin           GPIO_PIN_0
#define LED_B10_GPIO_Port     GPIOC
#define W25QXX_CS_Pin         GPIO_PIN_1
#define W25QXX_CS_GPIO_Port   GPIOB
#define SW1_Pin               GPIO_PIN_7
#define SW1_GPIO_Port         GPIOE
#define SW2_Pin               GPIO_PIN_8
#define SW2_GPIO_Port         GPIOE
#define S3_Pin                GPIO_PIN_10
#define S3_GPIO_Port          GPIOE
#define S3_EXTI_IRQn          EXTI15_10_IRQn
#define S2_Pin                GPIO_PIN_11
#define S2_GPIO_Port          GPIOE
#define S2_EXTI_IRQn          EXTI15_10_IRQn
#define S1_Pin                GPIO_PIN_12
#define S1_GPIO_Port          GPIOE
#define S1_EXTI_IRQn          EXTI15_10_IRQn
#define RS232_TX1_Pin         GPIO_PIN_10
#define RS232_TX1_GPIO_Port   GPIOB
#define RS232_RX1_Pin         GPIO_PIN_11
#define RS232_RX1_GPIO_Port   GPIOB
#define KEY_Pin               GPIO_PIN_8
#define KEY_GPIO_Port         GPIOD
#define KEY_EXTI_IRQn         EXTI9_5_IRQn
#define LED_Pin               GPIO_PIN_9
#define LED_GPIO_Port         GPIOD
#define CLED_Pin              GPIO_PIN_14
#define CLED_GPIO_Port        GPIOD
#define CYL_Pin               GPIO_PIN_15
#define CYL_GPIO_Port         GPIOD
#define RS232_TX2_Pin         GPIO_PIN_6
#define RS232_TX2_GPIO_Port   GPIOC
#define RS232_RX2_Pin         GPIO_PIN_7
#define RS232_RX2_GPIO_Port   GPIOC
#define RS485_RE_Pin          GPIO_PIN_8
#define RS485_RE_GPIO_Port    GPIOA
#define RS485_TX_Pin          GPIO_PIN_9
#define RS485_TX_GPIO_Port    GPIOA
#define RS485_RX_Pin          GPIO_PIN_10
#define RS485_RX_GPIO_Port    GPIOA
#define LED_OE_Pin            GPIO_PIN_0
#define LED_OE_GPIO_Port      GPIOD
#define LED_SCK_Pin           GPIO_PIN_1
#define LED_SCK_GPIO_Port     GPIOD
#define LED_LAT_Pin           GPIO_PIN_3
#define LED_LAT_GPIO_Port     GPIOD
#define LED_A_Pin             GPIO_PIN_4
#define LED_A_GPIO_Port       GPIOD
#define LED_B_Pin             GPIO_PIN_5
#define LED_B_GPIO_Port       GPIOD
#define LED_C_Pin             GPIO_PIN_6
#define LED_C_GPIO_Port       GPIOD
#define LED_D_Pin             GPIO_PIN_7
#define LED_D_GPIO_Port       GPIOD
#define LED_R1_Pin            GPIO_PIN_9
#define LED_R1_GPIO_Port      GPIOG
#define LED_G1_Pin            GPIO_PIN_10
#define LED_G1_GPIO_Port      GPIOG
#define LED_B1_Pin            GPIO_PIN_12
#define LED_B1_GPIO_Port      GPIOG
#define LED_R2_Pin            GPIO_PIN_15
#define LED_R2_GPIO_Port      GPIOG
#define W25QXX_CLK_Pin        GPIO_PIN_3
#define W25QXX_CLK_GPIO_Port  GPIOB
#define W25QXX_MISO_Pin       GPIO_PIN_4
#define W25QXX_MISO_GPIO_Port GPIOB
#define W25QXX_MOSI_Pin       GPIO_PIN_5
#define W25QXX_MOSI_GPIO_Port GPIOB
#define LED_G2_Pin            GPIO_PIN_6
#define LED_G2_GPIO_Port      GPIOB
#define LED_B2_Pin            GPIO_PIN_7
#define LED_B2_GPIO_Port      GPIOB
#define LED_R3_Pin            GPIO_PIN_8
#define LED_R3_GPIO_Port      GPIOB
#define LED_G3_Pin            GPIO_PIN_9
#define LED_G3_GPIO_Port      GPIOB
#define LED_B3_Pin            GPIO_PIN_0
#define LED_B3_GPIO_Port      GPIOE
#define LED_R4_Pin            GPIO_PIN_1
#define LED_R4_GPIO_Port      GPIOE

/* USER CODE BEGIN Private defines */
#define BITBAND(addr, bitnum)  ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr)         *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))
// IO口地址映射
#define GPIOA_ODR_Addr (GPIOA_BASE + 20) // 0x40020014
#define GPIOB_ODR_Addr (GPIOB_BASE + 20) // 0x40020414
#define GPIOC_ODR_Addr (GPIOC_BASE + 20) // 0x40020814
#define GPIOD_ODR_Addr (GPIOD_BASE + 20) // 0x40020C14
#define GPIOE_ODR_Addr (GPIOE_BASE + 20) // 0x40021014
#define GPIOF_ODR_Addr (GPIOF_BASE + 20) // 0x40021414
#define GPIOG_ODR_Addr (GPIOG_BASE + 20) // 0x40021814
#define GPIOH_ODR_Addr (GPIOH_BASE + 20) // 0x40021C14
#define GPIOI_ODR_Addr (GPIOI_BASE + 20) // 0x40022014

#define GPIOA_IDR_Addr (GPIOA_BASE + 16) // 0x40020010
#define GPIOB_IDR_Addr (GPIOB_BASE + 16) // 0x40020410
#define GPIOC_IDR_Addr (GPIOC_BASE + 16) // 0x40020810
#define GPIOD_IDR_Addr (GPIOD_BASE + 16) // 0x40020C10
#define GPIOE_IDR_Addr (GPIOE_BASE + 16) // 0x40021010
#define GPIOF_IDR_Addr (GPIOF_BASE + 16) // 0x40021410
#define GPIOG_IDR_Addr (GPIOG_BASE + 16) // 0x40021810
#define GPIOH_IDR_Addr (GPIOH_BASE + 16) // 0x40021C10
#define GPIOI_IDR_Addr (GPIOI_BASE + 16) // 0x40022010

// IO口操作,只对单一的IO口!
// 确保n的值小于16!
#define PAout(n) BIT_ADDR(GPIOA_ODR_Addr, n) // 输出
#define PAin(n)  BIT_ADDR(GPIOA_IDR_Addr, n) // 输入

#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr, n) // 输出
#define PBin(n)  BIT_ADDR(GPIOB_IDR_Addr, n) // 输入

#define PCout(n) BIT_ADDR(GPIOC_ODR_Addr, n) // 输出
#define PCin(n)  BIT_ADDR(GPIOC_IDR_Addr, n) // 输入

#define PDout(n) BIT_ADDR(GPIOD_ODR_Addr, n) // 输出
#define PDin(n)  BIT_ADDR(GPIOD_IDR_Addr, n) // 输入

#define PEout(n) BIT_ADDR(GPIOE_ODR_Addr, n) // 输出
#define PEin(n)  BIT_ADDR(GPIOE_IDR_Addr, n) // 输入

#define PFout(n) BIT_ADDR(GPIOF_ODR_Addr, n) // 输出
#define PFin(n)  BIT_ADDR(GPIOF_IDR_Addr, n) // 输入

#define PGout(n) BIT_ADDR(GPIOG_ODR_Addr, n) // 输出
#define PGin(n)  BIT_ADDR(GPIOG_IDR_Addr, n) // 输入

#define PHout(n) BIT_ADDR(GPIOH_ODR_Addr, n) // 输出
#define PHin(n)  BIT_ADDR(GPIOH_IDR_Addr, n) // 输入

#define PIout(n) BIT_ADDR(GPIOI_ODR_Addr, n) // 输出
#define PIin(n)  BIT_ADDR(GPIOI_IDR_Addr, n) // 输入

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
