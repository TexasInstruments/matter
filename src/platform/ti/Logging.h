/*
 *
 *    Copyright (c) 2023 Project CHIP Authors
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

 #include <stdint.h>
 #include <stdio.h>
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /*
  * Functions to setup and interact with a single UART interface for the
  * Logging, shell streamer, and OpenThread CLI.
  */
 
 void uartConsoleInit(void);
 ssize_t uartConsoleRead(char * buf, size_t len);
 ssize_t uartConsoleWrite(const char * buf, size_t len);
 
 #ifdef __cplusplus
 }
 #endif