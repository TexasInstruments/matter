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

#include <AppConfig.h>
#include <AppEvent.h>
#include <AppTask.h>
#include <AppoBLE.h>
#ifdef OAD_APP_OFFCHIP
#include <bootutil/bootutil.h>
#endif
#include <diagnosticService.h>
#include <lightingService.h>
#include <throughputService.h>
#include <ti/drivers/dpl/ClockP.h>

extern "C" {
#if defined(ti_log_Log_ENABLE)
#include "ti_drivers_config.h"
#include "ti_log_config.h"
#define PLAT_LOG(...) Log_printf(LogModule_App1, Log_DEBUG, __VA_ARGS__);
#endif
}

using namespace ::chip;
using namespace ::chip::app;
using namespace ::chip::Credentials;
using namespace ::chip::DeviceLayer;
using namespace ::chip::DeviceLayer::Internal;

LightServiceCBs_t LightServiceAppCBs = {
    // Provisioning GATT Characteristic value change callback
    AppoBLE_GenericcharValueChangeCB, LightServiceTestCB
};

diagnosticServiceCBs_t DiagnosticServiceAppCBs = {
    // Provisioning GATT Characteristic value change callback
    AppoBLE_GenericcharValueChangeCB
};

throughputServiceCBs_t throughputServiceAppCBs = {
    // Provisioning GATT Characteristic value change callback
    AppoBLE_GenericcharValueChangeCB
};

// Clock instance for periodic diagnostic reading
static ClockP_Struct diagRefresh;

#if THROUGHPUT_SERVICE_ENABLE
// Clock instance for thorughput service
static ClockP_Struct throughputClock;

uint8_t upstream_flag   = 0;
uint8_t downstream_flag = 0;

ThroughputRate_t upstream_calc;

ThroughputRate_t downstream_calc;

uint8_t dummy_data[244];
#endif /* THROUGHPUT_SERVICE_ENABLE */

uint8_t appoBLEadvHandle = ADV_INDEX_INVALID;
/*********************************************************************
 * @fn      AppoBLE_GenericcharValueChangeCB
 *
 * @brief   Callback from Generic Matter Service indicating a characteristic
 *          value change.
 *          Calling context (BLE Stack Task)
 *
 * @param   ServUUID - Pointer to service UUID
 * @param   charUUID - Pointer to Characteristic UUID changed
 * @param   len - Length of data changed
 * @param   connHandle - Connection on which data was changed
 *
 * @return  None.
 */
void AppoBLE_GenericcharValueChangeCB(const uint8_t * servUUID, const uint8_t * charUUID, uint8_t len, uint16_t connHandle)
{
    AppoBLE_interface().SendAppoBLEReadRequest((BLE_CONNECTION_OBJECT) &connHandle, (const ChipBleUUID *) servUUID,
                                               (const ChipBleUUID *) charUUID, NULL, len);
}

/* Reference callback implementation for custom events posted by a service */
void LightServiceTestCB(uint8_t event, uint8_t dummy)
{
    /* Map to valid AppoBLE event type */
    if (event == LIGHT_SERVICE_TEST_EVENT)
    {
        if (SUCCESS != AppoBLE_interface().EnqueueAppoBLEMsg(EXAMPLE_LIGHT_EVENT, NULL))
        {
            PLAT_LOG("Light Service Test Event Failure.");
        }
    }
}

/* Application callbacks
 * User's must update this API with service specific functionality desired.
 */
