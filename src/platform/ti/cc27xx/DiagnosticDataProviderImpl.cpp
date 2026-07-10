/*
 *    Copyright (c) 2021 Project CHIP Authors
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
 * @file
 *   DiagnosticDataProvider stub for cc27xx.
 *
 * All methods return CHIP_ERROR_NOT_IMPLEMENTED pending a proper implementation.
 *
 * TODO(improvement#1): Implement using cc27xx-specific APIs:
 *   - GetCurrentHeapFree/Used: xPortGetFreeHeapSize() / xPortGetMinimumEverFreeHeapSize()
 *   - GetBootReason: PMCTLGetResetReason() from <ti/devices/cc27xxx20/driverlib/pmctl.h>
 *   - GetRebootCount / GetTotalOperationalHours: persist via NVS (nvocmp)
 *   - GetThreadMetrics: FreeRTOS uxTaskGetSystemState()
 *   - GetNetworkInterfaces: OpenThread netif
 */

#include "DiagnosticDataProviderImpl.h"
#include <lib/support/CodeUtils.h>

namespace chip {
namespace DeviceLayer {

DiagnosticDataProviderImpl & DiagnosticDataProviderImpl::GetDefaultInstance()
{
    static DiagnosticDataProviderImpl sInstance;
    return sInstance;
}

DiagnosticDataProvider & GetDiagnosticDataProviderImpl()
{
    return DiagnosticDataProviderImpl::GetDefaultInstance();
}

CHIP_ERROR DiagnosticDataProviderImpl::GetRebootCount(uint16_t & rebootCount)
{
    // TODO(improvement#1): read from persistent storage via nvocmp
    return CHIP_ERROR_NOT_IMPLEMENTED;
}

CHIP_ERROR DiagnosticDataProviderImpl::GetBootReason(BootReasonType & bootReason)
{
    // TODO(improvement#1): use PMCTLGetResetReason() from driverlib/pmctl.h
    bootReason = BootReasonType::kUnspecified;
    return CHIP_NO_ERROR;
}

CHIP_ERROR DiagnosticDataProviderImpl::GetUpTime(uint64_t & upTime)
{
    // TODO(improvement#1): use chip::System::SystemClock().GetMonotonicTimestamp()
    return CHIP_ERROR_NOT_IMPLEMENTED;
}

CHIP_ERROR DiagnosticDataProviderImpl::GetTotalOperationalHours(uint32_t & totalOperationalHours)
{
    // TODO(improvement#1): persist and read from nvocmp
    return CHIP_ERROR_NOT_IMPLEMENTED;
}

CHIP_ERROR DiagnosticDataProviderImpl::GetCurrentHeapFree(uint64_t & currentHeapFree)
{
    // TODO(improvement#1): use xPortGetFreeHeapSize() from FreeRTOS
    return CHIP_ERROR_NOT_IMPLEMENTED;
}

CHIP_ERROR DiagnosticDataProviderImpl::GetCurrentHeapUsed(uint64_t & currentHeapUsed)
{
    // TODO(improvement#1): use configTOTAL_HEAP_SIZE - xPortGetFreeHeapSize()
    return CHIP_ERROR_NOT_IMPLEMENTED;
}

CHIP_ERROR DiagnosticDataProviderImpl::GetThreadMetrics(ThreadMetrics ** threadMetricsOut)
{
    // TODO(improvement#1): use uxTaskGetSystemState() from FreeRTOS
    return CHIP_ERROR_NOT_IMPLEMENTED;
}

void DiagnosticDataProviderImpl::ReleaseThreadMetrics(ThreadMetrics * threadMetrics) {}

CHIP_ERROR DiagnosticDataProviderImpl::GetActiveHardwareFaults(GeneralFaults<kMaxHardwareFaults> & hardwareFaults)
{
    return CHIP_NO_ERROR;
}

CHIP_ERROR DiagnosticDataProviderImpl::GetActiveRadioFaults(GeneralFaults<kMaxRadioFaults> & radioFaults)
{
    return CHIP_NO_ERROR;
}

CHIP_ERROR DiagnosticDataProviderImpl::GetActiveNetworkFaults(GeneralFaults<kMaxNetworkFaults> & networkFaults)
{
    return CHIP_NO_ERROR;
}

CHIP_ERROR DiagnosticDataProviderImpl::GetNetworkInterfaces(NetworkInterface ** netifpp)
{
    // TODO(improvement#1): enumerate OpenThread netif
    return CHIP_ERROR_NOT_IMPLEMENTED;
}

void DiagnosticDataProviderImpl::ReleaseNetworkInterfaces(NetworkInterface * netifp) {}

} // namespace DeviceLayer
} // namespace chip
