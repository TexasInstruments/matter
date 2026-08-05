/*
 *
 *    Copyright (c) 2026 Project CHIP Authors
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

#include <platform/ti/cc35xx/NetworkCommissioningWiFiDriver.h>

#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/KeyValueStoreManager.h>

#include <platform/ti/cc35xx/ti_wifi_structs.h>

#include <ti/drivers/net/wifi/wifi_host_driver/inc_adapt/osi_kernel.h>
// Use local copy of wlan_if.h with fix for profile API C linkage
// TODO: [MATTER-455] Remove this and use SDK header once the fix is applied in SDK
#include "wlan_if_cc35xx.h"

#include <cstring>

namespace chip {
namespace DeviceLayer {
namespace NetworkCommissioning {

namespace {
constexpr char kWiFiSSIDKey[]    = "cc35xx/wifi-ssid";
constexpr char kWiFiCredKey[]    = "cc35xx/wifi-credentials";
constexpr char kWiFiSecTypeKey[] = "cc35xx/wifi-sectype";
constexpr uint8_t kWiFiDeleteAllProfiles = 0xFF;
} // namespace

CC35XXWiFiDriver & CC35XXWiFiDriver::GetInstance()
{
    static CC35XXWiFiDriver sInstance;
    return sInstance;
}

bool CC35XXWiFiDriver::HasStagedNetwork() const
{
    return mStagingNetwork.ssidLen > 0;
}

CHIP_ERROR CC35XXWiFiDriver::Init()
{
    size_t bytesRead = 0;
    CHIP_ERROR err;

    err = PersistedStorage::KeyValueStoreMgr().Get(kWiFiSSIDKey, mSavedNetwork.ssid, sizeof(mSavedNetwork.ssid), &bytesRead);
    if (err != CHIP_NO_ERROR)
    {
        mSavedNetwork.ssidLen = 0;
        ChipLogProgress(DeviceLayer, "WiFi driver: no saved network; empty config");
        return CHIP_NO_ERROR;
    }
    mSavedNetwork.ssidLen = static_cast<uint8_t>(bytesRead);

    err = PersistedStorage::KeyValueStoreMgr().Get(kWiFiCredKey, mSavedNetwork.credentials,
                                                   sizeof(mSavedNetwork.credentials), &bytesRead);
    if (err == CHIP_NO_ERROR)
    {
        mSavedNetwork.credentialsLen = static_cast<uint8_t>(bytesRead);
    }
    else
    {
        mSavedNetwork.credentialsLen = 0;
        ChipLogError(DeviceLayer, "WiFi driver: KVS Get failed for credentials: %" CHIP_ERROR_FORMAT, err.Format());
    }

    uint32_t secType  = 0;
    size_t secTypeLen = 0;
    err               = PersistedStorage::KeyValueStoreMgr().Get(kWiFiSecTypeKey, &secType, sizeof(secType), &secTypeLen);
    mSavedNetwork.secType =
        (err == CHIP_NO_ERROR) ? secType : ((mSavedNetwork.credentialsLen > 0) ? WLAN_SEC_TYPE_WPA2_WPA3 : 0);

    mStagingNetwork = mSavedNetwork;
    mScanResultCount = 0;
    return CHIP_NO_ERROR;
}

NetworkIterator * CC35XXWiFiDriver::GetNetworks()
{
    return new WiFiNetworkIterator(this);
}

Status CC35XXWiFiDriver::AddOrUpdateNetwork(ByteSpan ssid, ByteSpan credentials, MutableCharSpan & outDebugText,
                                            uint8_t & outNetworkIndex)
{
    outDebugText.reduce_size(0);
    outNetworkIndex = 0;

    VerifyOrReturnValue(ssid.size() <= DeviceLayer::Internal::kMaxWiFiSSIDLength, Status::kBoundsExceeded);
    VerifyOrReturnValue(credentials.size() <= DeviceLayer::Internal::kMaxWiFiKeyLength, Status::kBoundsExceeded);

    memcpy(mStagingNetwork.ssid, ssid.data(), ssid.size());
    mStagingNetwork.ssidLen = static_cast<uint8_t>(ssid.size());

    memcpy(mStagingNetwork.credentials, credentials.data(), credentials.size());
    mStagingNetwork.credentialsLen = static_cast<uint8_t>(credentials.size());

    mStagingNetwork.secType = (credentials.size() > 0) ? WLAN_SEC_TYPE_WPA2_WPA3 : 0;

    return Status::kSuccess;
}

Status CC35XXWiFiDriver::RemoveNetwork(ByteSpan networkId, MutableCharSpan & outDebugText, uint8_t & outNetworkIndex)
{
    outDebugText.reduce_size(0);
    outNetworkIndex = 0;

    VerifyOrReturnValue(mStagingNetwork.ssidLen > 0 && networkId.size() == mStagingNetwork.ssidLen &&
                            memcmp(networkId.data(), mStagingNetwork.ssid, mStagingNetwork.ssidLen) == 0,
                        Status::kNetworkIDNotFound);

    mStagingNetwork = WiFiNetwork();
    return Status::kSuccess;
}

Status CC35XXWiFiDriver::ReorderNetwork(ByteSpan networkId, uint8_t index, MutableCharSpan & outDebugText)
{
    outDebugText.reduce_size(0);

    // Single-slot driver: only index 0 is valid
    VerifyOrReturnValue(index == 0, Status::kOutOfRange);
    VerifyOrReturnValue(mStagingNetwork.ssidLen > 0 && networkId.size() == mStagingNetwork.ssidLen &&
                            memcmp(networkId.data(), mStagingNetwork.ssid, mStagingNetwork.ssidLen) == 0,
                        Status::kNetworkIDNotFound);

    return Status::kSuccess;
}

void CC35XXWiFiDriver::ConnectNetwork(ByteSpan networkId, ConnectCallback * callback)
{
    VerifyOrReturn(mStagingNetwork.ssidLen > 0 && networkId.size() == mStagingNetwork.ssidLen &&
                       memcmp(networkId.data(), mStagingNetwork.ssid, mStagingNetwork.ssidLen) == 0,
                   callback->OnResult(Status::kNetworkIDNotFound, CharSpan(), 0));

    osi_SyncObjClear(&(app_CB.CON_CB.connectEventSyncObj));

    // Delete stale profiles from previous commissioning attempts or reverts that
    // may still be in flash. This ensures a clean slate — only the new SSID will
    // be in flash, so CME auto-connect targets the correct AP. autoPolicy=1 is
    // already persistent from _Init() boot path, so CME_MESSAGE_ID_PROFILE_ADDED
    // triggers auto-connect immediately via cmeProfileManagerConfigChange(CALLER4).
    DeleteWifiProfile();

    CHIP_ERROR err = AddWifiProfile();
    if (err != CHIP_NO_ERROR)
    {
        DeviceLayer::SystemLayer().ScheduleLambda([callback]() { callback->OnResult(Status::kUnknownError, CharSpan(), 0); });
        return;
    }

    // Register callback only if profile add succeeded.
    // Callback will be invoked when WLAN_EVENT_CONNECT fires (async via OnConnectResult).
    mpConnectCallback = callback;
}

CHIP_ERROR CC35XXWiFiDriver::CommitConfiguration()
{
    VerifyOrReturnError(mStagingNetwork.ssidLen > 0, CHIP_NO_ERROR);

    CHIP_ERROR err = PersistedStorage::KeyValueStoreMgr().Put(kWiFiSSIDKey, mStagingNetwork.ssid, mStagingNetwork.ssidLen);
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(DeviceLayer, "WiFi driver: KVS Put failed: %" CHIP_ERROR_FORMAT, err.Format());
        return err;
    }

    err = PersistedStorage::KeyValueStoreMgr().Put(kWiFiCredKey, mStagingNetwork.credentials, mStagingNetwork.credentialsLen);
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(DeviceLayer, "WiFi driver: KVS Put failed: %" CHIP_ERROR_FORMAT, err.Format());
        return err;
    }

    err = PersistedStorage::KeyValueStoreMgr().Put(kWiFiSecTypeKey, &mStagingNetwork.secType, sizeof(mStagingNetwork.secType));
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(DeviceLayer, "WiFi driver: KVS Put failed: %" CHIP_ERROR_FORMAT, err.Format());
        return err;
    }

    mSavedNetwork = mStagingNetwork;
    return CHIP_NO_ERROR;
}

CHIP_ERROR CC35XXWiFiDriver::RevertConfiguration()
{
    mStagingNetwork = mSavedNetwork;

    // Sync flash profile back to committed credentials. ConnectNetwork() may
    // have already replaced the profile with staged creds; without this,
    // autoPolicy=1 would keep reconnecting to the uncommitted AP.
    DeleteWifiProfile();
    if (mSavedNetwork.ssidLen > 0)
        (void) AddWifiProfile();

    return CHIP_NO_ERROR;
}

void CC35XXWiFiDriver::OnConnectResult(bool connected)
{
    if (mpConnectCallback == nullptr)
    {
        return;
    }

    auto * cb         = mpConnectCallback;
    mpConnectCallback = nullptr;

    if (connected)
    {
        ChipLogProgress(DeviceLayer, "WiFi driver: connect succeeded");
    }
    else
    {
        ChipLogProgress(DeviceLayer, "WiFi driver: connect failed");
    }

    auto status = connected ? Status::kSuccess : Status::kAuthFailure;
    DeviceLayer::SystemLayer().ScheduleLambda([cb, status]() { cb->OnResult(status, CharSpan(), 0); });
}

CHIP_ERROR CC35XXWiFiDriver::AddWifiProfile()
{
    WlanSecParams_t secParams;
    secParams.Type   = static_cast<uint8_t>(mStagingNetwork.secType);
    secParams.Key    = reinterpret_cast<int8_t *>(mStagingNetwork.credentials);
    secParams.KeyLen = mStagingNetwork.credentialsLen;

    // Only one profile exists at a time (DeleteWifiProfile cleans up stale ones),
    // so Priority value doesn't affect connect order. Using 7 as an arbitrary choice.
    int ret = Wlan_ProfileAdd(reinterpret_cast<const signed char *>(mStagingNetwork.ssid),
                              mStagingNetwork.ssidLen,
                              nullptr, &secParams, nullptr,
                              /*Priority=*/7, /*Hidden=*/0, /*Options=*/0);
    if (ret < 0)
    {
        ChipLogError(DeviceLayer, "WiFi driver: Wlan_ProfileAdd failed: %d", ret);
        return CHIP_ERROR_INTERNAL;
    }
    ChipLogProgress(DeviceLayer, "WiFi driver: Wlan_ProfileAdd succeeded, index: %d", ret);
    return CHIP_NO_ERROR;
}

