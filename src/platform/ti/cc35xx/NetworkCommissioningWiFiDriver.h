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

#pragma once

#include <platform/NetworkCommissioning.h>

namespace chip {
namespace DeviceLayer {
namespace NetworkCommissioning {

class CC35XXWiFiDriver final : public WiFiDriver
{
public:
    static CC35XXWiFiDriver & GetInstance();

    // BaseDriver
    uint8_t GetMaxNetworks() override { return 1; }
    NetworkIterator * GetNetworks() override;

    // WirelessDriver
    CHIP_ERROR CommitConfiguration() override { return CHIP_NO_ERROR; }
    CHIP_ERROR RevertConfiguration() override { return CHIP_NO_ERROR; }
    uint8_t GetScanNetworkTimeoutSeconds() override { return 30; }
    uint8_t GetConnectNetworkTimeoutSeconds() override { return 60; }

    Status RemoveNetwork(ByteSpan networkId, MutableCharSpan & outDebugText, uint8_t & outNetworkIndex) override;
    Status ReorderNetwork(ByteSpan networkId, uint8_t index, MutableCharSpan & outDebugText) override;
    void ConnectNetwork(ByteSpan networkId, ConnectCallback * callback) override;

    // WiFiDriver
    Status AddOrUpdateNetwork(ByteSpan ssid, ByteSpan credentials, MutableCharSpan & outDebugText,
                              uint8_t & outNetworkIndex) override;
    void ScanNetworks(ByteSpan ssid, ScanCallback * callback) override;

private:
    CC35XXWiFiDriver()  = default;
    ~CC35XXWiFiDriver() = default;

    class WiFiNetworkIterator final : public NetworkIterator
    {
    public:
        explicit WiFiNetworkIterator(CC35XXWiFiDriver * driver) : mDriver(driver) {}
        size_t Count() override;
        bool Next(Network & item) override;
        void Release() override { delete this; }

    private:
        CC35XXWiFiDriver * mDriver;
        bool mExhausted = false;
    };
};

} // namespace NetworkCommissioning
} // namespace DeviceLayer
} // namespace chip
