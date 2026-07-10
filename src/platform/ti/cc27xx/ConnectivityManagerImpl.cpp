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
 *   cc27xx-specific ConnectivityManager additions.
 *
 * TODO(improvement#1): Full cc27xx ConnectivityManagerImpl should be written
 *   here rather than borrowing from cc13xx_26xx. Currently only adds the
 *   functions missing from the borrowed implementation.
 */

#include <platform/internal/CHIPDeviceLayerInternal.h>
#include <platform/ti/cc27xx/ConnectivityManagerImpl.h>

#if !CHIP_DEVICE_CONFIG_SUPPORTS_CONCURRENT_CONNECTION
#include <app/server/Server.h>
#include <platform/DeviceControlServer.h>
#endif

namespace chip {
namespace DeviceLayer {

#if !CHIP_DEVICE_CONFIG_SUPPORTS_CONCURRENT_CONNECTION
void ConnectivityManagerImpl::StartNonConcurrentThreadManagement()
{
    DeviceControlServer::DeviceControlSvr().PostOperationalNetworkStartedEvent();
    ChipLogProgress(DeviceLayer, "Non-concurrent mode Thread Management Started.");
}
#endif

} // namespace DeviceLayer
} // namespace chip
