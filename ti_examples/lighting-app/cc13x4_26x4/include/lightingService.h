/*******************************************************************************
 * Filename:    ble_gatt_service.h
 *
 * Description:    This file contains the services definitions and
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
 ******************************************************************************/

#ifndef _SERVICES_H_
#define _SERVICES_H_

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * INCLUDES
 *********************************************************************/
#include <bcomdef.h>
#include <stdint.h>
#include <AppoBLE_interface_c.h>
#include <ti_ble_gatt_helper.h>

/*********************************************************************
 * CONSTANTS
 *********************************************************************/
// Profile Parameters
// Service UUID
#define LIGHTSERVICE_SERV_UUID 0x11A0
// Characteristic defines
#define LIGHTSERVICE_LIGHTONOFF 0
#define LIGHTSERVICE_LIGHTONOFF_UUID 0x11A1
#define LIGHTSERVICE_LIGHTONOFF_LEN 1
// Characteristic defines
#define LIGHTSERVICE_LIGHTTOGGLE 1
#define LIGHTSERVICE_LIGHTTOGGLE_UUID 0x11A2
#define LIGHTSERVICE_LIGHTTOGGLE_LEN 1
// Characteristic defines
#define LIGHTSERVICE_LIGHTSTATE 2
#define LIGHTSERVICE_LIGHTSTATE_UUID 0x11A3
#define LIGHTSERVICE_LIGHTSTATE_LEN 1

#define LIGHTSERVICE_LIGHTSTATE_CCC 3
/*********************************************************************
 * TYPEDEFS
 *********************************************************************/

/*********************************************************************
 * MACROS
 *********************************************************************/

/*********************************************************************
 * Profile Callbacks
 *********************************************************************/

// Callback when a characteristic value has changed
typedef void (*LightServiceEvt_t)(uint8_t event, uint8_t dummy);

typedef struct
{
    GenericServiceChange_t pfnSimpleProfileChange; // Called when characteristic value changes
    LightServiceEvt_t pfnTestEvt;                  // Sample Callback function to demonstrate custom service event usage
} LightServiceCBs_t;

#define LIGHT_SERVICE_TEST_EVENT (0x1)
/*********************************************************************
 * API FUNCTIONS
 *********************************************************************/
/*
 * _AddService- Initializes the service by registering
 *          GATT attributes with the GATT server.
 *
 */
extern bStatus_t LightService_AddService(uint32 services);

/*
 * _RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t LightService_RegisterAppCBs(LightServiceCBs_t * appCallbacks);

/*
 * _SetParameter - Set a service parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *    taskId - Task to be notified of indication
 */
extern bStatus_t LightService_SetParameter(uint8 param, uint8 len, void * value, uint8_t taskId);

/*
 * _GetParameter - Get a service parameter.
 *
 *    param - Profile parameter ID
 *    len - Length of target data
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t LightService_GetParameter(uint8 param, uint8_t len, void * value);

/*
 * _GetParamId - Get a characteristic UUID to ID mapping
 *
 *    uuid - Service Characteristic UUID
 */
extern uint8_t gatt_LightService_GetParamId(const uint8_t * uuid);

/*****************************************************
Extern variables
*****************************************************/
extern LightServiceCBs_t * LightService_AppCBs;
extern uint8 LightServiceLightOnOff;
extern uint8 LightServiceLightToggle;
extern uint8 LightServiceLightState;

// LightService Service UUID
extern CONST uint8 LightServiceServUUID[ATT_UUID_SIZE];

// LightOnOff UUID
extern CONST uint8 LightServiceLightOnOffUUID[ATT_UUID_SIZE];

// LightToggle UUID
extern CONST uint8 LightServiceLightToggleUUID[ATT_UUID_SIZE];

// LightState UUID
extern CONST uint8 LightServiceLightStateUUID[ATT_UUID_SIZE];

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _SERVICES_H_ */
