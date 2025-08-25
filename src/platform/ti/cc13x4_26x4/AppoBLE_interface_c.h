/*
 *
 *    Copyright (c) 2025 Project CHIP Authors
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

/******************************************************************************

 AppoBLE_interface_c.h

 C bindings for the AppoBLE extensions
 
 *****************************************************************************/

#ifndef APPOBLE_INTERFACE_C_H
#define APPOBLE_INTERFACE_C_H

#ifdef __cplusplus
extern "C" {
#endif
#include <bcomdef.h>
#include <icall.h>
#include <icall_ble_api.h>

#include "hal_types.h"

#include "ti_ble_config.h"
#include "ti_drivers_config.h"

#ifdef __cplusplus
}
#endif

/* Callback function prototype for ScheduleAppoBLEFxn() Calls */
typedef void (*pfnCallInBleMgrCtx_t)(void *);
// Callback when a characteristic value has changed
typedef void (*GenericServiceChange_t)(const uint8_t * servUUID, const uint8_t * charUUID, uint8_t len, uint16_t connHandle);

typedef struct
{
    GenericServiceChange_t pfnSimpleProfileChange1; // Called when characteristic value changes
} GenericServiceCBs_t;

extern bStatus_t ScheduleAppoBLEFxn(pfnCallInBleMgrCtx_t FxnPtr, void * arg);

#endif