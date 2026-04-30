#pragma once

#include "main.h"
#include "hub75.h"
#include "cmsis_os.h"
#include "render.h"

#define MODULE_PER_ROW       3  // 每行的模组数
#define MODULE_PER_COL       3  // 每列的模组数
#define MODULE_PIXEL_ROW     80 // 模组每行的像素个数
#define MODULE_PIXEL_COL     40 // 模组每列的像素个数
#define MOUDLE_CHANNEL_NUM   2  // 单块模组的通道个数
#define MOUDLE_SCAN_LINE_NUM 10 // 模组的扫描行数

#define SCREEN_PIXEL_ROW     (MODULE_PER_ROW * MODULE_PIXEL_COL) // 屏幕的像素行数
#define SCREEN_PIXEL_COL     (MODULE_PER_COL * MODULE_PIXEL_ROW) // 屏幕的像素列数

#define DISRAM_SIZE          (SCREEN_PIXEL_ROW * SCREEN_PIXEL_COL)                                       // 显存大小
#define CHANNEL_NUM          (MODULE_PER_ROW * MOUDLE_CHANNEL_NUM)                                       // 总通道数
#define CHANNEL_PIXEL_NUM    (MODULE_PIXEL_ROW * MODULE_PIXEL_COL * MODULE_PER_COL / MOUDLE_CHANNEL_NUM) // 单个通道的像素个数
#define SCAN_LINE_PIXEL_NUM  (CHANNEL_PIXEL_NUM / MOUDLE_SCAN_LINE_NUM)                                  // 每个扫描行所包含的像素个数

typedef struct channel_struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} ChannelStruct_TypeDef;

typedef void (*ColorHandler)(int16_t);

extern uint8_t pixel_map[];
extern uint8_t hub75_buff[];
extern volatile uint8_t light_level;

extern osThreadId_t RefreshTaskHandle;
extern const osThreadAttr_t RefreshTask_attributes;

void convert_pixelmap(void);
void send_hub75_buff(void);
void point_order_test(DispColor_t color, int32_t num_of_point, uint8_t channel);
void pwm_light_handle(void);
void Display_Task(void *argument);