void AppoBLE_interface::HandleAppoBLEWriteReceived(BLE_CONNECTION_OBJECT conId, const ChipBleUUID * svcId,
                                                   const ChipBleUUID * charId, uint8_t const * dstBuf, uint16_t dstBufLen,
                                                   uint8_t status)
{
    if (status == SUCCESS)
    {
        /* Light service */
        if (UUIDsMatch((ChipBleUUID *) LightServiceServUUID, svcId))
        {
            // Emulate external button press
            AppEvent event;
            event.Type = AppEvent::AppEventType::kEventType_None;

            if (UUIDsMatch((ChipBleUUID *) LightServiceLightOnOffUUID, charId))
            {
                /* Light On/Off */
                if (dstBuf[0] == 1)
                {
                    event.Type = AppEvent::kEventType_ButtonLeft; // on
                }
                else
                {
                    event.Type = AppEvent::kEventType_ButtonRight; // off
                }
                event.ButtonEvent.Type = AppEvent::kAppEventButtonType_Clicked;
            }
            else if (UUIDsMatch((ChipBleUUID *) LightServiceLightToggleUUID, charId))
            {
                /* Light Toggle */
                event.Type             = AppEvent::kEventType_Light;
                event.ButtonEvent.Type = AppEvent::kAppEventButtonType_Clicked;
            }
            if (event.Type != AppEvent::AppEventType::kEventType_None)
            {
                GetAppTask().PostEvent(&event);
            }
        }

        /* Diagnostic service*/
        if (UUIDsMatch((ChipBleUUID *) diagnosticServiceUUID, svcId))
        {
            AppEvent event;
            event.Type = AppEvent::AppEventType::kEventType_None;

            uint16_t clockPeriod = 0;

            /* diagnostic refresh*/
            if (UUIDsMatch((ChipBleUUID *) diagnosticService_RefreshUUID, charId))
            {
                event.Type = AppEvent::kEventType_Diagnostic_refresh;
            }
            /* timer period change*/
            else if (UUIDsMatch((ChipBleUUID *) diagnosticService_PeriodUUID, charId))
            {
                clockPeriod = ((uint16) dstBuf[0] << 8) | dstBuf[1];

                if (clockPeriod > 0)
                {
                    ClockP_setPeriod(ClockP_handle(&diagRefresh), (clockPeriod * (1000 / ClockP_getSystemTickPeriod())));
                    // start clock
                    ClockP_start(ClockP_handle(&diagRefresh));
                }
                else
                {
                    ClockP_stop(ClockP_handle(&diagRefresh));
                }
            }

            if (event.Type != AppEvent::AppEventType::kEventType_None)
            {
                GetAppTask().PostEvent(&event);
            }
        }
/* Throughput service*/
#if THROUGHPUT_SERVICE_ENABLE
        if (UUIDsMatch((ChipBleUUID *) throughputServiceUUID, svcId))
        {
            AppEvent event;
            event.Type = AppEvent::AppEventType::kEventType_None;

            /* throughput calculation enable*/
            if (UUIDsMatch((ChipBleUUID *) throughputService_ThroughputEnableUUID, charId))
            {
                PLAT_LOG("throughput enable write");
                // Turn on clock
                if (dstBuf[0] == 1)
                {
                    upstream_flag = 1;
                    // Send first notification to start chain
                    AppoBLE().SendAppoBLEWriteRequest(conId, (ChipBleUUID *) throughputServiceUUID,
                                                      (ChipBleUUID *) throughputService_ThroughputTxUUID, (uint8_t *) dummy_data,
                                                      sizeof(dummy_data));

                    ClockP_start(ClockP_handle(&throughputClock));
                }
                else
                {
                    upstream_flag = 0;
                    downstream_flag = 0;
                    ClockP_stop(ClockP_handle(&throughputClock));

                    // clear out calculation structs
                    memset(&upstream_calc, 0, sizeof(upstream_calc));
                    memset(&downstream_calc, 0, sizeof(downstream_calc));
                }
            }
            /* matter enable*/
            else if (UUIDsMatch((ChipBleUUID *) throughputService_MatterEnableUUID, charId))
            {
                if (dstBuf[0] == 0)
                {
                    // turn matter off
                    event.Type = AppEvent::kEventType_Throughput_Matter_Disable;
                }
                else
                {
                    // turn matter on
                    event.Type = AppEvent::kEventType_Throughput_Matter_Enable;
                }
            }
            /* dummy Rx*/
            else if (UUIDsMatch((ChipBleUUID *) throughputService_ThrougputRxUUID, charId))
            {
                if (downstream_flag == 0)
                {
                    // start timer
                    ClockP_start(ClockP_handle(&throughputClock));
                    downstream_flag = 1;
                }
                else
                {
                    downstream_calc.bytes = dstBufLen;
                    throughputGetInfo(&downstream_calc);
                    if (downstream_calc.message_count >= NUM_MESSAGES)
                    {
                        downstream_calc.message_count = 0;
                    }
                    else
                    {
                        downstream_calc.message_count++;
                    }
                }
            }

            if (event.Type != AppEvent::AppEventType::kEventType_None)
            {
                GetAppTask().PostEvent(&event);
            }
        }
#endif /* THROUGHPUT_SERVICE_ENABLE */

        /* Add extra service handling here */
    }
}

