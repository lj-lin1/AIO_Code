#ifndef BSP_HUB75_HUB75_H
#define BSP_HUB75_HUB75_H

#include "main.h"

// #define STATIC_MODE // 使用静态扫描
#define SCAN_MODE // 使用动态扫描模式

/*
标准75接口点阵屏管脚定义,缺口在左
R1      G1
B1      GND
R2      G2
B2      GND
A     	B
C       D
CLK	    LAT
OE      GND
*/

// SRAM 位带操作宏
#define BITBAND_SRAM(address, bit)                                                                                             \
    (*(volatile uint32_t*)(0x22000000 + ((uint32_t)(address) - 0x20000000) * 0x20 + (bit) * 0x04))

// 外设位带操作宏
#define BITBAND_PERIPH(address, bit)                                                                                           \
    (*(volatile uint32_t*)(0x42000000 + ((uint32_t)(address) - 0x40000000) * 0x20 + (bit) * 0x04))

// HUB75控制信号
#define HUB75_OE  BITBAND_PERIPH(&(LED_OE_GPIO_Port->ODR), 0)
#define HUB75_CLK BITBAND_PERIPH(&(LED_SCK_GPIO_Port->ODR), 1)
#define HUB75_LAT BITBAND_PERIPH(&(LED_LAT_GPIO_Port->ODR), 3)

// HUB75地址通道
#define HUB75_A BITBAND_PERIPH(&(LED_A_GPIO_Port->ODR), 4)
#define HUB75_B BITBAND_PERIPH(&(LED_B_GPIO_Port->ODR), 5)
#define HUB75_C BITBAND_PERIPH(&(LED_C_GPIO_Port->ODR), 6)
#define HUB75_D BITBAND_PERIPH(&(LED_D_GPIO_Port->ODR), 7)

// HUB75颜色通道
#define HUB75_R1 BITBAND_PERIPH(&(LED_R1_GPIO_Port->ODR), 9)
#define HUB75_G1 BITBAND_PERIPH(&(LED_G1_GPIO_Port->ODR), 10)
#define HUB75_B1 BITBAND_PERIPH(&(LED_B1_GPIO_Port->ODR), 12)

#define HUB75_R2 BITBAND_PERIPH(&(LED_R2_GPIO_Port->ODR), 15)
#define HUB75_G2 BITBAND_PERIPH(&(LED_G2_GPIO_Port->ODR), 6)
#define HUB75_B2 BITBAND_PERIPH(&(LED_B2_GPIO_Port->ODR), 7)

#define HUB75_R3 BITBAND_PERIPH(&(LED_R3_GPIO_Port->ODR), 8)
#define HUB75_G3 BITBAND_PERIPH(&(LED_G3_GPIO_Port->ODR), 9)
#define HUB75_B3 BITBAND_PERIPH(&(LED_B3_GPIO_Port->ODR), 0)

#define HUB75_R4 BITBAND_PERIPH(&(LED_R4_GPIO_Port->ODR), 1)
#define HUB75_G4 BITBAND_PERIPH(&(LED_G4_GPIO_Port->ODR), 2)
#define HUB75_B4 BITBAND_PERIPH(&(LED_B4_GPIO_Port->ODR), 3)

#define HUB75_R5 BITBAND_PERIPH(&(LED_R5_GPIO_Port->ODR), 4)
#define HUB75_G5 BITBAND_PERIPH(&(LED_G5_GPIO_Port->ODR), 5)
#define HUB75_B5 BITBAND_PERIPH(&(LED_B5_GPIO_Port->ODR), 6)

#define HUB75_R6 BITBAND_PERIPH(&(LED_R6_GPIO_Port->ODR), 13)
#define HUB75_G6 BITBAND_PERIPH(&(LED_G6_GPIO_Port->ODR), 14)
#define HUB75_B6 BITBAND_PERIPH(&(LED_B6_GPIO_Port->ODR), 15)

#define HUB75_R7 BITBAND_PERIPH(&(LED_R7_GPIO_Port->ODR), 0)
#define HUB75_G7 BITBAND_PERIPH(&(LED_G7_GPIO_Port->ODR), 1)
#define HUB75_B7 BITBAND_PERIPH(&(LED_B7_GPIO_Port->ODR), 2)

#define HUB75_R8 BITBAND_PERIPH(&(LED_R8_GPIO_Port->ODR), 3)
#define HUB75_G8 BITBAND_PERIPH(&(LED_G8_GPIO_Port->ODR), 4)
#define HUB75_B8 BITBAND_PERIPH(&(LED_B8_GPIO_Port->ODR), 5)

#define HUB75_R9 BITBAND_PERIPH(&(LED_R9_GPIO_Port->ODR), 6)
#define HUB75_G9 BITBAND_PERIPH(&(LED_G9_GPIO_Port->ODR), 7)
#define HUB75_B9 BITBAND_PERIPH(&(LED_B9_GPIO_Port->ODR), 8)

#define HUB75_R10 BITBAND_PERIPH(&(LED_R10_GPIO_Port->ODR), 9)
#define HUB75_G10 BITBAND_PERIPH(&(LED_G10_GPIO_Port->ODR), 10)
#define HUB75_B10 BITBAND_PERIPH(&(LED_B10_GPIO_Port->ODR), 0)

void init_hub75(void);

#endif // !BSP_HUB75_HUB75_H