#include "display.h"
#include "tim.h"
#include "voice.h"

osThreadId_t RefreshTaskHandle;
const osThreadAttr_t RefreshTask_attributes = {
    .name = "RefreshTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityAboveNormal,
};

__attribute__((section(".ccmram"))) uint8_t pixel_map[DISRAM_SIZE] = {0};
__attribute__((section(".ccmram"))) uint8_t hub75_buff[DISRAM_SIZE] = {0};

const ChannelStruct_TypeDef channel_red[] = {
    {LED_R1_GPIO_Port, LED_R1_Pin},
    {LED_R2_GPIO_Port, LED_R2_Pin},
    {LED_R3_GPIO_Port, LED_R3_Pin},
    {LED_R4_GPIO_Port, LED_R4_Pin},
    {LED_R5_GPIO_Port, LED_R5_Pin},
    {LED_R6_GPIO_Port, LED_R6_Pin},
    {LED_R7_GPIO_Port, LED_R7_Pin},
    {LED_R8_GPIO_Port, LED_R8_Pin},
    {LED_R9_GPIO_Port, LED_R9_Pin},
    {LED_R10_GPIO_Port, LED_R10_Pin},
};

const ChannelStruct_TypeDef channel_green[] = {
    {LED_G1_GPIO_Port, LED_G1_Pin},
    {LED_G2_GPIO_Port, LED_G2_Pin},
    {LED_G3_GPIO_Port, LED_G3_Pin},
    {LED_G4_GPIO_Port, LED_G4_Pin},
    {LED_G5_GPIO_Port, LED_G5_Pin},
    {LED_G6_GPIO_Port, LED_G6_Pin},
    {LED_G7_GPIO_Port, LED_G7_Pin},
    {LED_G8_GPIO_Port, LED_G8_Pin},
    {LED_G9_GPIO_Port, LED_G9_Pin},
    {LED_G10_GPIO_Port, LED_G10_Pin},
};

const ChannelStruct_TypeDef channel_blue[] = {
    {LED_B1_GPIO_Port, LED_B1_Pin},
    {LED_B2_GPIO_Port, LED_B2_Pin},
    {LED_B3_GPIO_Port, LED_B3_Pin},
    {LED_B4_GPIO_Port, LED_B4_Pin},
    {LED_B5_GPIO_Port, LED_B5_Pin},
    {LED_B6_GPIO_Port, LED_B6_Pin},
    {LED_B7_GPIO_Port, LED_B7_Pin},
    {LED_B8_GPIO_Port, LED_B8_Pin},
    {LED_B9_GPIO_Port, LED_B9_Pin},
    {LED_B10_GPIO_Port, LED_B10_Pin},
};

__STATIC_INLINE void handle_black(int16_t channel_cnt)
{
    channel_red[channel_cnt].port->BSRR = channel_red[channel_cnt].pin << 0x10;
    channel_green[channel_cnt].port->BSRR = channel_green[channel_cnt].pin << 0x10;
    channel_blue[channel_cnt].port->BSRR = channel_blue[channel_cnt].pin << 0x10;
}

__STATIC_INLINE void handle_red(int16_t channel_cnt)
{
    channel_red[channel_cnt].port->BSRR = channel_red[channel_cnt].pin;
    channel_green[channel_cnt].port->BSRR = channel_green[channel_cnt].pin << 0x10;
    channel_blue[channel_cnt].port->BSRR = channel_blue[channel_cnt].pin << 0x10;
}

__STATIC_INLINE void handle_green(int16_t channel_cnt)
{
    channel_red[channel_cnt].port->BSRR = channel_red[channel_cnt].pin << 0x10;
    channel_green[channel_cnt].port->BSRR = channel_green[channel_cnt].pin;
    channel_blue[channel_cnt].port->BSRR = channel_blue[channel_cnt].pin << 0x10;
}

__STATIC_INLINE void handle_blue(int16_t channel_cnt)
{
    channel_red[channel_cnt].port->BSRR = channel_red[channel_cnt].pin << 0x10;
    channel_green[channel_cnt].port->BSRR = channel_green[channel_cnt].pin << 0x10;
    channel_blue[channel_cnt].port->BSRR = channel_blue[channel_cnt].pin;
}

