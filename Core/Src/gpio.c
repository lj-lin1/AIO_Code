/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    gpio.c
 * @brief   This file provides code for the configuration
 *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, LED_G4_Pin|LED_B4_Pin|LED_R5_Pin|LED_G5_Pin
                          |LED_B5_Pin|LED_B3_Pin|LED_R4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LED_R6_Pin|LED_G6_Pin|LED_B6_Pin|LED_B10_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, LED_R7_Pin|LED_G7_Pin|LED_B7_Pin|LED_R8_Pin
                          |LED_G8_Pin|LED_B8_Pin|LED_R9_Pin|LED_G9_Pin
                          |LED_B9_Pin|LED_R10_Pin|LED_G10_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, W25QXX_CS_Pin|LED_G2_Pin|LED_B2_Pin|LED_R3_Pin
                          |LED_G3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LED_Pin|CLED_Pin|CYL_Pin|LED_OE_Pin
                          |LED_SCK_Pin|LED_LAT_Pin|LED_A_Pin|LED_B_Pin
                          |LED_C_Pin|LED_D_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, LED_R1_Pin|LED_G1_Pin|LED_B1_Pin|LED_R2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_G4_Pin LED_B4_Pin LED_R5_Pin LED_G5_Pin
                           LED_B5_Pin LED_B3_Pin LED_R4_Pin */
  GPIO_InitStruct.Pin = LED_G4_Pin|LED_B4_Pin|LED_R5_Pin|LED_G5_Pin
                          |LED_B5_Pin|LED_B3_Pin|LED_R4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_R6_Pin LED_G6_Pin LED_B6_Pin LED_B10_Pin */
  GPIO_InitStruct.Pin = LED_R6_Pin|LED_G6_Pin|LED_B6_Pin|LED_B10_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_R7_Pin LED_G7_Pin LED_B7_Pin LED_R8_Pin
                           LED_G8_Pin LED_B8_Pin LED_R9_Pin LED_G9_Pin
                           LED_B9_Pin LED_R10_Pin LED_G10_Pin */
  GPIO_InitStruct.Pin = LED_R7_Pin|LED_G7_Pin|LED_B7_Pin|LED_R8_Pin
                          |LED_G8_Pin|LED_B8_Pin|LED_R9_Pin|LED_G9_Pin
                          |LED_B9_Pin|LED_R10_Pin|LED_G10_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : W25QXX_CS_Pin LED_G2_Pin LED_B2_Pin LED_R3_Pin
                           LED_G3_Pin */
  GPIO_InitStruct.Pin = W25QXX_CS_Pin|LED_G2_Pin|LED_B2_Pin|LED_R3_Pin
                          |LED_G3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : SW1_Pin SW2_Pin */
  GPIO_InitStruct.Pin = SW1_Pin|SW2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : S3_Pin S2_Pin S1_Pin */
  GPIO_InitStruct.Pin = S3_Pin|S2_Pin|S1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : KEY_Pin */
  GPIO_InitStruct.Pin = KEY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_Pin CLED_Pin CYL_Pin LED_OE_Pin
                           LED_SCK_Pin LED_LAT_Pin LED_A_Pin LED_B_Pin
                           LED_C_Pin LED_D_Pin */
  GPIO_InitStruct.Pin = LED_Pin|CLED_Pin|CYL_Pin|LED_OE_Pin
                          |LED_SCK_Pin|LED_LAT_Pin|LED_A_Pin|LED_B_Pin
                          |LED_C_Pin|LED_D_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : RS485_RE_Pin */
  GPIO_InitStruct.Pin = RS485_RE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RS485_RE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_R1_Pin LED_G1_Pin LED_B1_Pin LED_R2_Pin */
  GPIO_InitStruct.Pin = LED_R1_Pin|LED_G1_Pin|LED_B1_Pin|LED_R2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 15, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 15, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