void AppoBLE_interface::HandleAppoBLEReadConfirmation(BLE_CONNECTION_OBJECT connObj, const ChipBleUUID * svcId,
                                                      const ChipBleUUID * charId, const uint8_t * dstBuf, uint16_t dstBufLen,
                                                      uint8_t status)
{
    /* Light service */
    if (UUIDsMatch((ChipBleUUID *) LightServiceServUUID, svcId))
    {
        if (UUIDsMatch((ChipBleUUID *) clientCharCfgUUID, charId))
        {
            /* CCC Request */
            if (dstBuf[0] == 1)
            {
                // CCC Write
            }
        }
    }
    /* Add extra service handling here */
}

void AppoBLE_interface::HandleAppoBLEIndicationConfirmation(BLE_CONNECTION_OBJECT connObj)
{
    // Add service specific Indiciation confirmation logic here
}

void AppoBLE_interface::HandleAppoBLEAdvChange(uint8_t advHandle, uint8_t status, AppoBLE_interface::AppoBLE_api_type_t advApiType)
{
    switch (advApiType)
    {
    case AppoBLE_interface::AppoBLE_ADV_ADD_UPDATE: {
        if ((status == SUCCESS) && (advHandle != 0))
        {
            PLAT_LOG("HandleAppoBLEAdvChange: Adv handle created: %d, API status: %d", advHandle, status);

            appoBLEadvHandle = advHandle;
            SetAppoBLEAdvInterval(appoBLEadvHandle, 35, 36);
            SetAppoBLEAdvertisingEnabled(appoBLEadvHandle, true);
        }
    }
    break;
    case AppoBLE_interface::AppoBLE_api_type_t::AppoBLE_ADV_REMOVE: {
    }
    break;
    case AppoBLE_interface::AppoBLE_api_type_t::AppoBLE_ADV_CHANGE_INTERVAL: {
        // Advertisement update failed due to BLE stack busy error, try again
        if ((status != SUCCESS) && (appoBLEadvHandle != ADV_INDEX_INVALID))
        {
            PLAT_LOG("HandleAppoBLEAdvChange: Adv handle attmempt: %d, API status: %d", advHandle, status);

            DMMPolicy_updateStackState(DMMPolicy_StackRole_BlePeripheral, DMMPOLICY_BLE_ADVERTISING);
            SetAppoBLEAdvertisingEnabled(appoBLEadvHandle, true);
        }
    }
    break;
    default:
        PLAT_LOG("Unknown API");
        break;
    }
}