__STATIC_INLINE void handle_yellow(int16_t channel_cnt)
{
    channel_red[channel_cnt].port->BSRR = channel_red[channel_cnt].pin;
    channel_green[channel_cnt].port->BSRR = channel_green[channel_cnt].pin;
    channel_blue[channel_cnt].port->BSRR = channel_blue[channel_cnt].pin << 0x10;
}

__STATIC_INLINE void handle_purple(int16_t channel_cnt)
{
    channel_red[channel_cnt].port->BSRR = channel_red[channel_cnt].pin;
    channel_green[channel_cnt].port->BSRR = channel_green[channel_cnt].pin << 0x10;
    channel_blue[channel_cnt].port->BSRR = channel_blue[channel_cnt].pin;
}

__STATIC_INLINE void handle_cyan(int16_t channel_cnt)
{
    channel_red[channel_cnt].port->BSRR = channel_red[channel_cnt].pin << 0x10;
    channel_green[channel_cnt].port->BSRR = channel_green[channel_cnt].pin;
    channel_blue[channel_cnt].port->BSRR = channel_blue[channel_cnt].pin;
}

__STATIC_INLINE void handle_white(int16_t channel_cnt)
{
    channel_red[channel_cnt].port->BSRR = channel_red[channel_cnt].pin;
    channel_green[channel_cnt].port->BSRR = channel_green[channel_cnt].pin;
    channel_blue[channel_cnt].port->BSRR = channel_blue[channel_cnt].pin;
}

// 跳转表
ColorHandler color_handlers[] = {
    handle_black,  // default
    handle_red,    // case red
    handle_green,  // case green
    handle_yellow, // case yellow
    handle_blue,   // case blue
    handle_purple, // case purple
    handle_cyan,   // case cyan
    handle_white,  // case white
};

/**
 * @brief P4模组，编号1-800
 * @brief 通用一体机扫描，竖安装
 */
void convert_pixelmap(void)
{
    uint32_t col = 0;
    uint32_t row = 0;
    uint32_t col_cnt = 0;

    const uint32_t half_col = MODULE_PIXEL_COL >> 1;
    const uint32_t module_size = MODULE_PIXEL_COL * MODULE_PIXEL_ROW;

    for (uint32_t i = 0; i < DISRAM_SIZE; i++)
    {
        /* -------- 行列计算（原逻辑等价） -------- */
        if (col_cnt >= SCREEN_PIXEL_ROW)
        {
            col_cnt = 0;
            row++;
        }
        col = col_cnt / MODULE_PIXEL_COL;
        uint32_t base_i = i - ((MODULE_PER_ROW - 1) * row + col) * MODULE_PIXEL_COL;

        /* -------- 以下为 Updata_info 完整展开 -------- */
        uint32_t mod_col = base_i % MODULE_PIXEL_COL;
        uint32_t bank = mod_col / half_col;
        uint32_t md = (mod_col % half_col) / MOUDLE_SCAN_LINE_NUM;
        uint32_t ket = base_i / module_size;
        uint32_t base = (MODULE_PIXEL_COL - bank * half_col) * SCREEN_PIXEL_COL;
        uint32_t i_div_col = base_i / MODULE_PIXEL_COL;
        uint32_t i_mod_10 = base_i % 10;
        uint32_t map = 0;
        if (md == 0)
        {
            map = base - SCREEN_PIXEL_COL * (i_mod_10 * 2 + 2) + i_div_col + ket * 80;
        }
        else
        {
            map = base - SCREEN_PIXEL_COL * (i_mod_10 * 2 + 1) + i_div_col - (MODULE_PER_COL - ket - 1) * 80;
        }

        /* -------- 原最终写入 -------- */
        uint32_t offset = map + col * MODULE_PIXEL_COL * SCREEN_PIXEL_COL;
        hub75_buff[offset] = pixel_map[i];
        col_cnt++;
    }
}

static uint32_t BKData = 0x0B23 << 16; // 消影数据
/**
 * @brief 动态扫描行切换
 * @param line_cnt 行计算
 */
