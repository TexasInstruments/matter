/*
 *
 *    Copyright (c) 2021-2022 Project CHIP Authors
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
 *          Provides an implementation of the DiagnosticDataProvider object
 *          for cc35xx platform.
 */

#include <lib/support/CHIPMemString.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/DiagnosticDataProvider.h>
#include <platform/internal/CHIPDeviceLayerInternal.h>

#include <FreeRTOS.h>
#include <portable.h>
#include <platform/ti/cc35xx/DiagnosticDataProviderImpl.h>

#include <lwip/netif.h>
#include <network_lwip.h>
#include "wlan_if_cc35xx.h"
#include <platform/ti/cc35xx/ti_wifi_structs.h>

namespace chip {
namespace DeviceLayer {

DiagnosticDataProviderImpl & DiagnosticDataProviderImpl::GetDefaultInstance()
{
    static DiagnosticDataProviderImpl sInstance;
    return sInstance;
}

/*
 * The following Heap stats are compiled values done by the BGET heap implementation.
 * See https://www.fourmilab.ch/bget/
 */
CHIP_ERROR DiagnosticDataProviderImpl::GetCurrentHeapFree(uint64_t & currentHeapFree)
{
    currentHeapFree = xPortGetFreeHeapSize();
    return CHIP_NO_ERROR;
}

CHIP_ERROR DiagnosticDataProviderImpl::GetCurrentHeapUsed(uint64_t & currentHeapUsed)
{
    currentHeapUsed = configTOTAL_HEAP_SIZE - xPortGetFreeHeapSize();
    return CHIP_NO_ERROR;
}

CHIP_ERROR DiagnosticDataProviderImpl::GetCurrentHeapHighWatermark(uint64_t & currentHeapHighWatermark)
{
    currentHeapHighWatermark = configTOTAL_HEAP_SIZE - xPortGetMinimumEverFreeHeapSize();
    return CHIP_NO_ERROR;
}

// General Diagnostics Getters

CHIP_ERROR DiagnosticDataProviderImpl::GetRebootCount(uint16_t & rebootCount)
{
    uint32_t count = 0;
    CHIP_ERROR err = ConfigurationMgr().GetRebootCount(count);

    if (err == CHIP_NO_ERROR)
    {
        VerifyOrReturnError(count <= UINT16_MAX, CHIP_ERROR_INVALID_INTEGER_VALUE);
        rebootCount = static_cast<uint16_t>(count);
    }

    return err;
}

CHIP_ERROR DiagnosticDataProviderImpl::GetBootReason(BootReasonType & bootReason)
{
    // TODO: CC35XX does not expose a reset source API equivalent to SysCtrlResetSourceGet().
    // Map to correct BootReasonType once a TI SimpleLink reset source API is identified.
    bootReason = BootReasonType::kUnspecified;
    return CHIP_NO_ERROR;
}

CHIP_ERROR DiagnosticDataProviderImpl::GetUpTime(uint64_t & upTime)
{
    System::Clock::Timestamp currentTime = System::SystemClock().GetMonotonicTimestamp();
    System::Clock::Timestamp startTime   = PlatformMgrImpl().GetStartTime();

    if (currentTime >= startTime)
    {
        upTime = std::chrono::duration_cast<System::Clock::Seconds64>(currentTime - startTime).count();
        return CHIP_NO_ERROR;
    }

    return CHIP_ERROR_INVALID_TIME;
}

CHIP_ERROR DiagnosticDataProviderImpl::GetTotalOperationalHours(uint32_t & totalOperationalHours)
{
    return ConfigurationMgr().GetTotalOperationalHours(totalOperationalHours);
}

CHIP_ERROR DiagnosticDataProviderImpl::GetNetworkInterfaces(NetworkInterface ** netifpp)
{
    NetworkInterface * ifp = new NetworkInterface();

    constexpr char kWifiIfName[] = "wl0";
    Platform::CopyString(ifp->Name, kWifiIfName);
    ifp->name = CharSpan::fromCharString(ifp->Name);

    ifp->type          = app::Clusters::GeneralDiagnostics::InterfaceTypeEnum::kWiFi;
    ifp->isOperational = IS_STA_CONNECTED(app_CB.Status) && GET_STATUS_BIT(app_CB.Status, STATUS_BIT_IP_ACQUIRED);
    ifp->offPremiseServicesReachableIPv4.SetNull();
    ifp->offPremiseServicesReachableIPv6.SetNull();

    struct netif * staNetif = static_cast<struct netif *>(network_get_sta_if());
    if (staNetif != nullptr)
    {
        memcpy(ifp->MacAddress, staNetif->hwaddr, staNetif->hwaddr_len);
        ifp->hardwareAddress = ByteSpan(ifp->MacAddress, staNetif->hwaddr_len);

        const ip4_addr_t * ip4 = netif_ip4_addr(staNetif);
        if (ip4 != nullptr && ip4->addr != 0)
        {
            memcpy(ifp->Ipv4AddressesBuffer[0], &ip4->addr, kMaxIPv4AddrSize);
            ifp->Ipv4AddressSpans[0] = ByteSpan(ifp->Ipv4AddressesBuffer[0], kMaxIPv4AddrSize);
            ifp->IPv4Addresses       = app::DataModel::List<ByteSpan>(ifp->Ipv4AddressSpans, 1);
        }

        uint8_t ipv6Count = 0;
        for (uint8_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES && ipv6Count < kMaxIPv6AddrCount; i++)
        {
            if (ip6_addr_isvalid(netif_ip6_addr_state(staNetif, i)))
            {
                memcpy(ifp->Ipv6AddressesBuffer[ipv6Count], netif_ip6_addr(staNetif, i)->addr, kMaxIPv6AddrSize);
                ifp->Ipv6AddressSpans[ipv6Count] = ByteSpan(ifp->Ipv6AddressesBuffer[ipv6Count], kMaxIPv6AddrSize);
                ipv6Count++;
            }
        }
        ifp->IPv6Addresses = app::DataModel::List<ByteSpan>(ifp->Ipv6AddressSpans, ipv6Count);
    }

    *netifpp = ifp;
    return CHIP_NO_ERROR;
}

void DiagnosticDataProviderImpl::ReleaseNetworkInterfaces(NetworkInterface * netifp)
{
    while (netifp)
    {
        NetworkInterface * del = netifp;
        netifp                 = netifp->Next;
        delete del;
    }
}

CHIP_ERROR DiagnosticDataProviderImpl::GetActiveHardwareFaults(GeneralFaults<kMaxHardwareFaults> & hardwareFaults)
{
    return CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE;
}

CHIP_ERROR DiagnosticDataProviderImpl::GetActiveRadioFaults(GeneralFaults<kMaxRadioFaults> & radioFaults)
{
    return CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE;
}

CHIP_ERROR DiagnosticDataProviderImpl::GetActiveNetworkFaults(GeneralFaults<kMaxNetworkFaults> & networkFaults)
{
    return CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE;
}

DiagnosticDataProvider & GetDiagnosticDataProviderImpl()
{
    return DiagnosticDataProviderImpl::GetDefaultInstance();
}

} // namespace DeviceLayer
} // namespace chip