uint8_t AppoBLE_interface::AppoBLE_GenericSetParameter(BLE_CONNECTION_OBJECT connObj, const ChipBleUUID * svcId,
                                                       const ChipBleUUID * charId, uint8 len, void * value, uint8_t taskId)
{
    /* Service ID to Matter CHIP BLE UUID Mapping  */
    uint8_t status = FAILURE;
    if (UUIDsMatch((ChipBleUUID *) LightServiceServUUID, svcId))
    {
        status = LightService_SetParameter(gatt_LightService_GetParamId(charId->bytes), len, value, taskId);
    }
    else if (UUIDsMatch((ChipBleUUID *) diagnosticServiceUUID, svcId))
    {
        status = DiagnosticService_SetParameter(diagnosticService_GetParamId(charId->bytes), len, value);
    }
#if THROUGHPUT_SERVICE_ENABLE
    else if (UUIDsMatch((ChipBleUUID *) throughputServiceUUID, svcId))
    {
        status = ThroughputService_SetParameter(throughputService_GetParamId(charId->bytes), len, value);
    }
#endif /* THROUGHPUT_SERVICE_ENABLE */
    /* Add service specific parameter handling here */
    return status;
}

uint8_t AppoBLE_interface::AppoBLE_GenericGetParameter(BLE_CONNECTION_OBJECT connObj, const ChipBleUUID * svcId,
                                                       const ChipBleUUID * charId, uint8 len, void * value)
{
    /* Service ID to Matter CHIP BLE UUID Mapping  */
    uint8_t status = FAILURE;
    if (UUIDsMatch((ChipBleUUID *) LightServiceServUUID, svcId))
    {
        status = LightService_GetParameter(gatt_LightService_GetParamId(charId->bytes), len, value);
    }
    else if (UUIDsMatch((ChipBleUUID *) diagnosticServiceUUID, svcId))
    {
        status = DiagnosticService_GetParameter(diagnosticService_GetParamId(charId->bytes), len, value);
    }
    else if (UUIDsMatch((ChipBleUUID *) throughputServiceUUID, svcId))
    {
        status = ThroughputService_GetParameter(throughputService_GetParamId(charId->bytes), len, value);
    }
    /* Add service specific parameter handling here */

    return status;
}

void AppoBLE_interface::HandleAppoBLEWriteConfirmation(BLE_CONNECTION_OBJECT connObj, const ChipBleUUID * svcId,
                                                       const ChipBleUUID * charId, uint8_t status)
{
// Add service specific Write confirmation logic here
#if THROUGHPUT_SERVICE_ENABLE
    if (UUIDsMatch((ChipBleUUID *) throughputServiceUUID, svcId))
    {
        vTaskDelay(1);
        if (upstream_flag == 1 && status == SUCCESS)
        {

            throughput_sequence_cpy(dummy_data, upstream_calc.packet_sequence_num);
            AppoBLE().SendAppoBLEWriteRequest(connObj, (ChipBleUUID *) throughputServiceUUID,
                                              (ChipBleUUID *) throughputService_ThroughputTxUUID, (uint8_t *) &dummy_data,
                                              sizeof(dummy_data));

            upstream_calc.packet_sequence_num++;
            upstream_calc.bytes = sizeof(dummy_data);
            throughputGetInfo(&upstream_calc);
            if (upstream_calc.message_count >= NUM_MESSAGES)
            {
                upstream_calc.message_count = 0;
            }
            else
            {
                upstream_calc.message_count++;
            }
        }
    }
#endif /* THROUGHPUT_SERVICE_ENABLE */
}

