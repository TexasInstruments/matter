/**********************************************************************************************
 * Filename:       throughputService.h
 *
 * Description:    This file contains the throughputService service definitions and
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


#ifndef _THROUGHPUTSERVICE_H_
#define _THROUGHPUTSERVICE_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <bcomdef.h>
#include <stdint.h>
#include <AppoBLE_interface_c.h>
#include <ti_ble_gatt_helper.h>
/*********************************************************************
* CONSTANTS
*/

// Service UUID
#define THROUGHPUTSERVICE_SERV_UUID 0x11C0

//  Characteristic defines
#define THROUGHPUTSERVICE_THROUGHPUTENABLE_ID   0
#define THROUGHPUTSERVICE_THROUGHPUTENABLE_UUID 0x11C1
#define THROUGHPUTSERVICE_THROUGHPUTENABLE_LEN  1

//  Characteristic defines
#define THROUGHPUTSERVICE_MATTERENABLE_ID   1
#define THROUGHPUTSERVICE_MATTERENABLE_UUID 0x11C2
#define THROUGHPUTSERVICE_MATTERENABLE_LEN  1

//  Characteristic defines
#define THROUGHPUTSERVICE_THROUGHPUTTX_ID   2
#define THROUGHPUTSERVICE_THROUGHPUTTX_UUID 0x11C3
#define THROUGHPUTSERVICE_THROUGHPUTTX_LEN 244

//  Characteristic defines
#define THROUGHPUTSERVICE_THROUGPUTRX_ID   3
#define THROUGHPUTSERVICE_THROUGPUTRX_UUID 0x11C4
#define THROUGHPUTSERVICE_THROUGPUTRX_LEN 244

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
typedef void (*throughputServiceChange_t)(void);

typedef struct
{
  GenericServiceChange_t        pfnChangeCb;  // Called when characteristic value changes
  throughputServiceChange_t        pfnCfgChangeCb;
} throughputServiceCBs_t;



/*********************************************************************
 * API FUNCTIONS
 */


/*
 * ThroughputService_AddService- Initializes the ThroughputService service by registering
 *          GATT attributes with the GATT server.
 *
 */
extern bStatus_t ThroughputService_AddService( uint32 services);

/*
 * ThroughputService_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t ThroughputService_RegisterAppCBs( throughputServiceCBs_t *appCallbacks );

/*
 * ThroughputService_SetParameter - Set a ThroughputService parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t ThroughputService_SetParameter(uint8_t param, uint16_t len, void *value);

/*
 * ThroughputService_GetParameter - Get a ThroughputService parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t ThroughputService_GetParameter(uint8_t param, uint8_t len, void *value);

/*
 * _GetParamId - Get a characteristic UUID to ID mapping
 *
 *    uuid - Service Characteristic UUID
 */
extern uint8_t throughputService_GetParamId(const uint8_t * uuid);

/*********************************************************************
*********************************************************************/

/*****************************************************
Extern variables
*****************************************************/

// throughputService Service UUID
extern CONST uint8 throughputServiceUUID[ATT_UUID_SIZE];

// ThroughputEnable UUID
extern CONST uint8 throughputService_ThroughputEnableUUID[ATT_UUID_SIZE];

// MatterEnable UUID
extern CONST uint8 throughputService_MatterEnableUUID[ATT_UUID_SIZE];

// ThroughputTx UUID
extern CONST uint8 throughputService_ThroughputTxUUID[ATT_UUID_SIZE];

// ThrougputRx UUID
extern CONST uint8 throughputService_ThrougputRxUUID[ATT_UUID_SIZE];


#ifdef __cplusplus
}
#endif

#endif /* _THROUGHPUTSERVICE_H_ */