void CC35XXWiFiDriver::DeleteWifiProfile()
{
    int ret = Wlan_ProfileDel(kWiFiDeleteAllProfiles);
    if (ret != 0)
    {
        ChipLogError(DeviceLayer, "WiFi driver: Wlan_ProfileDel(ALL) failed: %d", ret);
    }
    else
    {
        ChipLogProgress(DeviceLayer, "WiFi driver: Wlan_ProfileDel(ALL) succeeded");
    }
}

void CC35XXWiFiDriver::ClearNetworkConfig()
{
    ChipLogProgress(DeviceLayer, "WiFi driver: clearing network config");
    
    DeleteWifiProfile();

    // Reset both networks to empty state (ssidLen=0, credentialsLen=0, secType=0)
    mSavedNetwork = WiFiNetwork();
    mStagingNetwork = WiFiNetwork();

    CHIP_ERROR err = PersistedStorage::KeyValueStoreMgr().Delete(kWiFiSSIDKey);
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(DeviceLayer, "WiFi driver: KVS Delete failed: %" CHIP_ERROR_FORMAT, err.Format());
    }

    err = PersistedStorage::KeyValueStoreMgr().Delete(kWiFiCredKey);
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(DeviceLayer, "WiFi driver: KVS Delete failed: %" CHIP_ERROR_FORMAT, err.Format());
    }

    err = PersistedStorage::KeyValueStoreMgr().Delete(kWiFiSecTypeKey);
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(DeviceLayer, "WiFi driver: KVS Delete failed: %" CHIP_ERROR_FORMAT, err.Format());
    }
}