/* BLE Manager Execution Hooks */
void AppoBLE_interface::AppoBLEHook_Init(void)
{
    uint8_t * devAddr = GAP_GetDevAddress(FALSE);
    if (devAddr != NULL)
    {
        PLAT_LOG("BLE RP/Public Address: %x:%x:%x:%x:%x:%x", devAddr[5], devAddr[4], devAddr[3], devAddr[2], devAddr[1],
                 devAddr[0]);
    }

    LightService_AddService(GATT_ALL_SERVICES);
    LightService_RegisterAppCBs(&LightServiceAppCBs);

    DiagnosticService_AddService(GATT_ALL_SERVICES);
    DiagnosticService_RegisterAppCBs(&DiagnosticServiceAppCBs);

#if THROUGHPUT_SERVICE_ENABLE
    ThroughputService_AddService(GATT_ALL_SERVICES);
    ThroughputService_RegisterAppCBs(&throughputServiceAppCBs);
#endif /* THROUGHPUT_SERVICE_ENABLE */

#ifdef OAD_APP_OFFCHIP
    OADProfile_start(&App_OADCallback);
#endif

    // Diagnostic clock setup
    ClockP_Params myClockParams;
    ClockP_Params_init(&myClockParams);
    myClockParams.period = 0;
    ClockP_construct(&diagRefresh, myClockSwiFxn, 0, &myClockParams);

// throughput clock setup
#if THROUGHPUT_SERVICE_ENABLE
    ClockP_Params throughputClockParams;
    ClockP_Params_init(&throughputClockParams);
    throughputClockParams.period = 1000 * (1000 / ClockP_getSystemTickPeriod());
    ClockP_construct(&throughputClock, throughputClockSwiFxn, 0, &throughputClockParams);
    // TEST
    // fill data array
    for (int i = 4; i < 244; i++)
    {
        dummy_data[i] = 0xFF;
    }

    // initialize throughput calc struct
    upstream_calc.cbBufferFilled      = FALSE;
    upstream_calc.bytes_cb_index      = 0;
    upstream_calc.packet_sequence_num = 0;
    memset(upstream_calc.stored_bytes, 0, sizeof(upstream_calc.stored_bytes));
    memset(upstream_calc.bytes_cb, 0, sizeof(upstream_calc.bytes_cb));

    downstream_calc.cbBufferFilled      = FALSE;
    downstream_calc.bytes_cb_index      = 0;
    downstream_calc.packet_sequence_num = 0;
    memset(downstream_calc.stored_bytes, 0, sizeof(downstream_calc.stored_bytes));
    memset(downstream_calc.bytes_cb, 0, sizeof(downstream_calc.bytes_cb));
#endif /* THROUGHPUT_SERVICE_ENABLE */
    /* Add service registration here */
}

void AppoBLE_interface::AppoBLEHook_ProcessBleMgrEvt(const GenericQueuedEvt_t * pMsg)
{
    switch (pMsg->event)
    {
    case EXAMPLE_LIGHT_EVENT:
        PLAT_LOG("EXAMPLE_LIGHT_EVENT Processing...");
        break;
        /* Additional Service specific Processing can occur here */
    }
}

void AppoBLE_interface::AppoBLEHook_ProcessGapMessage(const gapEventHdr_t * pMsg)
{
    switch (pMsg->opcode)
    {
    case GAP_LINK_ESTABLISHED_EVENT: {
        DMMPolicy_updateStackState(DMMPolicy_StackRole_BlePeripheral, DMMPOLICY_BLE_CONNECTED);

        break;
    }
    case GAP_LINK_TERMINATED_EVENT: {
        /* Check if application advertisements were disabled due to max connections */
        if (!IsAppoBLEAdvertisingEnabled(appoBLEadvHandle))
        {
            SetAppoBLEAdvInterval(appoBLEadvHandle, 35, 36);
            SetAppoBLEAdvertisingEnabled(appoBLEadvHandle, true);
            DMMPolicy_updateStackState(DMMPolicy_StackRole_BlePeripheral, DMMPOLICY_BLE_ADVERTISING);
        }
#ifdef OAD_APP_OFFCHIP
        oadGAPConnEventHandler();
#endif
        break;
    }
    default:
        break;
    }
}

void AppoBLE_interface::AppoBLEHook_ProcessGattMessage(const gattMsgEvent_t * pMsg)
{
    // Add custom GATT HCI processing logic here
}

void AppoBLE_interface::AppoBLEHook_ProcessGapHCIEvt(const ICall_Hdr * pMsg)
{
    // Add custom GAP HCI processing logic here
}