void scan_channel(uint8_t line_cnt)
{
    static uint8_t BKNum = 0;

    /* ===== 行0特殊处理 ===== */
    if (line_cnt == 0)
    {
        LED_C = 1;
        __NOP();
        __NOP();
    }

    /* ===== BK数据输出（等价 BK_Write_Byte） ===== */
    LED_B = (BKData & (0x80UL >> BKNum)) ? 1 : 0;
    BKNum++;

    __NOP();
    __NOP();

    /* ===== 行时钟 ===== */
    LED_A = 1;
    __NOP();
    __NOP();
    LED_A = 0;

    LED_C = 0;
}

#define SCAN_CNT (addr * SCAN_LINE_PIXEL_NUM + channel_cnt * CHANNEL_PIXEL_NUM)
/**
 * @brief 发送显存数据到HUB75接口
 */
void send_hub75_buff(void)
{
    static uint8_t addr = 0;
    // 外层循环：遍历每一行的像素点
    for (int16_t line_cnt = 0; line_cnt < SCAN_LINE_PIXEL_NUM; line_cnt++)
    {
        // 内层循环：遍历每个通道
        for (int16_t channel_cnt = 0; channel_cnt < CHANNEL_NUM; channel_cnt++)
        {
            // 从缓冲区获取当前LED的颜色索引，并调用对应的颜色处理函数
            DispColor_t color_index = (DispColor_t)hub75_buff[line_cnt + SCAN_CNT];
            color_handlers[color_index](channel_cnt);
        }

        // 生成时钟信号，用于数据传输
        LED_CLK = 1;
        __NOP();
        __NOP();
        LED_CLK = 0;
    }

    NVIC_DisableIRQ(TIM4_IRQn);
    LED_OE = 1;
    // LE信号给一个周期，使数据从移位寄存器进入输出锁存器
    scan_channel(addr);
    LED_LAT = 1;
    __NOP();
    __NOP();
    LED_LAT = 0;
    NVIC_EnableIRQ(TIM4_IRQn);

    addr++;
    if (addr >= MOUDLE_SCAN_LINE_NUM)
        addr = 0;

    // LED输出信号使能
    __NOP();
    __NOP();
    LED_OE = 0;
}

/**
 * @brief 模组点顺序测试，用于单个模组的像素点规律寻找
 *
 * @param color 颜色
 * @param num_of_point 点数
 * @param channel 通道数
 */
void point_order_test(DispColor_t color, int32_t num_of_point, uint8_t channel)
{
    color_handlers[color](channel);

    for (int32_t i = 0; i < num_of_point; i++)
    {
        LED_CLK = 0;
        __NOP();
        __NOP();
        LED_CLK = 1;
        __NOP();
        __NOP();
    }

    HAL_NVIC_DisableIRQ(TIM4_IRQn);
    LED_OE = 1;

    LED_LAT = 1;
    __NOP();
    __NOP();
    LED_LAT = 0;
    __NOP();
    __NOP();
    LED_OE = 0;
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
}

volatile uint8_t light_level = 1; // 亮度等级
/**
 * @brief 软件pwm调光
 *
 */
void pwm_light_handle(void)
{
    static uint8_t pwm_cnt = 0; // pwm计数

    if (pwm_cnt < light_level) // light_level的取值范围：0~7,0为关闭显示
        LED_OE = 0;
    else
        LED_OE = 1;

    pwm_cnt++;
    if (pwm_cnt >= 8)
        pwm_cnt = 0;
}

void Display_Task(void *argument)
{
    __HAL_DBGMCU_FREEZE_TIM3();
    __HAL_DBGMCU_FREEZE_TIM4();

    light_level = 1;
    init_hub75();
    memset(pixel_map, 0, sizeof(pixel_map));
    memset(hub75_buff, 0, sizeof(hub75_buff));
    HAL_TIM_Base_Start_IT(&htim3);
    HAL_TIM_Base_Start_IT(&htim4);

    RenderString(0, 0, (uint8_t *)"欢迎行驶_高速公路_祝您旅途愉快", strlen("欢迎行驶_高速公路_祝您旅途愉快"), green, func.ft_size, func.ft_type, true);
    TxVoce((uint8_t *)"欢迎行驶 高速公路 祝您旅途愉快", strlen("欢迎行驶 高速公路 祝您旅途愉快"));

    convert_pixelmap();

    for (;;)
    {
        osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
        convert_pixelmap();
        osDelay(10);
    }
}
