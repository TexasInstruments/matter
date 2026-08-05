/*
 *
 *    Copyright (c) 2024 Project CHIP Authors
 *    All rights reserved.
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

/* this file behaves like a config.h, comes first */
#include <platform/internal/CHIPDeviceLayerInternal.h>

#if CHIP_DEVICE_CONFIG_ENABLE_CHIPOBLE

#include <ble/Ble.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/SafeInt.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/CommissionableDataProvider.h>
#include <platform/DeviceInstanceInfoProvider.h>
#include <platform/internal/BLEManager.h>
#include <setup_payload/AdditionalDataPayloadGenerator.h>
#include <system/SystemTimer.h>

#include <platform/ti/cc35xx/BLEManagerImpl.h>

// NimBLE host API
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "host/ble_hs.h"
#include "host/ble_hs_mbuf.h"
#include "host/ble_uuid.h"
#include "host/util/util.h"
#include "nimble/nimble_npl.h"
#include "nimble/nimble_port.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

// TI BLE interface and OSI (ble_if.h has no extern "C" guard — wrap explicitly)
#include <ti/drivers/net/wifi/wifi_host_driver/inc_adapt/osi_kernel.h>
extern "C" {
#include <ti/net/ble_interface/inc_common/ble_if.h>
}

extern "C" {
void ble_store_config_init(void);
// Wire NimBLE's HCI to the CC35XX NAB transport bridge
void ble_transport_ll_init(void);
}

#define MAX_ADV_DATA_LEN 31

#define CHIP_ADV_DATA_TYPE_FLAGS        0x01
#define CHIP_ADV_DATA_FLAGS             0x06
#define CHIP_ADV_DATA_TYPE_SERVICE_DATA 0x16
#define CHIP_ADV_DATA_TYPE_COMPLETE_NAME 0x09

using namespace ::chip;
using namespace ::chip::Ble;