/* Example API to erase all bonds via Generic AppoBLE function interface */
void eraseAllBonds(void * unused)
{
    bStatus_t status = GAPBondMgr_SetParameter(GAPBOND_ERASE_ALLBONDS, 0, NULL);
    if (status != SUCCESS)
    {
        PLAT_LOG("Failed to clear bond table");
    }
    else
    {
        PLAT_LOG("Cleared current bond table");
    }
}

void myClockSwiFxn(uintptr_t arg0)
{
    AppEvent event;
    event.Type = AppEvent::kEventType_Diagnostic_timer;
    GetAppTask().PostEvent(&event);
}

#ifdef OAD_APP_OFFCHIP

OADProfile_AppCommand_e App_OADCallback(OADProfile_App_Msg_e msg)
{
    static uint8_t oadInProgress = false;

    OADProfile_AppCommand_e cmd = OAD_PROFILE_PROCEED;
    switch (msg)
    {
    case OAD_PROFILE_MSG_REVOKE_IMG_HDR: {
        PLAT_LOG("App_OADCallback: Revoke image header");

        oadInProgress = false;

        cmd = OAD_PROFILE_PROCEED;
        break;
    }
    case OAD_PROFILE_MSG_NEW_IMG_IDENDIFY: {
        PLAT_LOG("App_OADCallback: New image identify");
        cmd = OAD_PROFILE_PROCEED;
        break;
    }
    case OAD_PROFILE_MSG_START_DOWNLOAD: {
        PLAT_LOG("App_OADCallback: Download new image");

        oadInProgress = true;
        DMMPolicy_updateStackState(DMMPolicy_StackRole_BlePeripheral, DMMPOLICY_BLE_HIGH_BANDWIDTH);
        matterThreadDisable();
        cmd = OAD_PROFILE_PROCEED;
        break;
    }
    case OAD_PROFILE_MSG_FINISH_DOWNLOAD: {
        PLAT_LOG("App_OADCallback: Download complete");
        cmd = OAD_PROFILE_PROCEED;
        break;
    }
    case OAD_PROFILE_MSG_RESET_REQ: {
        if (oadInProgress == true)
        {
            PLAT_LOG("App_OADCallback: Reset device");

            oadInProgress = false;

            cmd = OAD_PROFILE_PROCEED;
        }
        else
        {
            cmd = OAD_PROFILE_CANCEL;
        }

        break;
    }
    }
    return (cmd);
}
#endif
#ifdef THROUGHPUT_SERVICE_ENABLE
void throughputClockSwiFxn(uintptr_t arg0)
{
    AppEvent event;
    event.Type = AppEvent::kEventType_Throughput_calculate;
    GetAppTask().PostEvent(&event);

    upstream_calc.log_message_count += upstream_calc.message_count;
    downstream_calc.log_message_count += downstream_calc.message_count;

}

void throughputGetInfo(ThroughputRate_t * throughput_data)
{
    // store data and clear it for next entry
    throughput_data->stored_bytes[throughput_data->message_count] = throughput_data->bytes;
    throughput_data->bytes                                        = 0;
}

void throughput_sequence_cpy(uint8_t * packet, uint32_t sequence_num)
{
    packet[0] = (sequence_num & 0xff000000) >> 24;
    packet[1] = (sequence_num & 0x00ff0000) >> 16;
    packet[2] = (sequence_num & 0x0000ff00) >> 8;
    packet[3] = (sequence_num & 0x000000ff);
}
#endif
void matterThreadDisable()
{
    chip::DeviceLayer::ThreadStackMgr().LockThreadStack();
    PlatformMgr().LockChipStack();
    DMMPolicy_setBlockModeOn(DMMPolicy_StackRole_threadFtd);
}
void matterThreadEnable()
{
    PlatformMgr().UnlockChipStack();
    chip::DeviceLayer::ThreadStackMgr().UnlockThreadStack();
    DMMPolicy_setBlockModeOff(DMMPolicy_StackRole_threadFtd);
}