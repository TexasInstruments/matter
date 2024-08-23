/*******************************************************************************
 * Filename:    ble_gatt_service.c
 *
 * Description:    This file contains the implementation of the service.
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

/*********************************************************************
 * INCLUDES
 *********************************************************************/
#include "util.h"
#include <icall.h>
#include <string.h>
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"
#include "lightingService.h"

#ifdef SYSCFG
#include "ti_ble_config.h"
#endif
/*********************************************************************
 * MACROS
 *********************************************************************/

/*********************************************************************
 * CONSTANTS
 *********************************************************************/

/*********************************************************************
 * TYPEDEFS
 *********************************************************************/

/*********************************************************************
 * GLOBAL VARIABLES
 *********************************************************************/

// LightService Service UUID
CONST uint8 LightServiceServUUID[ATT_UUID_SIZE] = { TI_BASE_UUID_128(LIGHTSERVICE_SERV_UUID) };

// LightOnOff UUID
CONST uint8 LightServiceLightOnOffUUID[ATT_UUID_SIZE] = { TI_BASE_UUID_128(LIGHTSERVICE_LIGHTONOFF_UUID) };

// LightToggle UUID
CONST uint8 LightServiceLightToggleUUID[ATT_UUID_SIZE] = { TI_BASE_UUID_128(LIGHTSERVICE_LIGHTTOGGLE_UUID) };

// LightState UUID
CONST uint8 LightServiceLightStateUUID[ATT_UUID_SIZE] = { TI_BASE_UUID_128(LIGHTSERVICE_LIGHTSTATE_UUID) };

/*********************************************************************
 * LOCAL VARIABLES
 *********************************************************************/
LightServiceCBs_t * LightService_AppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 *********************************************************************/

// Service declaration
static CONST gattAttrType_t LightServiceService = { ATT_UUID_SIZE, LightServiceServUUID };

// Characteristic "LightOnOff" Properties (for declaration)
static uint8 LightServiceLightOnOffProps = GATT_PROP_READ | GATT_PROP_WRITE | GATT_PROP_AUTHEN;
// Characteristic "LightOnOff" Value variable
uint8 LightServiceLightOnOff = 0x0;
// Characteristic "LightOnOff" User Description
static uint8 LightServiceLightOnOffUserDesp[] = "Set Light state to On (1) or Off (0)";

// Characteristic "LightToggle" Properties (for declaration)
static uint8 LightServiceLightToggleProps = GATT_PROP_READ | GATT_PROP_WRITE | GATT_PROP_AUTHEN;
// Characteristic "LightToggle" Value variable
uint8 LightServiceLightToggle = 0x0;
// Characteristic "LightToggle" User Description
static uint8 LightServiceLightToggleUserDesp[] = "Toggle Light on Local device";

// Characteristic "LightState" Properties (for declaration)
static uint8 LightServiceLightStateProps = GATT_PROP_NOTIFY | GATT_PROP_READ | GATT_PROP_AUTHEN;
// Characteristic "LightState" Value variable
uint8 LightServiceLightState = 0x0;
// Characteristic "LightState" User Description
static uint8 LightServiceLightStateUserDesp[] = "Read current Light state On (1) or Off (0)	";
// Characteristic "LightState" CCCD
static gattCharCfg_t * LightServiceLightStateConfig;

/*********************************************************************
 * Profile Attributes - Table
 *********************************************************************/
