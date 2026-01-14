#include "hub75.h"
#include "main.h"

void init_hub75(void)
{
    // 初始化控制信号
    HUB75_OE = 1;
    HUB75_LAT = 0;
    HUB75_CLK = 0;

    // 初始化地址通道
    HUB75_A = 0;
    HUB75_B = 0;
    HUB75_C = 0;
    HUB75_D = 0;

    // 初始化颜色通道
    HUB75_R1 = 0;
    HUB75_G1 = 0;
    HUB75_B1 = 0;

    HUB75_R2 = 0;
    HUB75_G2 = 0;
    HUB75_B2 = 0;
}