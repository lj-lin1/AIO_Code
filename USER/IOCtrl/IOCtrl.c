#include "IOCtrl.h"
#include "cmsis_os2.h"

osEventFlagsId_t S123_Event;

/*
 * @brief  BSP_HSCtrl  (????)
 * @param  HSEnable: true: ??, false: ??
 */
void BSP_HSCtrl(bool HSEnable)
{
    do
    {
        HAL_GPIO_WritePin(CLED_GPIO_Port, CLED_Pin, HSEnable ? GPIO_PIN_SET : GPIO_PIN_RESET);
    } while (0);
}

/*
 * @brief  BSP_LILCtrl  (?????)
 * @param  LILEnable: true: ??, false: ??
 */
void BSP_LILCtrl(bool LILEnable)
{
    do
    {
        HAL_GPIO_WritePin(CYL_GPIO_Port, CYL_Pin, LILEnable ? GPIO_PIN_SET : GPIO_PIN_RESET);
    } while (0);
}

__weak void S1Ctrl(void)
{
}

__weak void S2Ctrl(void)
{
}

__weak void S3Ctrl(void)
{
}

void BSP_S123_Task(void *argument)
{
    uint32_t flags = 0;
    S123_Event = osEventFlagsNew(NULL);

    for (;;)
    {
        flags = osEventFlagsWait(
            S123_Event,
            S1_EVENT | S2_EVENT | S3_EVENT,
            osFlagsWaitAny,
            200);

        switch (flags)
        {
        case S1_EVENT:
            S1Ctrl();
            break;
        case S2_EVENT:
            S2Ctrl();
            break;
        case S3_EVENT:
            S3Ctrl();
            break;
        default:
            break;
        }

        osDelay(50);
    }
}