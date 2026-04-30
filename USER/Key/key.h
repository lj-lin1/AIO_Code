#ifndef _KEY_H
#define _KEY_H

#include "main.h"
#include <stdbool.h>
#include "cmsis_os2.h"

extern osSemaphoreId_t test_semaphore;

void TestKey_Task(void *argument);

#endif // !BSP_KEY_KEY_H
