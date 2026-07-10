/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include <stdbool.h>
#include <stdint.h>

#include <platform/CHIPDeviceLayer.h>

#include "AppConfig.h"
#include "AppTask.h"

#include <FreeRTOS.h>

/* Driver Header files */
#include <ti/drivers/AESECB.h>
#include <ti/drivers/Board.h>
#include <ti/drivers/ECDH.h>
#include <ti/drivers/ECDSA.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/NVS.h>
#include <ti/drivers/SHA2.h>

using namespace ::chip;
using namespace ::chip::Inet;
using namespace ::chip::DeviceLayer;

// ================================================================================
// FreeRTOS Callbacks
// ================================================================================
/* Wrapper functions for using the queue registry regardless of whether it is enabled or disabled */
#if (configQUEUE_REGISTRY_SIZE == 0)
/* Wrapper functions for using the queue registry regardless of whether it is enabled or disabled */
extern "C" void vQueueAddToRegistryWrapper(QueueHandle_t xQueue, const char * pcQueueName) {}

extern "C" void vQueueUnregisterQueueWrapper(QueueHandle_t xQueue) {}
#else
extern "C" void vQueueAddToRegistryWrapper(QueueHandle_t xQueue, const char * pcQueueName)
{
    vQueueAddToRegistry(xQueue, pcQueueName);
}

extern "C" void vQueueUnregisterQueueWrapper(QueueHandle_t xQueue)
{
    vQueueUnregisterQueue(xQueue);
}
#endif

// ================================================================================
// Main Code
// ================================================================================
int main(void)
{
    Board_init();

    GPIO_init();

    NVS_init();

    ECDH_init();

    ECDSA_init();

    AESECB_init();

    SHA2_init();

    int ret = GetAppTask().StartAppTask();
    if (ret != 0)
    {
        // can't log until the kernel is started
        // PLAT_LOG("GetAppTask().StartAppTask() failed");
        while (1)
            ;
    }

    vTaskStartScheduler();

    // Should never get here.
    while (1)
        ;
}
#ifdef configUSE_MALLOC_FAILED_HOOK
void vApplicationMallocFailedHook(void)
{
    PLAT_LOG("Malloc Failure");
    taskDISABLE_INTERRUPTS();
    for (;;)
        ;
}
#endif

#ifdef CONFIG_TI_DRIVERS_LOGSINKUART_COUNT
extern void LogSinkUART_flush(void);
#endif

extern "C" void vApplicationIdleHook(void)
{
#ifdef CONFIG_TI_DRIVERS_LOGSINKUART_COUNT
    LogSinkUART_flush();
#endif
}