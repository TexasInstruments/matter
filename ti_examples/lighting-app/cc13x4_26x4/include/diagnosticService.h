/**********************************************************************************************
 * Filename:       diagnosticService.h
 *
 * Description:    This file contains the diagnosticService service definitions and
 *                 prototypes.
 *
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *************************************************************************************************/

#ifndef _DIAGNOSTICSERVICE_H_
#define _DIAGNOSTICSERVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * INCLUDES
 */
#include <AppoBLE_interface_c.h>
#include <bcomdef.h>
#include <stdint.h>
#include <ti_ble_gatt_helper.h>
/*********************************************************************
 * CONSTANTS
 */

// Service UUID
#define DIAGNOSTICSERVICE_SERV_UUID 0x11B0

//  Characteristic defines
#define DIAGNOSTICSERVICE_HEAPFREE_ID 0
#define DIAGNOSTICSERVICE_HEAPFREE_UUID 0x11B1
#define DIAGNOSTICSERVICE_HEAPFREE_LEN 8

//  Characteristic defines
#define DIAGNOSTICSERVICE_HEAPUSED_ID 1
#define DIAGNOSTICSERVICE_HEAPUSED_UUID 0x11B2
#define DIAGNOSTICSERVICE_HEAPUSED_LEN 8

//  Characteristic defines
#define DIAGNOSTICSERVICE_THREADNETKEY_ID 2
#define DIAGNOSTICSERVICE_THREADNETKEY_UUID 0x11B3
#define DIAGNOSTICSERVICE_THREADNETKEY_LEN 16

//  Characteristic defines
#define DIAGNOSTICSERVICE_THREADPANID_ID 3
#define DIAGNOSTICSERVICE_THREADPANID_UUID 0x11B4
#define DIAGNOSTICSERVICE_THREADPANID_LEN 2

//  Characteristic defines
#define DIAGNOSTICSERVICE_THREADCHANNEL_ID 4
#define DIAGNOSTICSERVICE_THREADCHANNEL_UUID 0x11B5
#define DIAGNOSTICSERVICE_THREADCHANNEL_LEN 1

//  Characteristic defines
#define DIAGNOSTICSERVICE_REFRESH_ID 5
#define DIAGNOSTICSERVICE_REFRESH_UUID 0x11B6
#define DIAGNOSTICSERVICE_REFRESH_LEN 1

//  Characteristic defines
#define DIAGNOSTICSERVICE_PERIOD_ID 6
#define DIAGNOSTICSERVICE_PERIOD_UUID 0x11B7
#define DIAGNOSTICSERVICE_PERIOD_LEN 2

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */

// Callback when a characteristic value has changed
typedef void (*diagnosticServiceChange_t)(void);

typedef struct
{
    GenericServiceChange_t pfnChangeCb; // Called when characteristic value changes
    diagnosticServiceChange_t pfnCfgChangeCb;
} diagnosticServiceCBs_t;

/*********************************************************************
 * API FUNCTIONS
 */

/*
 * DiagnosticService_AddService- Initializes the DiagnosticService service by registering
 *          GATT attributes with the GATT server.
 *
 */
extern bStatus_t DiagnosticService_AddService(uint32 services);

/*
 * DiagnosticService_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t DiagnosticService_RegisterAppCBs(diagnosticServiceCBs_t * appCallbacks);

/*
 * DiagnosticService_SetParameter - Set a DiagnosticService parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t DiagnosticService_SetParameter(uint8_t param, uint16_t len, void * value);

/*
 * DiagnosticService_GetParameter - Get a DiagnosticService parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t DiagnosticService_GetParameter(uint8_t param, uint8_t len, void * value);

/*
 * _GetParamId - Get a characteristic UUID to ID mapping
 *
 *    uuid - Service Characteristic UUID
 */
extern uint8_t diagnosticService_GetParamId(const uint8_t * uuid);

/*********************************************************************
*********************************************************************/

/*****************************************************
Extern variables
*****************************************************/

// DiagnosticService Service UUID
extern CONST uint8 diagnosticServiceUUID[ATT_UUID_SIZE];

// DiagnosticHeapFree UUID
extern CONST uint8 diagnosticService_HeapFreeUUID[ATT_UUID_SIZE];

// DiagnosticHeapUsed UUID
extern CONST uint8 diagnosticService_HeapUsedUUID[ATT_UUID_SIZE];

// DiagnosticThreadNetKey UUID
extern CONST uint8 diagnosticService_ThreadNetKeyUUID[ATT_UUID_SIZE];

// DiagnosticThreadPanId UUID
extern CONST uint8 diagnosticService_ThreadPanIDUUID[ATT_UUID_SIZE];
// DiagnosticRefresh UUID
extern CONST uint8 diagnosticService_RefreshUUID[ATT_UUID_SIZE];
// DiagnosticPeriod UUID
extern CONST uint8 diagnosticService_PeriodUUID[ATT_UUID_SIZE];
// DiagnosticThreadChannelUUID
extern CONST uint8_t diagnosticService_ThreadChannelUUID[ATT_UUID_SIZE];

#ifdef __cplusplus
}
#endif

#endif /* _DIAGNOSTICSERVICE_H_ */