namespace chip {
namespace DeviceLayer {
namespace Internal {

namespace {

// CHIPoBLE service UUID: 0xFFF6
const ble_uuid16_t kCHIPoBLEServiceUUID16 = { BLE_UUID_TYPE_16, 0xFFF6 };

// RX characteristic: 18ee2ef5-263d-4559-959f-4f9c429f9d11
const ble_uuid128_t kCHIPoBLECharRXUUID = {
    BLE_UUID_TYPE_128, { 0x11, 0x9D, 0x9F, 0x42, 0x9C, 0x4F, 0x9F, 0x95, 0x59, 0x45, 0x3D, 0x26, 0xF5, 0x2E, 0xEE, 0x18 }
};

// TX characteristic: 18ee2ef5-263d-4559-959f-4f9c429f9d12
const ble_uuid128_t kCHIPoBLECharTXUUID = {
    BLE_UUID_TYPE_128, { 0x12, 0x9D, 0x9F, 0x42, 0x9C, 0x4F, 0x9F, 0x95, 0x59, 0x45, 0x3D, 0x26, 0xF5, 0x2E, 0xEE, 0x18 }
};

// Packed service data for CHIPoBLE advertisement
struct CC35XXChipServiceData
{
    uint8_t ServiceUUID[2];
    ChipBLEDeviceIdentificationInfo DeviceIdInfo;
};

} // namespace

// ===== Static member definitions =====

BLEManagerImpl BLEManagerImpl::sInstance;

const struct ble_gatt_svc_def BLEManagerImpl::kCHIPoBLEGATTSvc = {
    .type            = BLE_GATT_SVC_TYPE_PRIMARY,
    .uuid            = (ble_uuid_t *) (&kCHIPoBLEServiceUUID16),
    .characteristics = (struct ble_gatt_chr_def[]){
        {
            .uuid       = (ble_uuid_t *) (&kCHIPoBLECharRXUUID),
            .access_cb  = gatt_svr_chr_access,
            .flags      = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP,
            .val_handle = &sInstance.mRXCharAttrHandle,
        },
        {
            .uuid      = (ble_uuid_t *) (&kCHIPoBLECharTXUUID),
            .access_cb = gatt_svr_chr_access,
            .flags     = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_INDICATE,
            .val_handle = &sInstance.mTXCharCCCDAttrHandle,
        },
        { 0 }, // terminator
    }
};

// ===== BLEManager interface =====

CHIP_ERROR BLEManagerImpl::_Init()
{
    CHIP_ERROR err;

    mServiceMode = ConnectivityManager::kCHIPoBLEServiceMode_Enabled;
    mFlags.ClearAll();
    mFlags.Set(Flags::kAdvertisingEnabled, CHIP_DEVICE_CONFIG_CHIPOBLE_ENABLE_ADVERTISING_AUTOSTART);
    mFlags.Set(Flags::kFastAdvertisingEnabled, true);
    mNumGAPCons = 0;
    mRXCharAttrHandle     = 0;
    mTXCharCCCDAttrHandle = 0;
    mDeviceName[0] = 0;
    for (uint16_t i = 0; i < kMaxConnections; i++)
    {
        mSubscribedConIds[i] = BLE_CONNECTION_UNINITIALIZED;
    }

    // Initialize the CHIP BleLayer.
    err = BleLayer::Init(this, this, &DeviceLayer::SystemLayer());
    SuccessOrExit(err);

    // Kick off the BLE state machine on the CHIP task.
    PlatformMgr().ScheduleWork(DriveBLEState, 0);

exit:
    return err;
}

void BLEManagerImpl::_Shutdown()
{
    if (mFlags.Has(Flags::kAdvertising))
    {
        ble_gap_ext_adv_stop(kAdvInstance);
    }
    mFlags.ClearAll();
    mServiceMode = ConnectivityManager::kCHIPoBLEServiceMode_Disabled;
}

CHIP_ERROR BLEManagerImpl::_SetAdvertisingEnabled(bool val)
{
    VerifyOrReturnError(mServiceMode != ConnectivityManager::kCHIPoBLEServiceMode_NotSupported,
                        CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE);
    mFlags.Set(Flags::kFastAdvertisingEnabled, val);
    mFlags.Set(Flags::kAdvertisingRefreshNeeded, 1);
    mFlags.Set(Flags::kAdvertisingEnabled, val);
    PlatformMgr().ScheduleWork(DriveBLEState, 0);
    return CHIP_NO_ERROR;
}

CHIP_ERROR BLEManagerImpl::_SetAdvertisingMode(BLEAdvertisingMode mode)
{
    switch (mode)
    {
    case BLEAdvertisingMode::kFastAdvertising:
        mFlags.Set(Flags::kFastAdvertisingEnabled, true);
        break;
    case BLEAdvertisingMode::kSlowAdvertising:
        mFlags.Set(Flags::kFastAdvertisingEnabled, false);
        break;
    default:
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
    mFlags.Set(Flags::kAdvertisingRefreshNeeded);
    PlatformMgr().ScheduleWork(DriveBLEState, 0);
    return CHIP_NO_ERROR;
}

CHIP_ERROR BLEManagerImpl::_GetDeviceName(char * buf, size_t bufSize)
{
    if (strlen(mDeviceName) >= bufSize)
    {
        return CHIP_ERROR_BUFFER_TOO_SMALL;
    }
    strcpy(buf, mDeviceName);
    return CHIP_NO_ERROR;
}

CHIP_ERROR BLEManagerImpl::_SetDeviceName(const char * deviceName)
{
    VerifyOrReturnError(mServiceMode != ConnectivityManager::kCHIPoBLEServiceMode_NotSupported,
                        CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE);

    if (deviceName != nullptr && deviceName[0] != 0)
    {
        VerifyOrReturnError(strlen(deviceName) < kMaxDeviceNameLength, CHIP_ERROR_INVALID_ARGUMENT);
        strcpy(mDeviceName, deviceName);
        mFlags.Set(Flags::kUseCustomDeviceName);
    }
    else
    {
        mDeviceName[0] = 0;
        mFlags.Clear(Flags::kUseCustomDeviceName);
    }
    return CHIP_NO_ERROR;
}

uint16_t BLEManagerImpl::_NumConnections()
{
    uint16_t numCons = 0;
    for (uint16_t i = 0; i < kMaxConnections; i++)
    {
        if (mSubscribedConIds[i] != BLE_CONNECTION_UNINITIALIZED)
        {
            numCons++;
        }
    }
    return numCons;
}

void BLEManagerImpl::_OnPlatformEvent(const ChipDeviceEvent * event)
{
    switch (event->Type)
    {
    case DeviceEventType::kCHIPoBLESubscribe:
        // On CC35XX, NimBLE fires the subscribe event before the HandshakeRequest
        // write arrives, so the first subscribe has no BTP endpoint yet.
        // Only post kCHIPoBLEConnectionEstablished when HandleSubscribeReceived
        // finds an endpoint (returns true), which drives the BTP handshake.
        if (HandleSubscribeReceived(event->CHIPoBLESubscribe.ConId, &CHIP_BLE_SVC_ID, &Ble::CHIP_BLE_CHAR_2_UUID))
        {
            ChipDeviceEvent connEvent;
            connEvent.Type = DeviceEventType::kCHIPoBLEConnectionEstablished;
            PlatformMgr().PostEventOrDie(&connEvent);
        }
        break;

    case DeviceEventType::kCHIPoBLEUnsubscribe:
        HandleUnsubscribeReceived(event->CHIPoBLEUnsubscribe.ConId, &CHIP_BLE_SVC_ID, &Ble::CHIP_BLE_CHAR_2_UUID);
        break;

    case DeviceEventType::kCHIPoBLEWriteReceived:
        HandleWriteReceived(event->CHIPoBLEWriteReceived.ConId, &CHIP_BLE_SVC_ID, &Ble::CHIP_BLE_CHAR_1_UUID,
                            PacketBufferHandle::Adopt(event->CHIPoBLEWriteReceived.Data));
        break;

    case DeviceEventType::kCHIPoBLEIndicateConfirm:
        HandleIndicationConfirmation(event->CHIPoBLEIndicateConfirm.ConId, &CHIP_BLE_SVC_ID, &Ble::CHIP_BLE_CHAR_2_UUID);
        break;

    case DeviceEventType::kCHIPoBLEConnectionError:
        HandleConnectionError(event->CHIPoBLEConnectionError.ConId, event->CHIPoBLEConnectionError.Reason);
        break;

    case DeviceEventType::kServiceProvisioningChange:
    case DeviceEventType::kWiFiConnectivityChange:
        mFlags.Set(Flags::kAdvertisingRefreshNeeded);
        DriveBLEState();
        break;

    default:
        break;
    }
}

// ===== BlePlatformDelegate =====

CHIP_ERROR BLEManagerImpl::SendIndication(BLE_CONNECTION_OBJECT conId, const ChipBleUUID * svcId, const ChipBleUUID * charId,
                                          System::PacketBufferHandle data)
{
    VerifyOrReturnError(IsSubscribed(conId), CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(CanCastTo<uint16_t>(data->DataLength()), CHIP_ERROR_MESSAGE_TOO_LONG);

    struct os_mbuf * om = ble_hs_mbuf_from_flat(data->Start(), static_cast<uint16_t>(data->DataLength()));
    VerifyOrReturnError(om != nullptr, CHIP_ERROR_NO_MEMORY);

    int rc = ble_gatts_indicate_custom(conId, mTXCharCCCDAttrHandle, om);
    if (rc != 0)
    {
        ChipLogError(DeviceLayer, "ble_gatts_indicate_custom failed: %d", rc);
        return CHIP_ERROR(ChipError::Range::kPlatform, rc);
    }
    return CHIP_NO_ERROR;
}

CHIP_ERROR BLEManagerImpl::SendWriteRequest(BLE_CONNECTION_OBJECT conId, const ChipBleUUID * svcId, const ChipBleUUID * charId,
                                            System::PacketBufferHandle pBuf)
{
    // CC35XX operates as BLE peripheral only
    return CHIP_ERROR_NOT_IMPLEMENTED;
}

CHIP_ERROR BLEManagerImpl::SubscribeCharacteristic(BLE_CONNECTION_OBJECT conId, const ChipBleUUID * svcId,
                                                   const ChipBleUUID * charId)
{
    return CHIP_ERROR_NOT_IMPLEMENTED;
}

CHIP_ERROR BLEManagerImpl::UnsubscribeCharacteristic(BLE_CONNECTION_OBJECT conId, const ChipBleUUID * svcId,
                                                     const ChipBleUUID * charId)
{
    return CHIP_ERROR_NOT_IMPLEMENTED;
}

CHIP_ERROR BLEManagerImpl::CloseConnection(BLE_CONNECTION_OBJECT conId)
{
    ChipLogProgress(DeviceLayer, "Closing BLE connection (con %u)", conId);

    int rc = ble_gap_terminate(conId, BLE_ERR_REM_USER_CONN_TERM);
    if (rc != 0 && rc != BLE_HS_ENOTCONN)
    {
        ChipLogError(DeviceLayer, "ble_gap_terminate() failed: %d", rc);
        return CHIP_ERROR(ChipError::Range::kPlatform, rc);
    }

    mFlags.Set(Flags::kAdvertisingRefreshNeeded);
    PlatformMgr().ScheduleWork(DriveBLEState, 0);
    return CHIP_NO_ERROR;
}

uint16_t BLEManagerImpl::GetMTU(BLE_CONNECTION_OBJECT conId) const
{
    return ble_att_mtu(conId);
}

// ===== BleApplicationDelegate =====

void BLEManagerImpl::NotifyChipConnectionClosed(BLE_CONNECTION_OBJECT conId)
{
    ChipLogDetail(DeviceLayer, "CHIPoBLE connection closed notification (con %u)", conId);
    CloseConnection(conId);
}

// ===== Internal helpers =====

CHIP_ERROR BLEManagerImpl::SetSubscribed(uint16_t conHandle)
{
    for (uint16_t i = 0; i < kMaxConnections; i++)
    {
        if (mSubscribedConIds[i] == BLE_CONNECTION_UNINITIALIZED)
        {
            mSubscribedConIds[i] = conHandle;
            return CHIP_NO_ERROR;
        }
        if (mSubscribedConIds[i] == conHandle)
        {
            return CHIP_NO_ERROR; // already subscribed
        }
    }
    return CHIP_ERROR_NO_MEMORY;
}

void BLEManagerImpl::UnsetSubscribed(uint16_t conHandle)
{
    for (uint16_t i = 0; i < kMaxConnections; i++)
    {
        if (mSubscribedConIds[i] == conHandle)
        {
            mSubscribedConIds[i] = BLE_CONNECTION_UNINITIALIZED;
            return;
        }
    }
}

bool BLEManagerImpl::IsSubscribed(uint16_t conHandle)
{
    for (uint16_t i = 0; i < kMaxConnections; i++)
    {
        if (mSubscribedConIds[i] == conHandle)
        {
            return true;
        }
    }
    return false;
}

// ===== BLE stack init =====

CHIP_ERROR BLEManagerImpl::InitBLELayer()
{
    ChipLogProgress(DeviceLayer, "BLE: initializing NimBLE host");

    // Create sync object — signaled by HandleNimBLESync when host-controller link is up
    int rc = osi_SyncObjCreate(&mHostSyncObj);
    if (rc != OSI_OK)
    {
        ChipLogError(DeviceLayer, "BLE: osi_SyncObjCreate failed: %d", rc);
        return CHIP_ERROR_INTERNAL;
    }

    // Register BLE with the WLAN shared NAB transport — must be after Wlan_Start()
    BleIf_OpenTransport();

    // Enable BLE controller hardware
    rc = BleIf_EnableBLE();
    if (rc != 0)
    {
        ChipLogError(DeviceLayer, "BLE: BleIf_EnableBLE failed: %d", rc);
        osi_SyncObjDelete(&mHostSyncObj);
        return CHIP_ERROR_INTERNAL;
    }

    // Initialize NimBLE port (memory pools, event queues)
    nimble_port_init();

    // Wire NimBLE's HCI to the CC35XX NAB mailbox bridge
    ble_transport_ll_init();

    // Initialize GAP and GATT service infrastructure
    ble_svc_gap_init();
    ble_svc_gatt_init();

    // Register CHIPoBLE GATT service
    static const struct ble_gatt_svc_def kGATTSvcs[] = { kCHIPoBLEGATTSvc, { 0 } };
    rc                                                 = ble_gatts_count_cfg(kGATTSvcs);
    if (rc != 0)
    {
        ChipLogError(DeviceLayer, "BLE: ble_gatts_count_cfg failed: %d", rc);
        goto exit;
    }
    rc = ble_gatts_add_svcs(kGATTSvcs);
    if (rc != 0)
    {
        ChipLogError(DeviceLayer, "BLE: ble_gatts_add_svcs failed: %d", rc);
        goto exit;
    }

    // Configure NimBLE host callbacks
    ble_hs_cfg.reset_cb        = HandleNimBLEReset;
    ble_hs_cfg.sync_cb         = HandleNimBLESync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;
    ble_hs_cfg.sm_bonding      = 0; // no SMP bonding for Matter CHIPoBLE
    ble_hs_cfg.sm_mitm         = 0;

    // Initialize bond store (persistent; satisfies MYNEWT_VAL_BLE_STORE_CONFIG_PERSIST=1)
    ble_store_config_init();

    // Create NimBLE host task (runs nimble_port_run; never returns)
    rc = ble_npl_task_init(&mHostTask, "nimble_host", HandleNimBLEHostTask, nullptr, kNimBLEHostTaskPriority,
                           BLE_NPL_TIME_FOREVER, nullptr, kNimBLEHostTaskStackSize);
    if (rc != 0)
    {
        ChipLogError(DeviceLayer, "BLE: ble_npl_task_init failed: %d", rc);
        goto exit;
    }

    // Block until HandleNimBLESync fires (host-controller synchronized)
    rc = osi_SyncObjWait(&mHostSyncObj, OSI_WAIT_FOR_SECOND);
    if (rc != OSI_OK)
    {
        ChipLogError(DeviceLayer, "BLE: timeout waiting for NimBLE sync (%d)", rc);
        osi_SyncObjDelete(&mHostSyncObj);
        return CHIP_ERROR_TIMEOUT;
    }
    osi_SyncObjDelete(&mHostSyncObj);

    mFlags.Set(Flags::kBLELayerInitialized);
    ChipLogProgress(DeviceLayer, "BLE: NimBLE host initialized");
    return CHIP_NO_ERROR;

exit:
    osi_SyncObjDelete(&mHostSyncObj);
    return CHIP_ERROR_INTERNAL;
}

// ===== Advertising =====

CHIP_ERROR BLEManagerImpl::PrepareAdvertisingConfig()
{
    CHIP_ERROR err;

    uint16_t discriminator;
    err = GetCommissionableDataProvider()->GetSetupDiscriminator(discriminator);
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(DeviceLayer, "BLE: GetSetupDiscriminator failed: %" CHIP_ERROR_FORMAT, err.Format());
        return err;
    }

    if (!mFlags.Has(Flags::kUseCustomDeviceName))
    {
        snprintf(mDeviceName, sizeof(mDeviceName), "%s%04u", CHIP_DEVICE_CONFIG_BLE_DEVICE_NAME_PREFIX, discriminator);
        mDeviceName[kMaxDeviceNameLength] = 0;
    }

    ble_svc_gap_device_name_set(mDeviceName);

    return CHIP_NO_ERROR;
}

CHIP_ERROR BLEManagerImpl::BuildAdvertisingPayload(uint8_t * advData, uint8_t & advIndex, uint8_t * srData, uint8_t & srIndex)
{
    ChipBLEDeviceIdentificationInfo deviceIdInfo;
    ReturnErrorOnFailure(ConfigurationMgr().GetBLEDeviceIdentificationInfo(deviceIdInfo));

    // Flags AD (3 bytes)
    advData[advIndex++] = 0x02;
    advData[advIndex++] = CHIP_ADV_DATA_TYPE_FLAGS;
    advData[advIndex++] = CHIP_ADV_DATA_FLAGS;

    // Service Data AD: UUID 0xFFF6 + ChipBLEDeviceIdentificationInfo
    constexpr uint8_t kServiceDataTypeSize = 1;
    advData[advIndex++]                    = static_cast<uint8_t>(kServiceDataTypeSize + sizeof(CC35XXChipServiceData));
    advData[advIndex++]                    = CHIP_ADV_DATA_TYPE_SERVICE_DATA;
    advData[advIndex++]                    = static_cast<uint8_t>(kCHIPoBLEServiceUUID16.value & 0xFF);
    advData[advIndex++]                    = static_cast<uint8_t>((kCHIPoBLEServiceUUID16.value >> 8) & 0xFF);
    if (advIndex + sizeof(deviceIdInfo) > MAX_ADV_DATA_LEN)
    {
        return CHIP_ERROR_OUTBOUND_MESSAGE_TOO_BIG;
    }
    memcpy(&advData[advIndex], &deviceIdInfo, sizeof(deviceIdInfo));
    advIndex = static_cast<uint8_t>(advIndex + sizeof(deviceIdInfo));

    // Scan response: device name
    size_t nameLen = strlen(mDeviceName);
    if (nameLen > 0 && (nameLen + 2) <= MAX_ADV_DATA_LEN)
    {
        srData[srIndex++] = static_cast<uint8_t>(nameLen + 1);
        srData[srIndex++] = CHIP_ADV_DATA_TYPE_COMPLETE_NAME;
        memcpy(&srData[srIndex], mDeviceName, nameLen);
        srIndex = static_cast<uint8_t>(srIndex + nameLen);
    }

    return CHIP_NO_ERROR;
}

void BLEManagerImpl::SetAdvertisingParameters(struct ble_gap_ext_adv_params & advParams)
{
    advParams.legacy_pdu    = 1;
    advParams.connectable   = (mNumGAPCons < kMaxConnections) ? 1 : 0;
    advParams.scannable     = 1;
    advParams.own_addr_type = BLE_OWN_ADDR_PUBLIC;

    if (mFlags.Has(Flags::kFastAdvertisingEnabled))
    {
        advParams.itvl_min = CHIP_DEVICE_CONFIG_BLE_FAST_ADVERTISING_INTERVAL_MIN;
        advParams.itvl_max = CHIP_DEVICE_CONFIG_BLE_FAST_ADVERTISING_INTERVAL_MAX;
    }
    else
    {
        advParams.itvl_min = CHIP_DEVICE_CONFIG_BLE_SLOW_ADVERTISING_INTERVAL_MIN;
        advParams.itvl_max = CHIP_DEVICE_CONFIG_BLE_SLOW_ADVERTISING_INTERVAL_MAX;
    }
}

CHIP_ERROR BLEManagerImpl::StartAdvertising()
{
    int rc;

    ReturnErrorOnFailure(PrepareAdvertisingConfig());

    uint8_t advData[MAX_ADV_DATA_LEN];
    uint8_t advIndex = 0;
    uint8_t srData[MAX_ADV_DATA_LEN];
    uint8_t srIndex = 0;

    ReturnErrorOnFailure(BuildAdvertisingPayload(advData, advIndex, srData, srIndex));

    struct ble_gap_ext_adv_params advParams;
    memset(&advParams, 0, sizeof(advParams));
    SetAdvertisingParameters(advParams);

    // Stop existing adv instance if running to ensure fresh state
    if (ble_gap_ext_adv_active(kAdvInstance))
    {
        ble_gap_ext_adv_stop(kAdvInstance);
    }

    rc = ble_gap_ext_adv_configure(kAdvInstance, &advParams, nullptr, ble_svr_gap_event, nullptr);
    if (rc != 0)
    {
        ChipLogError(DeviceLayer, "BLE: ble_gap_ext_adv_configure failed: %d", rc);
        return CHIP_ERROR(ChipError::Range::kPlatform, rc);
    }

    struct os_mbuf * advMbuf = os_msys_get_pkthdr(advIndex, 0);
    VerifyOrReturnError(advMbuf != nullptr, CHIP_ERROR_NO_MEMORY);
    rc = os_mbuf_append(advMbuf, advData, advIndex);
    if (rc != 0)
    {
        os_mbuf_free_chain(advMbuf);
        return CHIP_ERROR_NO_MEMORY;
    }
    rc = ble_gap_ext_adv_set_data(kAdvInstance, advMbuf);
    if (rc != 0)
    {
        ChipLogError(DeviceLayer, "BLE: ble_gap_ext_adv_set_data failed: %d", rc);
        return CHIP_ERROR(ChipError::Range::kPlatform, rc);
    }

    if (srIndex > 0)
    {
        struct os_mbuf * srMbuf = os_msys_get_pkthdr(srIndex, 0);
        if (srMbuf != nullptr)
        {
            if (os_mbuf_append(srMbuf, srData, srIndex) == 0)
            {
                ble_gap_ext_adv_rsp_set_data(kAdvInstance, srMbuf);
            }
            else
            {
                os_mbuf_free_chain(srMbuf);
            }
        }
    }

    rc = ble_gap_ext_adv_start(kAdvInstance, 0, 0);
    if (rc != 0)
    {
        ChipLogError(DeviceLayer, "BLE: ble_gap_ext_adv_start failed: %d", rc);
        return CHIP_ERROR(ChipError::Range::kPlatform, rc);
    }

    ChipLogProgress(DeviceLayer, "CHIPoBLE advertising started (%s, itvl ~%" PRIu32 " ms)",
                    mFlags.Has(Flags::kFastAdvertisingEnabled) ? "fast" : "slow",
                    (uint32_t)(((uint32_t) advParams.itvl_min) * 10) / 16);
    return CHIP_NO_ERROR;
}

CHIP_ERROR BLEManagerImpl::StopAdvertising()
{
    if (ble_gap_ext_adv_active(kAdvInstance))
    {
        int rc = ble_gap_ext_adv_stop(kAdvInstance);
        if (rc != 0)
        {
            ChipLogError(DeviceLayer, "BLE: ble_gap_ext_adv_stop failed: %d", rc);
            return CHIP_ERROR(ChipError::Range::kPlatform, rc);
        }
    }
    return CHIP_NO_ERROR;
}

// ===== DriveBLEState — advertising state machine =====

void BLEManagerImpl::DriveBLEState(intptr_t /* arg */)
{
    sInstance.DriveBLEState();
}

void BLEManagerImpl::DriveBLEState()
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    // Initialize the BLE layer on first entry with advertising requested
    if (mServiceMode == ConnectivityManager::kCHIPoBLEServiceMode_Enabled && !mFlags.Has(Flags::kBLELayerInitialized))
    {
        err = InitBLELayer();
        SuccessOrExit(err);
    }

    if (mServiceMode == ConnectivityManager::kCHIPoBLEServiceMode_Enabled && mFlags.Has(Flags::kAdvertisingEnabled))
    {
        // Only advertise if we have room for more connections
        if (mNumGAPCons < kMaxConnections)
        {
            // Start or refresh advertising
            if (!mFlags.Has(Flags::kAdvertising) || mFlags.Has(Flags::kAdvertisingRefreshNeeded))
            {
                err = StartAdvertising();
                SuccessOrExit(err);

                mFlags.Clear(Flags::kAdvertisingRefreshNeeded);

                if (!mFlags.Has(Flags::kAdvertising))
                {
                    mFlags.Set(Flags::kAdvertising);

                    ChipDeviceEvent advChange;
                    advChange.Type                             = DeviceEventType::kCHIPoBLEAdvertisingChange;
                    advChange.CHIPoBLEAdvertisingChange.Result = kActivity_Started;
                    PlatformMgr().PostEventOrDie(&advChange);
                }
            }
        }
        else if (mFlags.Has(Flags::kAdvertising))
        {
            // We're at max connections; stop advertising
            err = StopAdvertising();
            SuccessOrExit(err);

            mFlags.Clear(Flags::kAdvertising);
        }
    }
    else if (mFlags.Has(Flags::kAdvertising))
    {
        err = StopAdvertising();
        SuccessOrExit(err);

        mFlags.Clear(Flags::kAdvertising);
        mFlags.Set(Flags::kFastAdvertisingEnabled, true);

        ChipLogProgress(DeviceLayer, "CHIPoBLE advertising stopped (BLE disabled)");

        ChipDeviceEvent advChange;
        advChange.Type                             = DeviceEventType::kCHIPoBLEAdvertisingChange;
        advChange.CHIPoBLEAdvertisingChange.Result = kActivity_Stopped;
        PlatformMgr().PostEventOrDie(&advChange);
    }

exit:
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(DeviceLayer, "BLE DriveBLEState error: %" CHIP_ERROR_FORMAT, err.Format());
        mServiceMode = ConnectivityManager::kCHIPoBLEServiceMode_Disabled;
    }
}

// ===== NimBLE host task =====

void BLEManagerImpl::HandleNimBLEHostTask(void * param)
{
    nimble_port_run(); // event loop; never returns
}

// ===== NimBLE host callbacks (NimBLE host task context) =====

void BLEManagerImpl::HandleNimBLEReset(int reason)
{
    ChipLogError(DeviceLayer, "BLE: NimBLE host reset; reason=%d", reason);
}

void BLEManagerImpl::HandleNimBLESync()
{
    // Infer and confirm our public address
    uint8_t ownAddrType;
    ble_hs_util_ensure_addr(0);
    ble_hs_id_infer_auto(0, &ownAddrType);

    ChipLogProgress(DeviceLayer, "BLE: NimBLE host-controller synced (addr type %u)", ownAddrType);

    // Unblock the init wait in InitBLELayer (running on CHIP task)
    osi_SyncObjSignal(&sInstance.mHostSyncObj);
}

// ===== GAP event callback (NimBLE host task context) =====

int BLEManagerImpl::ble_svr_gap_event(struct ble_gap_event * event, void * arg)
{
    switch (event->type)
    {
    case BLE_GAP_EVENT_CONNECT: {
        uint16_t connHandle = event->connect.conn_handle;
        if (event->connect.status == 0)
        {
            ChipLogProgress(DeviceLayer, "BLE: GAP connected (con %u)", connHandle);
            PlatformMgr().ScheduleWork(HandleGAPConnect, static_cast<intptr_t>(connHandle));
        }
        else
        {
            ChipLogError(DeviceLayer, "BLE: GAP connect failed status=%d", event->connect.status);
            // Restart advertising
            PlatformMgr().ScheduleWork(DriveBLEState, 0);
        }
        break;
    }

    case BLE_GAP_EVENT_DISCONNECT: {
        // Pack conn_handle + reason into intptr_t
        intptr_t disconnArg = (static_cast<intptr_t>(event->disconnect.conn.conn_handle) << 8) |
                              static_cast<intptr_t>(event->disconnect.reason);
        PlatformMgr().ScheduleWork(HandleGAPDisconnect, disconnArg);
        break;
    }

    case BLE_GAP_EVENT_SUBSCRIBE: {
        // Must copy the event since it lives on the NimBLE stack frame
        auto * ev = new ble_gap_event(*event);
        if (ev != nullptr)
        {
            if (PlatformMgr().ScheduleWork(HandleTXCharCCCDWrite, reinterpret_cast<intptr_t>(ev)) != CHIP_NO_ERROR)
            {
                delete ev;
            }
        }
        break;
    }

    case BLE_GAP_EVENT_NOTIFY_TX:
        // NimBLE fires status==0 as an intermediate "queued to controller" event
        // before the real confirmation (BLE_HS_EDONE==11) or failure arrives.
        // Skipping status==0 matches the ESP32 NimBLE implementation and prevents
        // a spurious kCHIPoBLEConnectionError from closing the BTP endpoint.
        if (event->notify_tx.status != 0)
        {
            auto * ev = new ble_gap_event(*event);
            if (ev != nullptr)
            {
                if (PlatformMgr().ScheduleWork(HandleTXComplete, reinterpret_cast<intptr_t>(ev)) != CHIP_NO_ERROR)
                {
                    delete ev;
                }
            }
        }
        break;

    case BLE_GAP_EVENT_MTU:
        ChipLogProgress(DeviceLayer, "BLE: MTU update (con %u, mtu %u)", event->mtu.conn_handle, event->mtu.value);
        break;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        // Advertising instance stopped — refresh state
        PlatformMgr().ScheduleWork(DriveBLEState, 0);
        break;

    default:
        break;
    }
    return 0;
}

// ===== GATT access callback (NimBLE host task context) =====

int BLEManagerImpl::gatt_svr_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                                        struct ble_gatt_access_ctxt * ctxt, void * arg)
{
    if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR && attr_handle == sInstance.mRXCharAttrHandle)
    {
        uint16_t dataLen = OS_MBUF_PKTLEN(ctxt->om);
        if (dataLen == 0)
        {
            return 0;
        }

        // Copy received bytes into a Matter PacketBuffer and post to CHIP task
        PacketBufferHandle buf = System::PacketBufferHandle::New(dataLen, 0);
        if (buf.IsNull())
        {
            return BLE_ATT_ERR_INSUFFICIENT_RES;
        }
        ble_hs_mbuf_to_flat(ctxt->om, buf->Start(), dataLen, nullptr);
        buf->SetDataLength(dataLen);

        ChipDeviceEvent event;
        event.Type                        = DeviceEventType::kCHIPoBLEWriteReceived;
        event.CHIPoBLEWriteReceived.ConId = conn_handle;
        event.CHIPoBLEWriteReceived.Data  = std::move(buf).UnsafeRelease();
        PlatformMgr().PostEventOrDie(&event);
        return 0;
    }

