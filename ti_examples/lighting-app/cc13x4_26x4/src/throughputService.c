/**********************************************************************************************
 * Filename:       throughputService.c
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
 *************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <string.h>

#include <icall.h>

/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"

#include "throughputService.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// throughputService Service UUID
CONST uint8_t throughputServiceUUID[ATT_UUID_SIZE] = { TI_BASE_UUID_128(THROUGHPUTSERVICE_SERV_UUID) };

// throughputEnable UUID
CONST uint8_t throughputService_ThroughputEnableUUID[ATT_UUID_SIZE] = { TI_BASE_UUID_128(THROUGHPUTSERVICE_THROUGHPUTENABLE_UUID) };
// MatterEnable UUID
CONST uint8_t throughputService_MatterEnableUUID[ATT_UUID_SIZE] = { TI_BASE_UUID_128(THROUGHPUTSERVICE_MATTERENABLE_UUID) };
// ThroughputTx UUID
CONST uint8_t throughputService_ThroughputTxUUID[ATT_UUID_SIZE] = { TI_BASE_UUID_128(THROUGHPUTSERVICE_THROUGHPUTTX_UUID) };
// ThrougputRx UUID
CONST uint8_t throughputService_ThrougputRxUUID[ATT_UUID_SIZE] = { TI_BASE_UUID_128(THROUGHPUTSERVICE_THROUGPUTRX_UUID) };

/*********************************************************************
 * LOCAL VARIABLES
 */

static throughputServiceCBs_t * throughputServiceAppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

// Service declaration
static CONST gattAttrType_t throughputServiceDecl = { ATT_UUID_SIZE, throughputServiceUUID };

// Characteristic "ThroughputEnable" Properties (for declaration)
static uint8_t throughputService_ThroughputEnableProps = GATT_PROP_WRITE;
// Characteristic "ThroughputEnable" Value variable
static uint8_t throughputService_ThroughputEnableVal[THROUGHPUTSERVICE_THROUGHPUTENABLE_LEN] = { 0 };
// Characteristic "ThroughputEnable" description
static uint8_t throughputService_ThroughputEnableDesp[] = "enable and start upstream throughput calculation";

// Characteristic "MatterEnable" Properties (for declaration)
static uint8_t throughputService_MatterEnableProps = GATT_PROP_WRITE;
// Characteristic "MatterEnable" Value variable
static uint8_t throughputService_MatterEnableVal[THROUGHPUTSERVICE_MATTERENABLE_LEN] = { 0 };
// Characteristic "MatterEnable" description
static uint8_t throughputService_MatterEnableDesp[] = "enable(1)/disable(0) matter over thread";

// Characteristic "ThroughputTx" Properties (for declaration)
static uint8_t throughputService_ThroughputTxProps = GATT_PROP_READ | GATT_PROP_NOTIFY;
// Characteristic "ThroughputTx" Value variable
static uint8_t throughputService_ThroughputTxVal[THROUGHPUTSERVICE_THROUGHPUTTX_LEN] = { 0 };
// Characteristic "ThroughputTx" CCCD
static gattCharCfg_t * throughputService_ThroughputTxConfig;
// Characteristic "ThroughputTx" description
static uint8_t throughputService_ThroughputTxDesp[] = "Receive upstream data from server";

// Characteristic "ThrougputRx" Properties (for declaration)
static uint8_t throughputService_ThrougputRxProps = GATT_PROP_WRITE;
// Characteristic "ThrougputRx" Value variable
static uint8_t throughputService_ThrougputRxVal[THROUGHPUTSERVICE_THROUGPUTRX_LEN] = { 0 };
// Characteristic "HeapFree" description
static uint8_t throughputService_ThrougputRxDesp[] = "send downstream data from client to server, and start calculation";

