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
 *      This file implements the callback API's for the Application Over BLE interface used
 *      to manage additional BLE advertisements and connections to
 *      work in tandem with the MatterOverBLE GATT service.
 */
#ifndef APPOBLE__H
#define APPOBLE__H

#include <lib/support/DLLUtil.h>
#include <platform/CHIPDeviceLayer.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <icall.h>
#include <icall_ble_api.h>

#include "hal_types.h"
#include "ti_ble_config.h"
#include "ti_dmm_application_policy.h"
#include "ti_drivers_config.h"

#include <dmm/dmm_policy.h>
#include <dmm/dmm_priority_ble_thread.h>
#include <dmm/dmm_scheduler.h>

#include <diagnosticService.h>
#include <lightingService.h>
#include <ti_ble_gatt_helper.h>

#ifdef OAD_APP_OFFCHIP
#include <oad_profile.h>
#endif
#include <throughputService.h>
#include <ti_ble_gatt_helper.h>
#ifdef __cplusplus
}
#endif

using namespace ::chip;
using namespace ::chip::app;
using namespace ::chip::DeviceLayer;
using namespace ::chip::DeviceLayer::Internal;
/* Enable throughput testing service functionality */
#define THROUGHPUT_SERVICE_ENABLE 1
#define CB_SIZE 8
#define NUM_MESSAGES 700
typedef struct
{
    uint16_t bytes;                      //  Total amount of bytes sent/received
    uint16_t stored_bytes[NUM_MESSAGES]; //  buffer to hold the number of bytes send in a message
    uint32_t bytes_cb[CB_SIZE];          //  Circular buffer for the CB_SIZE latest bytes
    uint8_t bytes_cb_index;              //  index of circular buffer
    bool cbBufferFilled;                 //  Whether the buffer is full
    uint16_t message_count;              //  Index for the current position in the circular buffer
    uint32_t log_message_count;          // holds number of messages for current messages for 1 second period
    uint32_t packet_sequence_num;        // first 4 bytes of payload to check sequence
} ThroughputRate_t;

void myClockSwiFxn(uintptr_t arg0);
void throughputClockSwiFxn(uintptr_t arg0);
void throughputGetInfo(ThroughputRate_t * throughput_data);
void throughputProcessAvgSpeed(ThroughputRate_t * throughput_data);
void throughput_sequence_cpy(uint8_t * packet, uint32_t sequence_num);
void matterThreadDisable();
void matterThreadEnable();

/* Forward delcaration for characteristic change callback */
extern void AppoBLE_GenericcharValueChangeCB(const uint8_t * servUUID, const uint8_t * charUUID, uint8_t len, uint16_t connHandle);
extern void LightServiceTestCB(uint8_t event, uint8_t dummy);
extern void eraseAllBonds(void * unused);
#ifdef OAD_APP_OFFCHIP
extern OADProfile_AppCommand_e App_OADCallback(OADProfile_App_Msg_e msg);
#endif
/* AppoBLE Interface */
class DLL_EXPORT AppoBLE : public AppoBLE_interface
{
public:
};

#endif /* APPOBLE__H */