    // TX characteristic: read not supported
    return BLE_ATT_ERR_READ_NOT_PERMITTED;
}

// ===== Per-event handlers (CHIP task context) =====

void BLEManagerImpl::HandleGAPConnect(intptr_t connHandle)
{
    sInstance.mNumGAPCons++;

    // Stop advertising immediately when at max connections (don't wait for DriveBLEState)
    if (sInstance.mNumGAPCons >= kMaxConnections && sInstance.mFlags.Has(Flags::kAdvertising))
    {
        sInstance.StopAdvertising();
        sInstance.mFlags.Clear(Flags::kAdvertising);
        ChipLogProgress(DeviceLayer, "CHIPoBLE advertising stopped (max connections)");
    }
}

void BLEManagerImpl::HandleGAPDisconnect(intptr_t arg)
{
    uint16_t connHandle = static_cast<uint16_t>((arg >> 8) & 0xFFFF);

    ChipLogProgress(DeviceLayer, "BLE: GAP disconnected (con %u reason 0x%02x)", connHandle,
                    static_cast<unsigned>(arg & 0xFF));

    if (sInstance.mNumGAPCons > 0)
    {
        sInstance.mNumGAPCons--;
    }
    sInstance.UnsetSubscribed(connHandle);

    ChipDeviceEvent event;
    event.Type                           = DeviceEventType::kCHIPoBLEConnectionError;
    event.CHIPoBLEConnectionError.ConId  = connHandle;
    event.CHIPoBLEConnectionError.Reason = BLE_ERROR_REMOTE_DEVICE_DISCONNECTED;
    PlatformMgr().PostEventOrDie(&event);

    // Restart advertising if commissioning window is still open (kAdvertisingEnabled set by Matter stack)
    if (sInstance.mNumGAPCons < kMaxConnections && sInstance.mFlags.Has(Flags::kAdvertisingEnabled))
    {
        sInstance.mFlags.Set(Flags::kAdvertisingRefreshNeeded);
        PlatformMgr().ScheduleWork(DriveBLEState, 0);
    }
}

