/*
 *    Copyright (c) 2020 Project CHIP Authors
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

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

// Logging
#ifdef __cplusplus
extern "C" {
#endif

#ifdef ti_log_Log_ENABLE
#include "ti_drivers_config.h"
#include "ti_log_config.h"
#endif

int cc27xxLogInit(void);
void cc27xxLog(const char * aFormat, ...);
#ifndef ti_log_Log_ENABLE
#define PLAT_LOG(...) cc27xxLog(__VA_ARGS__);
#else // SILK Logging
#define PLAT_LOG(...) Log_printf(LogModule_Matter, Log_DEBUG, __VA_ARGS__);
#endif

#ifdef __cplusplus
}
#endif
#endif // APP_CONFIG_H
