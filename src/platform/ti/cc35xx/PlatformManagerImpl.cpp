/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
 *    Copyright (c) 2020 Texas Instruments Incorporated
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
 *          Provides an implementation of the PlatformManager object
 *          for the Texas Instruments CC35XX platform.
 */

/* this file behaves like a config.h, comes first */
#include <platform/internal/CHIPDeviceLayerInternal.h>

#include <crypto/CHIPCryptoPAL.h>
#include <lib/support/TimeUtils.h>
#include <platform/PlatformManager.h>
#include <platform/ti/cc35xx/DiagnosticDataProviderImpl.h>
#include <platform/internal/GenericPlatformManagerImpl_FreeRTOS.ipp>

#include <lwip/tcpip.h>

#if CHIP_DEVICE_CONFIG_ENABLE_OTA_REQUESTOR
#include "psa_fwu.h"
#endif

namespace chip {
namespace DeviceLayer {

PlatformManagerImpl PlatformManagerImpl::sInstance;

CHIP_ERROR PlatformManagerImpl::_InitChipStack(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    // Initialize the configuration system.
    err = Internal::CC35XXConfig::Init();
    SuccessOrExit(err);

#if CHIP_DEVICE_CONFIG_ENABLE_OTA_REQUESTOR
    // Must be called before any other PSA FWU API. Reads the boot report and
    // initializes component state — required for IsFirstImageRun() to detect TRIAL state.
    psa_fwu_init();
#endif

    // Call _InitChipStack() on the generic implementation base class
    // to finish the initialization process.
    err = Internal::GenericPlatformManagerImpl_FreeRTOS<PlatformManagerImpl>::_InitChipStack();
    SuccessOrExit(err);

    SystemLayer().StartTimer(System::Clock::Seconds32(kSecondsPerHour), UpdateOperationalHours, nullptr);

exit:
    return err;
}

void PlatformManagerImpl::UpdateOperationalHours(System::Layer * systemLayer, void * appState)
{
    uint32_t totalOperationalHours = 0;

    if (ConfigurationMgr().GetTotalOperationalHours(totalOperationalHours) == CHIP_NO_ERROR)
    {
        ConfigurationMgr().StoreTotalOperationalHours(totalOperationalHours + 1);
    }
    else
    {
        ChipLogError(DeviceLayer, "Failed to get total operational hours of the Node");
    }

    SystemLayer().StartTimer(System::Clock::Seconds32(kSecondsPerHour), UpdateOperationalHours, nullptr);
}

void PlatformManagerImpl::_Shutdown()
{
    Internal::GenericPlatformManagerImpl_FreeRTOS<PlatformManagerImpl>::_Shutdown();
}

} // namespace DeviceLayer
} // namespace chip