static gattAttribute_t LightServiceAttrTbl[] = {
    { { ATT_BT_UUID_SIZE, primaryServiceUUID }, GATT_PERMIT_READ, 0, (uint8 *) &LightServiceService },

    // LightOnOff Characteristic Declaration
    { { ATT_BT_UUID_SIZE, characterUUID }, GATT_PERMIT_READ, 0, &LightServiceLightOnOffProps },

    // LightOnOff Characteristic Value
    { { ATT_UUID_SIZE, LightServiceLightOnOffUUID }, GATT_PERMIT_READ | GATT_PERMIT_WRITE | GATT_PERMIT_AUTHEN_READ | GATT_PERMIT_AUTHEN_WRITE, 0, &LightServiceLightOnOff },

    // Characteristic LightOnOff User Description
    { { ATT_BT_UUID_SIZE, charUserDescUUID }, GATT_PERMIT_READ, 0, LightServiceLightOnOffUserDesp },

    // LightToggle Characteristic Declaration
    { { ATT_BT_UUID_SIZE, characterUUID }, GATT_PERMIT_READ, 0, &LightServiceLightToggleProps },

    // LightToggle Characteristic Value
    { { ATT_UUID_SIZE, LightServiceLightToggleUUID }, GATT_PERMIT_READ | GATT_PERMIT_WRITE | GATT_PERMIT_AUTHEN_READ | GATT_PERMIT_AUTHEN_WRITE, 0, &LightServiceLightToggle },

    // Characteristic LightToggle User Description
    { { ATT_BT_UUID_SIZE, charUserDescUUID }, GATT_PERMIT_READ, 0, LightServiceLightToggleUserDesp },

    // LightState Characteristic Declaration
    { { ATT_BT_UUID_SIZE, characterUUID }, GATT_PERMIT_READ, 0, &LightServiceLightStateProps },

    // LightState Characteristic Value
    { { ATT_UUID_SIZE, LightServiceLightStateUUID }, GATT_PERMIT_READ | GATT_PERMIT_AUTHEN_READ , 0, &LightServiceLightState },

    // Characteristic LightState User Description
    { { ATT_BT_UUID_SIZE, charUserDescUUID }, GATT_PERMIT_READ, 0, LightServiceLightStateUserDesp },

    // LightState configuration
    { { ATT_BT_UUID_SIZE, clientCharCfgUUID }, GATT_PERMIT_READ | GATT_PERMIT_WRITE, 0, (uint8 *) &LightServiceLightStateConfig },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 *********************************************************************/
static bStatus_t gatt_LightService_ReadAttrCB(uint16_t connHandle, gattAttribute_t * pAttr, uint8_t * pValue, uint16_t * pLen,
                                              uint16_t offset, uint16_t maxLen, uint8_t method);
static bStatus_t gatt_LightService_WriteAttrCB(uint16_t connHandle, gattAttribute_t * pAttr, uint8_t * pValue, uint16_t len,
                                               uint16_t offset, uint8_t method);
static bStatus_t gatt_LightService_AuthorizationCB(uint16_t connHandle, gattAttribute_t * pAttr, uint8_t opcode);

/*********************************************************************
 * PROFILE CALLBACKS
 *********************************************************************/
// LightService Service Callbacks
CONST gattServiceCBs_t LightServiceCBs = {
    gatt_LightService_ReadAttrCB,     // Read callback function pointer
    gatt_LightService_WriteAttrCB,    // Write callback function pointer
    gatt_LightService_AuthorizationCB // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 *********************************************************************/
/*
 * AddService - Initializes the services by registering
 * GATT attributes with the GATT server.
 *
 */
bStatus_t LightService_AddService(uint32 services)
{
    uint8 status;

    // Allocate Client Characteristic Configuration table
    LightServiceLightStateConfig = (gattCharCfg_t *) ICall_malloc(sizeof(gattCharCfg_t) * MAX_NUM_BLE_CONNS);
    if (LightServiceLightStateConfig == NULL)
    {
        return (bleMemAllocError);
    }
    // Initialize Client Characteristic Configuration attributes
    GATTServApp_InitCharCfg(LINKDB_CONNHANDLE_INVALID, LightServiceLightStateConfig);
    if (services)
    {
        // Register GATT attribute list and CBs with GATT Server App
        status = GATTServApp_RegisterService(LightServiceAttrTbl, GATT_NUM_ATTRS(LightServiceAttrTbl), GATT_MAX_ENCRYPT_KEY_SIZE,
                                             &LightServiceCBs);
    }
    else
    {
        status = SUCCESS;
    }
    return (status);
}

/***************************************************************
 * RegisterAppCBs - Registers the application callback function.
 *                  Only call this function once.
 *
 * appCallbacks - pointer to application callbacks.
 ***************************************************************/

bStatus_t LightService_RegisterAppCBs(LightServiceCBs_t * appCallbacks)
{
    if (appCallbacks)
    {
        LightService_AppCBs = appCallbacks;
        return (SUCCESS);
    }
    else
    {
        return (bleAlreadyInRequestedMode);
    }
}

/*******************************************************************
 * SetParameter - Set a service parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 ********************************************************************/
bStatus_t LightService_SetParameter(uint8 param, uint8 len, void * value, uint8_t taskId)
{
    bStatus_t ret = SUCCESS;
    switch (param)
    {

    case LIGHTSERVICE_LIGHTONOFF:

        if (len == sizeof(uint8))
        {
            memcpy(&LightServiceLightOnOff, value, len);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case LIGHTSERVICE_LIGHTTOGGLE:

        if (len == sizeof(uint8))
        {
            memcpy(&LightServiceLightToggle, value, len);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case LIGHTSERVICE_LIGHTSTATE:

        if (len == sizeof(uint8))
        {
            memcpy(&LightServiceLightState, value, len);
            // Try to send notification.
            GATTServApp_ProcessCharCfg(LightServiceLightStateConfig, (uint8 *) &LightServiceLightState, FALSE, LightServiceAttrTbl,
                                       GATT_NUM_ATTRS(LightServiceAttrTbl), taskId, gatt_LightService_ReadAttrCB);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    default:
        ret = INVALIDPARAMETER;
        break;
    }
    return ret;
}

/******************************************************************
 * GetParameter - Get a service parameter.
 *
 * param - Profile parameter ID
 * value - pointer to data to write.  This is dependent on
 *         the parameter ID and WILL be cast to the appropriate
 *         data type (example: data type of uint16 will be cast to
 *         uint16 pointer).
 ******************************************************************/
bStatus_t LightService_GetParameter(uint8 param, uint8_t len, void * value)
{
    bStatus_t ret = SUCCESS;
    switch (param)
    {
    case LIGHTSERVICE_LIGHTONOFF: {
        memcpy(value, &LightServiceLightOnOff, LIGHTSERVICE_LIGHTONOFF_LEN);
        break;
    }

    case LIGHTSERVICE_LIGHTTOGGLE: {
        memcpy(value, &LightServiceLightToggle, LIGHTSERVICE_LIGHTTOGGLE_LEN);
        break;
    }

    case LIGHTSERVICE_LIGHTSTATE: {
        memcpy(value, &LightServiceLightState, LIGHTSERVICE_LIGHTSTATE_LEN);
        break;
    }
    case LIGHTSERVICE_LIGHTSTATE_CCC: {
        *((uint8 *) value) = LightServiceLightStateConfig->value;
        break;
    }

    default: {
        ret = INVALIDPARAMETER;
        break;
    }
    }
    return ret;
}

/*********************************************************************
 * @fn          ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 ********************************************************************/
static bStatus_t gatt_LightService_ReadAttrCB(uint16_t connHandle, gattAttribute_t * pAttr, uint8_t * pValue, uint16_t * pLen,
                                              uint16_t offset, uint16_t maxLen, uint8_t method)
{
    bStatus_t status = SUCCESS;

    // Make sure it's not a blob operation (no attributes in the profile are long)
    if (offset > 0)
    {
        return (ATT_ERR_ATTR_NOT_LONG);
    }

    if (pAttr->type.len == ATT_UUID_SIZE)
    {
        if (!memcmp(pAttr->type.uuid, LightServiceLightOnOffUUID, pAttr->type.len))
        {
            *pLen = LIGHTSERVICE_LIGHTONOFF_LEN;
            VOID osal_memcpy(pValue, pAttr->pValue, LIGHTSERVICE_LIGHTONOFF_LEN);
        }
        else if (!memcmp(pAttr->type.uuid, LightServiceLightToggleUUID, pAttr->type.len))
        {
            *pLen = LIGHTSERVICE_LIGHTTOGGLE_LEN;
            VOID osal_memcpy(pValue, pAttr->pValue, LIGHTSERVICE_LIGHTTOGGLE_LEN);
        }
        else if (!memcmp(pAttr->type.uuid, LightServiceLightStateUUID, pAttr->type.len))
        {
            *pLen = LIGHTSERVICE_LIGHTSTATE_LEN;
            VOID osal_memcpy(pValue, pAttr->pValue, LIGHTSERVICE_LIGHTSTATE_LEN);
        }
        else
        {
            // Should never get here! (other characteristics do not have read permissions)
            *pLen  = 0;
            status = ATT_ERR_ATTR_NOT_FOUND;
        }
    }

    return status;
}

/*********************************************************************
 * @fn      WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 ********************************************************************/
static bStatus_t gatt_LightService_WriteAttrCB(uint16_t connHandle, gattAttribute_t * pAttr, uint8_t * pValue, uint16_t len,
                                               uint16_t offset, uint8_t method)
{
    bStatus_t status = SUCCESS;

    const uint8_t * notifyApp = NULL;

    if (pAttr->type.len == ATT_UUID_SIZE)
    {
        if (!memcmp(pAttr->type.uuid, LightServiceLightOnOffUUID, pAttr->type.len))
        {
            if (offset + len > LIGHTSERVICE_LIGHTONOFF_LEN)
            {
                status = ATT_ERR_INVALID_OFFSET;
            }
            else
            {
                // Copy pValue into the variable we point to from the attribute table.
                VOID osal_memcpy(pAttr->pValue, pValue, len);

                // Only notify application if entire expected value is written
                if (offset + len == LIGHTSERVICE_LIGHTONOFF_LEN)
                notifyApp = LightServiceLightOnOffUUID;
            }
        }
        else if (!memcmp(pAttr->type.uuid, LightServiceLightToggleUUID, pAttr->type.len))
        {
            if (offset + len > LIGHTSERVICE_LIGHTTOGGLE_LEN)
            {
                status = ATT_ERR_INVALID_OFFSET;
            }
            else
            {
                // Copy pValue into the variable we point to from the attribute table.
                VOID osal_memcpy(pAttr->pValue, pValue, len);

                // Only notify application if entire expected value is written
                if (offset + len == LIGHTSERVICE_LIGHTTOGGLE_LEN)
                notifyApp = LightServiceLightToggleUUID;
            }
        }
        else
        {
            status = ATT_ERR_ATTR_NOT_FOUND;
        }
    }
    else if ((pAttr->type.len == ATT_BT_UUID_SIZE) &&
        (BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]) == GATT_CLIENT_CHAR_CFG_UUID))
    {
        notifyApp = clientCharCfgUUID;

        status = GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue, len, offset, GATT_CLIENT_CFG_NOTIFY);
    }
    else
    {
        status = ATT_ERR_ATTR_NOT_FOUND;
    }

    if (LightService_AppCBs && LightService_AppCBs->pfnTestEvt)
    {
        LightService_AppCBs->pfnTestEvt(LIGHT_SERVICE_TEST_EVENT, 0);
    }

    // If a characteristic value changed then callback function to notify application of change
    if ((notifyApp != NULL) && LightService_AppCBs && LightService_AppCBs->pfnSimpleProfileChange)
    {
        LightService_AppCBs->pfnSimpleProfileChange((const uint8_t *) LightServiceServUUID, notifyApp, len, connHandle);
    }

    return status;
}

/*********************************************************************
 * @fn      AuthorizationCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   opcode
 *
 * @return  SUCCESS, blePending or Failure
 ********************************************************************/
static bStatus_t gatt_LightService_AuthorizationCB(uint16_t connHandle, gattAttribute_t * pAttr, uint8_t opcode)
{
    return SUCCESS;
}

/*********************************************************************
 * @fn      GetParamId
 *
 * @brief   Return service specific mapping from Characteristic UUID to parameter ID
 *
 * @param   pointer to 16 byte characteristic uuid
 *
 * @return  Valid Paramerter ID, 0xFF on Failure
 ********************************************************************/
uint8_t gatt_LightService_GetParamId(const uint8_t * uuid)
{
    uint8_t paramid = 0xff;
    if (!memcmp(uuid, LightServiceLightOnOffUUID, ATT_UUID_SIZE))
    {
        paramid = LIGHTSERVICE_LIGHTONOFF;
    }
    else if (!memcmp(uuid, LightServiceLightToggleUUID, ATT_UUID_SIZE))
    {
        paramid = LIGHTSERVICE_LIGHTTOGGLE;
    }
    else if (!memcmp(uuid, LightServiceLightStateUUID, ATT_UUID_SIZE))
    {
        paramid = LIGHTSERVICE_LIGHTSTATE;
    }
    else if (!memcmp(uuid, clientCharCfgUUID, ATT_BT_UUID_SIZE))
    {
        paramid = LIGHTSERVICE_LIGHTSTATE_CCC;
    }
    return paramid;
}