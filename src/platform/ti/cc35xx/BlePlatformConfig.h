/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
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
 *          Platform-specific configuration overrides for the CHIP BLE
 *          Layer for the Texas Instruments CC35XX platform.
 *
 * Platform-specific BLE configuration for CC35XX (NimBLE backend).
 */

#pragma once

// Maximum number of concurrent BLE connections (commissioning = 1)
#define BLE_LAYER_NUM_BLE_ENDPOINTS 1

// NimBLE uses uint16_t connection handles, not void*
#define BLE_CONNECTION_OBJECT uint16_t
#define BLE_CONNECTION_UNINITIALIZED UINT16_MAX
