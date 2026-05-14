#include "key.h"
#include "cmd.h"
#include "display.h"
#include "freertos_os2.h"
#include "gpio.h"
#include "render.h"

osSemaphoreId_t test_semaphore;

static const uint8_t product_code[] = "程序编码：_9J6F1C5112";

static const uint8_t testDisBuf[][240] = {
    "重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重重",
    "庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆庆",
    "创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创创",
    "迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪迪",
    "科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科科",
    "技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技技",
    "屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏屏",
    "幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕幕",
    "测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测测",
    "试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试试",
    "                                                                                                                                                                                                            ",
};

void TestKey_Task(void *argument)
{
    test_semaphore = osSemaphoreNew(1, 0, NULL);

    uint32_t test_cnt = 0;
    uint32_t testDisPlayCnt = 0;
    DispColor_t tempColor = red;
    FontSize_t tempSize = font_16;
    FontType_t tempType = font_fs;
    uint8_t lamp_true = 0;

    for (;;)
    {
        osSemaphoreAcquire(test_semaphore, osWaitForever);

        test_cnt++;

        if (test_cnt == 1)
            Disp_Fill(green, 0);
        else if (test_cnt == 2)
            Disp_Fill(red, 0);
        else if (test_cnt == 3)
            Disp_Fill(yellow, 0);
        else if (test_cnt == 4)
            RenderString(0, 0, product_code, strlen((char *)product_code), green, font_20, font_ht, true);
        else
            break;
    }

    for (;;)
    {
        if (osSemaphoreAcquire(test_semaphore, 10) == osOK)
            NVIC_SystemReset();

        RenderString(0, 0, (uint8_t *)testDisBuf[testDisPlayCnt], 240, tempColor, tempSize, tempType, true);
        testDisPlayCnt++;
        if (testDisPlayCnt >= 11)
        {
            tempColor++;
            tempSize++;
            tempType++;
            if (tempSize > font_24)
            {
                LAMP = ~lamp_true;
                tempSize = font_16;
            }
            if (tempColor > yellow)
            {
                tempColor = red;
            }
            if (tempType > font_ht)
            {
                tempType = font_st;
            }
            testDisPlayCnt = 0;
        }

        osDelay(1000);
    }
}