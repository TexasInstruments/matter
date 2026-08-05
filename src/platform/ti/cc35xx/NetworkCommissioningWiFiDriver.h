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

#pragma once

#include <platform/NetworkCommissioning.h>
#include "wlan_if_cc35xx.h"

namespace chip {
namespace DeviceLayer {
namespace NetworkCommissioning {

class CC35XXWiFiDriver final : public WiFiDriver
{
public:
    static constexpr uint8_t kMaxWiFiNetworks                  = 1;
    static constexpr uint8_t kWiFiScanNetworksTimeOutSeconds   = 10;
    static constexpr uint8_t kWiFiConnectNetworkTimeoutSeconds = 20;
    static constexpr uint8_t kMaxScanResults                   = WLAN_MAX_SCAN_COUNT;
    
    // Consolidated accessor for staged network credentials
    struct StagedNetworkCredentials {
        const uint8_t* ssid;
        size_t ssidLen;
        const uint8_t* credentials;
        size_t credentialsLen;
        uint32_t secType;
    };

    static CC35XXWiFiDriver & GetInstance();

    // Load saved credentials from KVS into mStagingNetwork. Called from ConnectivityManagerImpl::_Init().
    CHIP_ERROR Init();

    // BaseDriver
    uint8_t GetMaxNetworks() override { return kMaxWiFiNetworks; }
    NetworkIterator * GetNetworks() override;

    // WirelessDriver
    CHIP_ERROR CommitConfiguration() override;
    CHIP_ERROR RevertConfiguration() override;
    uint8_t GetScanNetworkTimeoutSeconds() override { return 30; }
    uint8_t GetConnectNetworkTimeoutSeconds() override { return 60; }

    Status RemoveNetwork(ByteSpan networkId, MutableCharSpan & outDebugText, uint8_t & outNetworkIndex) override;
    Status ReorderNetwork(ByteSpan networkId, uint8_t index, MutableCharSpan & outDebugText) override;
    void ConnectNetwork(ByteSpan networkId, ConnectCallback * callback) override;

    // WiFiDriver
    Status AddOrUpdateNetwork(ByteSpan ssid, ByteSpan credentials, MutableCharSpan & outDebugText,
                              uint8_t & outNetworkIndex) override;
    void ScanNetworks(ByteSpan ssid, ScanCallback * callback) override;

    bool HasStagedNetwork() const;

    // Called from WlanStackEventHandler on WLAN_EVENT_CONNECT / auth+assoc rejection
    void OnConnectResult(bool connected);

    // Clears staged network from memory and KVS
    void ClearNetworkConfig();

    // WiFi profile management — called from ConnectivityManagerImpl boot path
    // Stores mStagingNetwork as preferred network in flash
    CHIP_ERROR AddWifiProfile();
    // Removes all preferred networks (WLAN_DEL_ALL_PROFILES)
    void DeleteWifiProfile();

    // Scan result accessors for ConnectivityManagerImpl event handler
    size_t GetScanResultCount() const { return mScanResultCount; }
    uint8_t GetScanResultsBuffer(WiFiScanResponse *& outBuffer)
    {
        outBuffer = mScanResults;
        return kMaxScanResults;
    }
    void SetScanInProgress(bool inProgress) { mScanInProgress = inProgress; }
    void SetScanCallback(ScanCallback * callback) { mpScanCallback = callback; }
    ScanCallback * GetScanCallback() const { return mpScanCallback; }
    void ResetScanResults() { mScanResultCount = 0; }
    void IncrementScanResultCount() { mScanResultCount++; }
    WiFiScanResponseIterator * SetupAndGetScanIterator()
    {
        mScanIterator.Reset(mScanResults, mScanResultCount);
        return &mScanIterator;
    }

private:
    class CC35XXScanResponseIterator final : public WiFiScanResponseIterator
    {
    public:
        void Reset(WiFiScanResponse * results, size_t count)
        {
            mResults = results;
            mCount   = count;
            mIndex   = 0;
        }
        size_t Count() override { return mCount; }
        bool Next(WiFiScanResponse & item) override
        {
            if (mIndex >= mCount)
                return false;
            item = mResults[mIndex++];
            return true;
        }
        void Release() override {}

    private:
        WiFiScanResponse * mResults = nullptr;
        size_t mCount               = 0;
        size_t mIndex               = 0;
    };

    CC35XXWiFiDriver()  = default;
    ~CC35XXWiFiDriver() = default;

    struct WiFiNetwork
    {
        uint8_t ssid[DeviceLayer::Internal::kMaxWiFiSSIDLength];
        uint8_t ssidLen = 0;
        uint8_t credentials[DeviceLayer::Internal::kMaxWiFiKeyLength];
        uint8_t credentialsLen = 0;
        uint32_t secType = 0;
    };

    WiFiNetwork mSavedNetwork;
    WiFiNetwork mStagingNetwork;
    ConnectCallback * mpConnectCallback = nullptr;
    ScanCallback * mpScanCallback       = nullptr;
    WiFiScanResponse mScanResults[kMaxScanResults];
    size_t mScanResultCount;
    bool mScanInProgress                = false;
    CC35XXScanResponseIterator mScanIterator;

    class WiFiNetworkIterator final : public NetworkIterator
    {
    public:
        explicit WiFiNetworkIterator(CC35XXWiFiDriver * driver) : mDriver(driver) {}
        size_t Count() override;
        bool Next(Network & item) override;
        void Release() override { delete this; }
        ~WiFiNetworkIterator() = default;

    private:
        CC35XXWiFiDriver * mDriver;
        bool mExhausted = false;
    };
};

} // namespace NetworkCommissioning
} // namespace DeviceLayer
} // namespace chip