void BLEManagerImpl::HandleRXCharWrite(intptr_t arg)
{
    // Handled inline in gatt_svr_chr_access to avoid extra allocation; no-op here
}

void BLEManagerImpl::HandleTXCharCCCDWrite(intptr_t arg)
{
    auto * gapEvent = reinterpret_cast<struct ble_gap_event *>(arg);
    if (gapEvent == nullptr)
    {
        return;
    }

    bool indicationsEnabled = (gapEvent->subscribe.cur_indicate != 0);
    uint16_t connHandle     = gapEvent->subscribe.conn_handle;
    delete gapEvent;

    ChipLogProgress(DeviceLayer, "BLE: CCCD write con=%u indicate=%d", connHandle, indicationsEnabled);

    if (indicationsEnabled)
    {
        sInstance.SetSubscribed(connHandle);
    }
    else
    {
        sInstance.UnsetSubscribed(connHandle);
    }

    ChipDeviceEvent event;
    event.Type = indicationsEnabled ? DeviceEventType::kCHIPoBLESubscribe : DeviceEventType::kCHIPoBLEUnsubscribe;
    event.CHIPoBLESubscribe.ConId = connHandle;
    PlatformMgr().PostEventOrDie(&event);
}

void BLEManagerImpl::HandleTXComplete(intptr_t arg)
{
    auto * gapEvent = reinterpret_cast<struct ble_gap_event *>(arg);
    if (gapEvent == nullptr)
    {
        return;
    }

    uint16_t connHandle = gapEvent->notify_tx.conn_handle;
    int      status     = gapEvent->notify_tx.status;
    delete gapEvent;

    if (status == BLE_HS_EDONE)
    {
        ChipDeviceEvent event;
        event.Type                          = DeviceEventType::kCHIPoBLEIndicateConfirm;
        event.CHIPoBLEIndicateConfirm.ConId = connHandle;
        PlatformMgr().PostEventOrDie(&event);
    }
    else
    {
        ChipDeviceEvent event;
        event.Type                           = DeviceEventType::kCHIPoBLEConnectionError;
        event.CHIPoBLEConnectionError.ConId  = connHandle;
        event.CHIPoBLEConnectionError.Reason = BLE_ERROR_CHIPOBLE_PROTOCOL_ABORT;
        PlatformMgr().PostEventOrDie(&event);
    }
}

} // namespace Internal
} // namespace DeviceLayer
} // namespace chip

#endif // CHIP_DEVICE_CONFIG_ENABLE_CHIPOBLE
