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

#include <platform/ti/cc35xx/NetworkCommissioningWiFiDriver.h>

#include <lib/support/CodeUtils.h>
#include <platform/CHIPDeviceLayer.h>

#include <platform/ti/cc35xx/ti_wifi_structs.h>

#include <cstring>

// Must match AP_SSID in ConnectivityManagerImpl.cpp
#define CC35XX_AP_SSID "WLP_Test_2p4Ghz"

namespace chip {
namespace DeviceLayer {
namespace NetworkCommissioning {

CC35XXWiFiDriver & CC35XXWiFiDriver::GetInstance()
{
    static CC35XXWiFiDriver sInstance;
    return sInstance;
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
    // CC35XX uses hardcoded AP credentials; network configuration via Matter is not supported
    return Status::kBoundsExceeded;
}

Status CC35XXWiFiDriver::RemoveNetwork(ByteSpan networkId, MutableCharSpan & outDebugText, uint8_t & outNetworkIndex)
{
    outDebugText.reduce_size(0);
    outNetworkIndex = 0;
    return Status::kNetworkIDNotFound;
}

Status CC35XXWiFiDriver::ReorderNetwork(ByteSpan networkId, uint8_t index, MutableCharSpan & outDebugText)
{
    outDebugText.reduce_size(0);
    return Status::kNetworkIDNotFound;
}

void CC35XXWiFiDriver::ConnectNetwork(ByteSpan networkId, ConnectCallback * callback)
{
    constexpr char kSsid[]    = CC35XX_AP_SSID;
    constexpr size_t kSsidLen = sizeof(kSsid) - 1;

    if (networkId.size() != kSsidLen || memcmp(networkId.data(), kSsid, kSsidLen) != 0)
    {
        callback->OnResult(Status::kNetworkIDNotFound, CharSpan(), 0);
        return;
    }

    // CC35XX is already connected at boot with hardcoded credentials
    DeviceLayer::SystemLayer().ScheduleLambda([callback]() { callback->OnResult(Status::kSuccess, CharSpan(), 0); });
}

void CC35XXWiFiDriver::ScanNetworks(ByteSpan ssid, ScanCallback * callback)
{
    // Scanning is performed at boot init; on-demand scan is not supported
    if (callback != nullptr)
    {
        DeviceLayer::SystemLayer().ScheduleLambda(
            [callback]() { callback->OnFinished(Status::kSuccess, CharSpan(), nullptr); });
    }
}

size_t CC35XXWiFiDriver::WiFiNetworkIterator::Count()
{
    return 1;
}

bool CC35XXWiFiDriver::WiFiNetworkIterator::Next(Network & item)
{
    if (mExhausted)
    {
        return false;
    }
    mExhausted = true;

    constexpr char kSsid[]    = CC35XX_AP_SSID;
    constexpr size_t kSsidLen = sizeof(kSsid) - 1;

    memcpy(item.networkID, kSsid, kSsidLen);
    item.networkIDLen = static_cast<uint8_t>(kSsidLen);
    item.connected    = IS_STA_CONNECTED(app_CB.Status);
    return true;
}

} // namespace NetworkCommissioning
} // namespace DeviceLayer
} // namespace chip