void CC35XXWiFiDriver::ScanNetworks(ByteSpan ssid, ScanCallback * callback)
{
    // Prevent concurrent scans
    if (mScanInProgress)
    {
        ChipLogError(DeviceLayer, "WiFi driver: scan already in progress, rejecting request");
        if (callback != nullptr)
        {
            DeviceLayer::SystemLayer().ScheduleLambda(
                [callback]() { callback->OnFinished(Status::kUnknownError, CharSpan(), nullptr); });
        }
        return;
    }

    // Set scan running status bit
    SET_STATUS_BIT(app_CB.Status, STATUS_BIT_SCAN_RUNNING);

    // Store callback for when scan results arrive
    mpScanCallback = callback;
    mScanResultCount = 0;
    mScanInProgress = true;

    // Initialize scan parameters
    scanCommon_t scanParams;
    os_memset(&scanParams, 0x0, sizeof(scanCommon_t));
    scanParams.Band = BAND_SEL_BOTH;

    // Start WiFi scan in STA mode
    int32_t ret = Wlan_Scan(WLAN_ROLE_STA, &scanParams, CC35XXWiFiDriver::kMaxScanResults);
    if (ret != 0)
    {
        // Clear scan running status bit on error
        CLR_STATUS_BIT(app_CB.Status, STATUS_BIT_SCAN_RUNNING);
        mScanInProgress = false;
        mpScanCallback = nullptr;
        ChipLogError(DeviceLayer, "WiFi driver: Wlan_Scan failed: %" PRId32, ret);
        if (callback != nullptr)
        {
            DeviceLayer::SystemLayer().ScheduleLambda(
                [callback]() { callback->OnFinished(Status::kOtherConnectionFailure, CharSpan(), nullptr); });
        }
        return;
    }
}

size_t CC35XXWiFiDriver::WiFiNetworkIterator::Count()
{
    return mDriver->mStagingNetwork.ssidLen > 0 ? 1 : 0;
}

bool CC35XXWiFiDriver::WiFiNetworkIterator::Next(Network & item)
{
    if (mExhausted || mDriver->mStagingNetwork.ssidLen == 0)
    {
        return false;
    }
    mExhausted = true;

    memcpy(item.networkID, mDriver->mStagingNetwork.ssid, mDriver->mStagingNetwork.ssidLen);
    item.networkIDLen = mDriver->mStagingNetwork.ssidLen;
    item.connected    = IS_STA_CONNECTED(app_CB.Status);
    return true;
}

} // namespace NetworkCommissioning
} // namespace DeviceLayer
} // namespace chip
