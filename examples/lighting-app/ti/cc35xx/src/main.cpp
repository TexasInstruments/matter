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

#include <lib/support/CHIPMem.h>
#include <lib/support/CHIPPlatformMemory.h>
#include <mbedtls/platform.h>
#include <platform/CHIPDeviceLayer.h>

#include "AppConfig.h"
#include "AppTask.h"

#include <FreeRTOS.h>

/* Driver Header files */
#include <ti/drivers/Board.h>

using namespace ::chip;
using namespace ::chip::Inet;
using namespace ::chip::DeviceLayer;

typedef int (*hcitrans_rx_cb_t)(uint8_t *data, uint16_t len);
typedef int (*hcitrans_Read)(uint8_t* buf, uint16_t len);
typedef int (*hcitrans_Write)(uint8_t* buf, uint16_t len);

typedef struct
{
    hcitrans_Read read;
    hcitrans_Write write;
    hcitrans_rx_cb_t rx_cb;
}hciTransport_t;

extern "C" int uart_Read(uint8_t* buf, uint16_t len){ return 0; }
extern "C" int uart_Write(uint8_t* buf, uint16_t len){ return 0; }

extern "C" const hciTransport_t uartForHci =
{
    &uart_Read,
    &uart_Write
};

// ================================================================================
// FreeRTOS Callbacks
// ================================================================================
/* Wrapper functions for using the queue registry regardless of whether it is enabled or disabled */
extern "C" void vQueueAddToRegistryWrapper(QueueHandle_t xQueue, const char * pcQueueName)
{
    /* This function is intentionally left empty as the Queue Registry is disabled */
}

extern "C" void vQueueUnregisterQueueWrapper(QueueHandle_t xQueue)
{
    /* This function is intentionally left empty as the Queue Registry is disabled */
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    PLAT_LOG("Stack overflow detected in task: %s\n", pcTaskName);
    taskDISABLE_INTERRUPTS();
    for(;;);
}

extern "C" void vApplicationMallocFailedHook(void) {
    PLAT_LOG("Malloc failed!\n");
    taskDISABLE_INTERRUPTS();
    for(;;);
}

extern "C" { void psa_crypto_init(void); }

extern "C" void UartHciOpen(void)
{
    // Intentionally left empty
}

extern "C" void UartHciClose(void)
{
    // Intentionally left empty
}
 
// ================================================================================
// Main Code
// ================================================================================
int main(void)
{
    Board_init();

    psa_crypto_init();

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
