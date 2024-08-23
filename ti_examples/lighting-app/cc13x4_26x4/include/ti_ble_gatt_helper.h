/*
 *
 *    Copyright (c) 2024 Project CHIP Authors
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

/**
 *    @file
 *      This file is intended to define C based primitives for use with
 *      user defined BLE GATT Services.
 */
#ifndef TI_BLE_GATT_HELPER__H
#define TI_BLE_GATT_HELPER__H

#ifdef __cplusplus
extern "C" {
#endif
#include <icall.h>
#include <icall_ble_api.h>

#include "hal_types.h"

#include "ti_ble_config.h"
#include "ti_drivers_config.h"
#ifdef __cplusplus
}
#endif

/* This starting index must NOT change as BLE Manager reserved events begin prior. */
#define APPOBLE_START_INDEX (200)

/*
 * Application specific events triggered by BLE services.
 * These can be used to run internal BLE service processes, such as reading data from a queue,
 * in the context of the BLE manager task.
 */
typedef enum AppoBLE_events
{
    START_OF_APPOBLE_EVENTS = APPOBLE_START_INDEX,
    EXAMPLE_LIGHT_EVENT,
    /* Add application events here */
} AppoBLE_events_t;
#endif /* TI_BLE_GATT_HELPER__H */