/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t throughputServiceAttrTbl[] = {
    // throughputService Service Declaration
    { { ATT_BT_UUID_SIZE, primaryServiceUUID }, GATT_PERMIT_READ, 0, (uint8_t *) &throughputServiceDecl },
    // ThroughputEnable Characteristic Declaration
    { { ATT_BT_UUID_SIZE, characterUUID }, GATT_PERMIT_READ, 0, &throughputService_ThroughputEnableProps },
    // ThroughputEnable Characteristic Value
    { { ATT_UUID_SIZE, throughputService_ThroughputEnableUUID }, GATT_PERMIT_WRITE, 0, throughputService_ThroughputEnableVal },
    // ThroughputEnable Characteristic Description
    { { ATT_BT_UUID_SIZE, charUserDescUUID }, GATT_PERMIT_READ, 0, throughputService_ThroughputEnableDesp },
    // MatterEnable Characteristic Declaration
    { { ATT_BT_UUID_SIZE, characterUUID }, GATT_PERMIT_READ, 0, &throughputService_MatterEnableProps },
    // MatterEnable Characteristic Value
    { { ATT_UUID_SIZE, throughputService_MatterEnableUUID }, GATT_PERMIT_WRITE, 0, throughputService_MatterEnableVal },
    // MatterEnable Characteristic Description
    { { ATT_BT_UUID_SIZE, charUserDescUUID }, GATT_PERMIT_READ, 0, throughputService_MatterEnableDesp },
    // ThroughputTx Characteristic Declaration
    { { ATT_BT_UUID_SIZE, characterUUID }, GATT_PERMIT_READ, 0, &throughputService_ThroughputTxProps },
    // ThroughputTx Characteristic Value
    { { ATT_UUID_SIZE, throughputService_ThroughputTxUUID }, GATT_PERMIT_READ, 0, throughputService_ThroughputTxVal },
    // ThroughputTx CCCD
    { { ATT_BT_UUID_SIZE, clientCharCfgUUID },
      GATT_PERMIT_READ | GATT_PERMIT_WRITE,
      0,
      (uint8 *) &throughputService_ThroughputTxConfig },
    // ThroughputTx Characteristic Description
    { { ATT_BT_UUID_SIZE, charUserDescUUID }, GATT_PERMIT_READ, 0, throughputService_ThroughputTxDesp },
    // ThrougputRx Characteristic Declaration
    { { ATT_BT_UUID_SIZE, characterUUID }, GATT_PERMIT_READ, 0, &throughputService_ThrougputRxProps },
    // ThrougputRx Characteristic Value
    { { ATT_UUID_SIZE, throughputService_ThrougputRxUUID }, GATT_PERMIT_WRITE, 0, throughputService_ThrougputRxVal },
    // ThrougputRx Characteristic Description
    { { ATT_BT_UUID_SIZE, charUserDescUUID }, GATT_PERMIT_READ, 0, throughputService_ThrougputRxDesp },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t throughputService_ReadAttrCB(uint16_t connHandle, gattAttribute_t * pAttr, uint8_t * pValue, uint16_t * pLen,
                                              uint16_t offset, uint16_t maxLen, uint8_t method);
static bStatus_t throughputService_WriteAttrCB(uint16_t connHandle, gattAttribute_t * pAttr, uint8_t * pValue, uint16_t len,
                                               uint16_t offset, uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Simple Profile Service Callbacks
CONST gattServiceCBs_t throughputServiceCBs = {
    throughputService_ReadAttrCB,  // Read callback function pointer
    throughputService_WriteAttrCB, // Write callback function pointer
    NULL                           // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*
 * ThroughputService_AddService- Initializes the ThroughputService service by registering
 *          GATT attributes with the GATT server.
 *
 */
extern bStatus_t ThroughputService_AddService(uint32 services)
{
    uint8_t status;

    // Allocate Client Characteristic Configuration table
    throughputService_ThroughputTxConfig = (gattCharCfg_t *) ICall_malloc(sizeof(gattCharCfg_t) * linkDBNumConns);
    if (throughputService_ThroughputTxConfig == NULL)
    {
        return (bleMemAllocError);
    }

    // Initialize Client Characteristic Configuration attributes
    GATTServApp_InitCharCfg(LINKDB_CONNHANDLE_INVALID, throughputService_ThroughputTxConfig);
    if (services)
    {
        // Register GATT attribute list and CBs with GATT Server App
        status = GATTServApp_RegisterService(throughputServiceAttrTbl, GATT_NUM_ATTRS(throughputServiceAttrTbl),
                                             GATT_MAX_ENCRYPT_KEY_SIZE, &throughputServiceCBs);
    }
    else
    {
        status = SUCCESS;
    }

    return (status);
}

/*
 * ThroughputService_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
bStatus_t ThroughputService_RegisterAppCBs(throughputServiceCBs_t * appCallbacks)
{
    if (appCallbacks)
    {
        throughputServiceAppCBs = appCallbacks;

        return (SUCCESS);
    }
    else
    {
        return (bleAlreadyInRequestedMode);
    }
}

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
bStatus_t ThroughputService_SetParameter(uint8_t param, uint16_t len, void * value)
{
    bStatus_t ret = SUCCESS;
    switch (param)
    {
    case THROUGHPUTSERVICE_THROUGHPUTTX_ID:
        if (len == THROUGHPUTSERVICE_THROUGHPUTTX_LEN)
        {
            memcpy(throughputService_ThroughputTxVal, value, len);

            // Try to send notification.
            GATTServApp_ProcessCharCfg(throughputService_ThroughputTxConfig, (uint8_t *) &throughputService_ThroughputTxVal, FALSE,
                                       throughputServiceAttrTbl, GATT_NUM_ATTRS(throughputServiceAttrTbl), INVALID_TASK_ID,
                                       throughputService_ReadAttrCB);
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

/*
 * ThroughputService_GetParameter - Get a ThroughputService parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
bStatus_t ThroughputService_GetParameter(uint8_t param, uint8_t len, void * value)
{
    bStatus_t ret = SUCCESS;
    switch (param)
    {
    case THROUGHPUTSERVICE_THROUGHPUTENABLE_ID:
        memcpy(value, throughputService_ThroughputEnableVal, THROUGHPUTSERVICE_THROUGHPUTENABLE_LEN);
        break;

    case THROUGHPUTSERVICE_MATTERENABLE_ID:
        memcpy(value, throughputService_MatterEnableVal, THROUGHPUTSERVICE_MATTERENABLE_LEN);
        break;

    case THROUGHPUTSERVICE_THROUGPUTRX_ID:
        memcpy(value, throughputService_ThrougputRxVal, THROUGHPUTSERVICE_THROUGPUTRX_LEN);
        break;

    default:
        ret = INVALIDPARAMETER;
        break;
    }
    return ret;
}

/*********************************************************************
 * @fn          throughputService_ReadAttrCB
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
 */
static bStatus_t throughputService_ReadAttrCB(uint16_t connHandle, gattAttribute_t * pAttr, uint8_t * pValue, uint16_t * pLen,
                                              uint16_t offset, uint16_t maxLen, uint8_t method)
{
    bStatus_t status = SUCCESS;

    // See if request is regarding the ThroughputTx Characteristic Value
    if (!memcmp(pAttr->type.uuid, throughputService_ThroughputTxUUID, pAttr->type.len))
    {
        if (offset > THROUGHPUTSERVICE_THROUGHPUTTX_LEN) // Prevent malicious ATT ReadBlob offsets.
        {
            status = ATT_ERR_INVALID_OFFSET;
        }
        else
        {
            *pLen = MIN(maxLen, THROUGHPUTSERVICE_THROUGHPUTTX_LEN - offset); // Transmit as much as possible
            memcpy(pValue, pAttr->pValue + offset, *pLen);
        }
    }
    else
    {
        // If we get here, that means you've forgotten to add an if clause for a
        // characteristic value attribute in the attribute table that has READ permissions.
        *pLen  = 0;
        status = ATT_ERR_ATTR_NOT_FOUND;
    }

    return status;
}

/*********************************************************************
 * @fn      throughputService_WriteAttrCB
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
 */
static bStatus_t throughputService_WriteAttrCB(uint16_t connHandle, gattAttribute_t * pAttr, uint8_t * pValue, uint16_t len,
                                               uint16_t offset, uint8_t method)
{
    bStatus_t status          = SUCCESS;
    const uint8_t * notifyApp = NULL;

    // See if request is regarding a Client Characterisic Configuration
    if (!memcmp(pAttr->type.uuid, clientCharCfgUUID, pAttr->type.len))
    {
        // Allow only notifications.
        status = GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue, len, offset, GATT_CLIENT_CFG_NOTIFY);
    }
    // See if request is regarding the ThroughputEnable Characteristic Value
    else if (!memcmp(pAttr->type.uuid, throughputService_ThroughputEnableUUID, pAttr->type.len))
    {
        if (offset + len > THROUGHPUTSERVICE_THROUGHPUTENABLE_LEN)
        {
            status = ATT_ERR_INVALID_OFFSET;
        }
        else
        {
            // Copy pValue into the variable we point to from the attribute table.
            memcpy(pAttr->pValue + offset, pValue, len);

            // Only notify application if entire expected value is written
            if (offset + len == THROUGHPUTSERVICE_THROUGHPUTENABLE_LEN)
                notifyApp = throughputService_ThroughputEnableUUID;
        }
    }
    // See if request is regarding the MatterEnable Characteristic Value
    else if (!memcmp(pAttr->type.uuid, throughputService_MatterEnableUUID, pAttr->type.len))
    {
        if (offset + len > THROUGHPUTSERVICE_MATTERENABLE_LEN)
        {
            status = ATT_ERR_INVALID_OFFSET;
        }
        else
        {
            // Copy pValue into the variable we point to from the attribute table.
            memcpy(pAttr->pValue + offset, pValue, len);

            // Only notify application if entire expected value is written
            if (offset + len == THROUGHPUTSERVICE_MATTERENABLE_LEN)
                notifyApp = throughputService_MatterEnableUUID;
        }
    }
    // See if request is regarding the ThrougputRx Characteristic Value
    else if (!memcmp(pAttr->type.uuid, throughputService_ThrougputRxUUID, pAttr->type.len))
    {
        if (offset + len > THROUGHPUTSERVICE_THROUGPUTRX_LEN)
        {
            status = ATT_ERR_INVALID_OFFSET;
        }
        else
        {
            // Copy pValue into the variable we point to from the attribute table.
            memcpy(pAttr->pValue + offset, pValue, len);

            // Only notify application if entire expected value is written
            if (offset + len == THROUGHPUTSERVICE_THROUGPUTRX_LEN)
                notifyApp = throughputService_ThrougputRxUUID;
        }
    }
    else
    {
        // If we get here, that means you've forgotten to add an if clause for a
        // characteristic value attribute in the attribute table that has WRITE permissions.
        status = ATT_ERR_ATTR_NOT_FOUND;
    }

    if (throughputServiceAppCBs && throughputServiceAppCBs->pfnCfgChangeCb)
    {
        throughputServiceAppCBs->pfnCfgChangeCb(); // Call app function from stack task context.
    }

    // If a characteristic value changed then callback function to notify application of change
    if ((notifyApp != NULL) && throughputServiceAppCBs && throughputServiceAppCBs->pfnChangeCb)
    {
        throughputServiceAppCBs->pfnChangeCb((const uint8_t *) throughputServiceUUID, notifyApp, len, connHandle);
    }

    return status;
}

/*********************************************************************
 * @fn      GetParamId
 *
 * @brief   Return service specific mapping from Characteristic UUID to parameter ID
 *
 * @param   pointer to 4 byte characteristic uuid
 *
 * @return  Valid Paramerter ID, 0xFF on Failure
 ********************************************************************/
uint8_t throughputService_GetParamId(const uint8_t * uuid)
{
    uint8_t paramid = 0xff;
    if (!memcmp(uuid, throughputService_ThroughputEnableUUID, ATT_UUID_SIZE))
    {
        paramid = THROUGHPUTSERVICE_THROUGHPUTENABLE_ID;
    }
    else if (!memcmp(uuid, throughputService_MatterEnableUUID, ATT_UUID_SIZE))
    {
        paramid = THROUGHPUTSERVICE_MATTERENABLE_ID;
    }
    else if (!memcmp(uuid, throughputService_ThroughputTxUUID, ATT_UUID_SIZE))
    {
        paramid = THROUGHPUTSERVICE_THROUGHPUTTX_ID;
    }
    else if (!memcmp(uuid, throughputService_ThrougputRxUUID, ATT_UUID_SIZE))
    {
        paramid = THROUGHPUTSERVICE_THROUGPUTRX_ID;
    }
    return paramid;
}