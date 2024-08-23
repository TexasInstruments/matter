/******************************************************************************

 @file AppoBLE_interface_c.h

 @brief C bindings for the AppoBLE extensions

 Group: CMCU, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2022-2023, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#ifndef APPOBLE_INTERFACE_C_H
#define APPOBLE_INTERFACE_C_H

#ifdef __cplusplus
extern "C" {
#endif
#include <bcomdef.h>
#include <icall.h>
#include <icall_ble_api.h>

#include "hal_types.h"

#include "ti_ble_config.h"
#include "ti_drivers_config.h"

#ifdef __cplusplus
}
#endif

/* Callback function prototype for ScheduleAppoBLEFxn() Calls */
typedef void (*pfnCallInBleMgrCtx_t)(void *);
// Callback when a characteristic value has changed
typedef void (*GenericServiceChange_t)(const uint8_t * servUUID, const uint8_t * charUUID, uint8_t len, uint16_t connHandle);

typedef struct
{
    GenericServiceChange_t pfnSimpleProfileChange1; // Called when characteristic value changes
} GenericServiceCBs_t;

extern bStatus_t ScheduleAppoBLEFxn(pfnCallInBleMgrCtx_t FxnPtr, void * arg);

#endif
