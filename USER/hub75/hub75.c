#include "hub75.h"
#include "display.h"
#include "main.h"
#include "render.h"

uint8_t fontColor = red;
uint8_t fontSize = font_16;
uint8_t fontType = font_ht;

void init_hub75(void)
{
    // 初始化控制信号
    LED_OE = 1;
    LED_LAT = 0;
    LED_CLK = 0;

    // 初始化地址通道
    LED_A = 0;
    LED_B = 0;
    LED_C = 0;
    LED_D = 0;

    // 初始化颜色通道
    LED_R1 = 0;
    LED_G1 = 0;
    LED_B1 = 0;

    LED_R2 = 0;
    LED_G2 = 0;
    LED_B2 = 0;
}