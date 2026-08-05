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

/**
 *    @file
 *          Provides an implementation of the BLEManager singleton object
 *          for the Texas Instruments CC35XX platform using NimBLE.
 */

#pragma once

#include <platform/internal/BLEManager.h>

#if CHIP_DEVICE_CONFIG_ENABLE_CHIPOBLE

#include <ble/Ble.h>
#include <lib/core/CHIPError.h>
#include <lib/support/BitFlags.h>
#include <system/SystemLayer.h>

// NimBLE host API
#include <host/ble_gap.h>
#include <host/ble_gatt.h>
#include <host/ble_hs.h>
#include <nimble/nimble_npl.h>

// TI OSI sync objects
#include <ti/drivers/net/wifi/wifi_host_driver/inc_adapt/osi_kernel.h>

namespace chip {
namespace DeviceLayer {
namespace Internal {

class BLEManagerImpl final : public BLEManager,
                             private Ble::BleLayer,
                             private Ble::BlePlatformDelegate,
                             private Ble::BleApplicationDelegate
{
    // Allow the BLEManager interface class to delegate method calls to
    // the implementation methods provided by this class.
    friend BLEManager;

    // Allow the PlatformManager to call helper methods.
    friend class PlatformManagerImpl;

public:
    static BLEManagerImpl sInstance;

public:
    BLEManagerImpl() : mServiceMode(ConnectivityManager::kCHIPoBLEServiceMode_Enabled) {}

private:
    enum class Flags : uint16_t
    {
        kAdvertisingEnabled       = 0x0001, // CHIPoBLE advertising enabled
        kFastAdvertisingEnabled   = 0x0002, // Fast advertising interval active
        kAdvertising              = 0x0004, // Currently advertising
        kAdvertisingRefreshNeeded = 0x0008, // Adv data/params need refresh
        kBLELayerInitialized      = 0x0010, // NimBLE host + controller ready
        kUseCustomDeviceName      = 0x0020, // Custom name set via SetDeviceName
    };

    // Maximum one simultaneous BLE commissioning connection
    static constexpr uint16_t kMaxConnections      = 1;
    static constexpr uint8_t  kMaxDeviceNameLength = 16;
    // Extended advertising instance 0
    static constexpr uint8_t  kAdvInstance         = 0;
    // NimBLE host task runs below the CHIP task (priority 4) to avoid starving it
    static constexpr uint8_t  kNimBLEHostTaskPriority = 3;
    static constexpr uint16_t kNimBLEHostTaskStackSize = 4096;

    // ===== BLEManager interface methods =====
    CHIP_ERROR _Init();
    void       _Shutdown();
    bool       _IsAdvertisingEnabled();
    CHIP_ERROR _SetAdvertisingEnabled(bool val);
    bool       _IsAdvertising();
    CHIP_ERROR _SetAdvertisingMode(BLEAdvertisingMode mode);
    CHIP_ERROR _GetDeviceName(char * buf, size_t bufSize);
    CHIP_ERROR _SetDeviceName(const char * deviceName);
    uint16_t   _NumConnections();
    void       _OnPlatformEvent(const ChipDeviceEvent * event);
    Ble::BleLayer * _GetBleLayer();

    // ===== BlePlatformDelegate =====
    CHIP_ERROR SendIndication(BLE_CONNECTION_OBJECT conId, const Ble::ChipBleUUID * svcId, const Ble::ChipBleUUID * charId,
                              System::PacketBufferHandle data) override;
    CHIP_ERROR SendWriteRequest(BLE_CONNECTION_OBJECT conId, const Ble::ChipBleUUID * svcId, const Ble::ChipBleUUID * charId,
                                System::PacketBufferHandle pBuf) override;
    CHIP_ERROR SubscribeCharacteristic(BLE_CONNECTION_OBJECT conId, const Ble::ChipBleUUID * svcId,
                                       const Ble::ChipBleUUID * charId) override;
    CHIP_ERROR UnsubscribeCharacteristic(BLE_CONNECTION_OBJECT conId, const Ble::ChipBleUUID * svcId,
                                         const Ble::ChipBleUUID * charId) override;
    CHIP_ERROR CloseConnection(BLE_CONNECTION_OBJECT conId) override;
    uint16_t   GetMTU(BLE_CONNECTION_OBJECT conId) const override;

    // ===== BleApplicationDelegate =====
    void NotifyChipConnectionClosed(BLE_CONNECTION_OBJECT conId) override;

    // ===== Internal helpers =====
    CHIP_ERROR InitBLELayer();
    CHIP_ERROR PrepareAdvertisingConfig();
    CHIP_ERROR BuildAdvertisingPayload(uint8_t * advData, uint8_t & advIndex, uint8_t * srData, uint8_t & srIndex);
    void       SetAdvertisingParameters(struct ble_gap_ext_adv_params & advParams);
    CHIP_ERROR StartAdvertising();
    CHIP_ERROR StopAdvertising();

    CHIP_ERROR SetSubscribed(uint16_t conHandle);
    void       UnsetSubscribed(uint16_t conHandle);
    bool       IsSubscribed(uint16_t conHandle);

    // Advertising state machine — runs on CHIP task
    static void DriveBLEState(intptr_t arg);
    void        DriveBLEState();

    // NimBLE host task entry point
    static void HandleNimBLEHostTask(void * param);

    // NimBLE host callbacks (called on NimBLE host task; marshal to CHIP task)
    static void HandleNimBLEReset(int reason);
    static void HandleNimBLESync();

    // GAP/GATT event callbacks (NimBLE task → CHIP task via ScheduleWork)
    static int ble_svr_gap_event(struct ble_gap_event * event, void * arg);
    static int gatt_svr_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                                   struct ble_gatt_access_ctxt * ctxt, void * arg);

    // Per-event handlers (run on CHIP task)
    static void HandleGAPConnect(intptr_t connHandle);
    static void HandleGAPDisconnect(intptr_t arg);
    static void HandleRXCharWrite(intptr_t arg);
    static void HandleTXCharCCCDWrite(intptr_t arg);
    static void HandleTXComplete(intptr_t arg);

    // GATT service definition (implemented in .cpp)
    static const struct ble_gatt_svc_def kCHIPoBLEGATTSvc;

    // ===== State =====
    BitFlags<Flags>      mFlags;
    char                 mDeviceName[kMaxDeviceNameLength + 1];
    CHIPoBLEServiceMode  mServiceMode;
    uint16_t             mNumGAPCons;
    uint16_t             mSubscribedConIds[kMaxConnections];
    uint16_t             mRXCharAttrHandle;
    uint16_t             mTXCharCCCDAttrHandle;

    // NimBLE host task and init sync
    struct ble_npl_task mHostTask;
    OsiSyncObj_t        mHostSyncObj;
};

// ===== Inline implementations =====

inline Ble::BleLayer * BLEManagerImpl::_GetBleLayer()
{
    return this;
}

inline bool BLEManagerImpl::_IsAdvertisingEnabled()
{
    return mFlags.Has(Flags::kAdvertisingEnabled);
}

inline bool BLEManagerImpl::_IsAdvertising()
{
    return mFlags.Has(Flags::kAdvertising);
}

inline BLEManager & BLEMgr()
{
    return BLEManagerImpl::sInstance;
}

inline BLEManagerImpl & BLEMgrImpl()
{
    return BLEManagerImpl::sInstance;
}

} // namespace Internal
} // namespace DeviceLayer
} // namespace chip

#endif // CHIP_DEVICE_CONFIG_ENABLE_CHIPOBLE
