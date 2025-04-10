/**********************************************************************************************
 * Filename:       diagnosticService.c
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
#include "util.h"
#include <string.h>

#include <icall.h>

/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"

#include "diagnosticService.h"

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

// diagnosticService Service UUID
CONST uint8_t diagnosticServiceUUID[ATT_UUID_SIZE] = { TI_BASE_UUID_128(DIAGNOSTICSERVICE_SERV_UUID) };

// heapFree UUID
CONST uint8_t diagnosticService_HeapFreeUUID[ATT_UUID_SIZE] = { TI_BASE_UUID_128(DIAGNOSTICSERVICE_HEAPFREE_UUID) };
// heapUsed UUID
CONST uint8_t diagnosticService_HeapUsedUUID[ATT_UUID_SIZE] = { TI_BASE_UUID_128(DIAGNOSTICSERVICE_HEAPUSED_UUID) };
// threadNetKey UUID
CONST uint8_t diagnosticService_ThreadNetKeyUUID[ATT_UUID_SIZE] = { TI_BASE_UUID_128(DIAGNOSTICSERVICE_THREADNETKEY_UUID) };
// threadPanID UUID
CONST uint8_t diagnosticService_ThreadPanIDUUID[ATT_UUID_SIZE] = { TI_BASE_UUID_128(DIAGNOSTICSERVICE_THREADPANID_UUID) };
// threadChannel UUID
CONST uint8_t diagnosticService_ThreadChannelUUID[ATT_UUID_SIZE] = { TI_BASE_UUID_128(DIAGNOSTICSERVICE_THREADCHANNEL_UUID) };
// refresh UUID
CONST uint8_t diagnosticService_RefreshUUID[ATT_UUID_SIZE] = { TI_BASE_UUID_128(DIAGNOSTICSERVICE_REFRESH_UUID) };
// period UUID
CONST uint8_t diagnosticService_PeriodUUID[ATT_UUID_SIZE] = { TI_BASE_UUID_128(DIAGNOSTICSERVICE_PERIOD_UUID) };

/*********************************************************************
 * LOCAL VARIABLES
 */

diagnosticServiceCBs_t * DiagnosticService_AppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

// Service declaration
static CONST gattAttrType_t diagnosticServiceDecl = { ATT_UUID_SIZE, diagnosticServiceUUID };

// Characteristic "HeapFree" Properties (for declaration)
static uint8_t diagnosticService_HeapFreeProps = GATT_PROP_READ;
// Characteristic "HeapFree" Value variable
static uint8_t diagnosticService_HeapFreeVal[DIAGNOSTICSERVICE_HEAPFREE_LEN] = { 0 };
// Characteristic "HeapFree" description
static uint8_t diagnosticService_HeapFreeDesp[] = "Heap free";

// Characteristic "HeapUsed" Properties (for declaration)
static uint8_t diagnosticService_HeapUsedProps = GATT_PROP_READ;
// Characteristic "HeapUsed" Value variable
static uint8_t diagnosticService_HeapUsedVal[DIAGNOSTICSERVICE_HEAPUSED_LEN] = { 0 };
// Characteristic "HeapUsed" description
static uint8_t diagnosticService_HeapUsedValDesp[] = "Heap used";

// Characteristic "ThreadNetKey" Properties (for declaration)
static uint8_t diagnosticService_ThreadNetKeyProps = GATT_PROP_READ;
// Characteristic "ThreadNetKey" Value variable
static uint8_t diagnosticService_ThreadNetKeyVal[DIAGNOSTICSERVICE_THREADNETKEY_LEN] = { 0 };
// Characteristic "ThreadNetKey" description
static uint8_t diagnosticService_ThreadNetKeyDesp[] = "Thread network key";

// Characteristic "ThreadPanID" Properties (for declaration)
static uint8_t diagnosticService_ThreadPanIDProps = GATT_PROP_READ;
// Characteristic "ThreadPanID" Value variable
static uint8_t diagnosticService_ThreadPanIDVal[DIAGNOSTICSERVICE_THREADPANID_LEN] = { 0 };
// Characteristic "ThreadPanID" Description
static uint8_t diagnosticService_ThreadPanIDDesp[] = "Thread Panid";

// Characteristic "ThreadChannel" Properties (for declaration)
static uint8_t diagnosticService_ThreadChannelProps = GATT_PROP_READ;
// Characteristic "ThreadChannel" Value variable
static uint8_t diagnosticService_ThreadChannelVal[DIAGNOSTICSERVICE_THREADCHANNEL_LEN] = { 0 };
// Characteristic "ThreadChannel" description
static uint8_t diagnosticService_ThreadChannelDesp[] = "Thread channel";

// Characteristic "Refresh" Properties (for declaration)
static uint8_t diagnosticService_RefreshProps = GATT_PROP_WRITE;
// Characteristic "Refresh" Value variable
static uint8_t diagnosticService_RefreshVal[DIAGNOSTICSERVICE_REFRESH_LEN] = { 0 };
// Characteristic "Refresh" description
static uint8_t diagnosticService_RefreshDesp[] = "Refresh Diagnostic values";

// Characteristic "Period" Properties (for declaration)
static uint8_t diagnosticService_PeriodProps = GATT_PROP_WRITE;
// Characteristic "Period" Value variable
static uint8_t diagnosticService_PeriodVal[DIAGNOSTICSERVICE_PERIOD_LEN] = { 0 };
// Characteristic "Period" description
static uint8_t diagnosticService_PeriodDesp[] = "Change Refresh Period";

/*********************************************************************
 * Profile Attributes - Table
 */

// Characteristic Heap_Free User Description

static gattAttribute_t diagnosticServiceAttrTbl[] = {
    // diagnosticService Service Declaration
    { { ATT_BT_UUID_SIZE, primaryServiceUUID }, GATT_PERMIT_READ, 0, (uint8_t *) &diagnosticServiceDecl },
    // HeapFree Characteristic Declaration
    { { ATT_BT_UUID_SIZE, characterUUID }, GATT_PERMIT_READ, 0, &diagnosticService_HeapFreeProps },
    // HeapFree Characteristic Value
    { { ATT_UUID_SIZE, diagnosticService_HeapFreeUUID }, GATT_PERMIT_READ, 0, diagnosticService_HeapFreeVal },
    // HeapFree user description
    { { ATT_BT_UUID_SIZE, charUserDescUUID }, GATT_PERMIT_READ, 0, diagnosticService_HeapFreeDesp },
    // HeapUsed Characteristic Declaration
    { { ATT_BT_UUID_SIZE, characterUUID }, GATT_PERMIT_READ, 0, &diagnosticService_HeapUsedProps },
    // HeapUsed Characteristic Value
    { { ATT_UUID_SIZE, diagnosticService_HeapUsedUUID }, GATT_PERMIT_READ, 0, diagnosticService_HeapUsedVal },
    // Heapused user description
    { { ATT_BT_UUID_SIZE, charUserDescUUID }, GATT_PERMIT_READ, 0, diagnosticService_HeapUsedValDesp },
    // ThreadNetKey Characteristic Declaration
    { { ATT_BT_UUID_SIZE, characterUUID }, GATT_PERMIT_READ, 0, &diagnosticService_ThreadNetKeyProps },
    // ThreadNetKey Characteristic Value
    { { ATT_UUID_SIZE, diagnosticService_ThreadNetKeyUUID }, GATT_PERMIT_READ, 0, diagnosticService_ThreadNetKeyVal },
    // ThreadNetKey user description
    { { ATT_BT_UUID_SIZE, charUserDescUUID }, GATT_PERMIT_READ, 0, diagnosticService_ThreadNetKeyDesp },
    // ThreadPanID Characteristic Declaration
    { { ATT_BT_UUID_SIZE, characterUUID }, GATT_PERMIT_READ, 0, &diagnosticService_ThreadPanIDProps },
    // ThreadPanID Characteristic Value
    { { ATT_UUID_SIZE, diagnosticService_ThreadPanIDUUID }, GATT_PERMIT_READ, 0, diagnosticService_ThreadPanIDVal },
    // ThreadPanID user description
    { { ATT_BT_UUID_SIZE, charUserDescUUID }, GATT_PERMIT_READ, 0, diagnosticService_ThreadPanIDDesp },
    // ThreadChannel Characteristic Declaration
    { { ATT_BT_UUID_SIZE, characterUUID }, GATT_PERMIT_READ, 0, &diagnosticService_ThreadChannelProps },
    // ThreadChannel Characteristic Value
    { { ATT_UUID_SIZE, diagnosticService_ThreadChannelUUID }, GATT_PERMIT_READ, 0, diagnosticService_ThreadChannelVal },
    // ThreadChannel user description
    { { ATT_BT_UUID_SIZE, charUserDescUUID }, GATT_PERMIT_READ, 0, diagnosticService_ThreadChannelDesp },
    // Refresh Characteristic Declaration
    { { ATT_BT_UUID_SIZE, characterUUID }, GATT_PERMIT_READ, 0, &diagnosticService_RefreshProps },
    // Refresh Characteristic Value
    { { ATT_UUID_SIZE, diagnosticService_RefreshUUID }, GATT_PERMIT_WRITE, 0, diagnosticService_RefreshVal },
    // Refresh user description
    { { ATT_BT_UUID_SIZE, charUserDescUUID }, GATT_PERMIT_READ, 0, diagnosticService_RefreshDesp },
    // Period Characteristic Declaration
    { { ATT_BT_UUID_SIZE, characterUUID }, GATT_PERMIT_READ, 0, &diagnosticService_PeriodProps },
    // Period Characteristic Value
    { { ATT_UUID_SIZE, diagnosticService_PeriodUUID }, GATT_PERMIT_WRITE, 0, diagnosticService_PeriodVal },
    // Period user description
    { { ATT_BT_UUID_SIZE, charUserDescUUID }, GATT_PERMIT_READ, 0, diagnosticService_PeriodDesp },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t diagnosticService_ReadAttrCB(uint16_t connHandle, gattAttribute_t * pAttr, uint8_t * pValue, uint16_t * pLen,
                                              uint16_t offset, uint16_t maxLen, uint8_t method);
static bStatus_t diagnosticService_WriteAttrCB(uint16_t connHandle, gattAttribute_t * pAttr, uint8_t * pValue, uint16_t len,
                                               uint16_t offset, uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Simple Profile Service Callbacks
CONST gattServiceCBs_t diagnosticServiceCBs = {
    diagnosticService_ReadAttrCB,  // Read callback function pointer
    diagnosticService_WriteAttrCB, // Write callback function pointer
    NULL                           // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*
 * DiagnosticService_AddService- Initializes the DiagnosticService service by registering
 *          GATT attributes with the GATT server.
 *
 */
extern bStatus_t DiagnosticService_AddService(uint32 services)
{
    uint8_t status;

    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService(diagnosticServiceAttrTbl, GATT_NUM_ATTRS(diagnosticServiceAttrTbl),
                                         GATT_MAX_ENCRYPT_KEY_SIZE, &diagnosticServiceCBs);

    return (status);
}

/*
 * DiagnosticService_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
bStatus_t DiagnosticService_RegisterAppCBs(diagnosticServiceCBs_t * appCallbacks)
{
    if (appCallbacks)
    {
        DiagnosticService_AppCBs = appCallbacks;

        return (SUCCESS);
    }
    else
    {
        return (bleAlreadyInRequestedMode);
    }
}

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
bStatus_t DiagnosticService_SetParameter(uint8_t param, uint16_t len, void * value)
{
    bStatus_t ret = SUCCESS;
    switch (param)
    {
    case DIAGNOSTICSERVICE_HEAPFREE_ID:
        if (len == DIAGNOSTICSERVICE_HEAPFREE_LEN)
        {
            memcpy(diagnosticService_HeapFreeVal, value, len);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case DIAGNOSTICSERVICE_HEAPUSED_ID:
        if (len == DIAGNOSTICSERVICE_HEAPUSED_LEN)
        {
            memcpy(diagnosticService_HeapUsedVal, value, len);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case DIAGNOSTICSERVICE_THREADNETKEY_ID:
        if (len == DIAGNOSTICSERVICE_THREADNETKEY_LEN)
        {
            memcpy(diagnosticService_ThreadNetKeyVal, value, len);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case DIAGNOSTICSERVICE_THREADPANID_ID:
        if (len == DIAGNOSTICSERVICE_THREADPANID_LEN)
        {
            memcpy(diagnosticService_ThreadPanIDVal, value, len);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case DIAGNOSTICSERVICE_THREADCHANNEL_ID:
        if (len == DIAGNOSTICSERVICE_THREADCHANNEL_LEN)
        {
            memcpy(diagnosticService_ThreadChannelVal, value, len);
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
 * DiagnosticService_GetParameter - Get a DiagnosticService parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
bStatus_t DiagnosticService_GetParameter(uint8_t param, uint8_t len, void * value)
{
    bStatus_t ret = SUCCESS;
    switch (param)
    {
    case DIAGNOSTICSERVICE_REFRESH_ID:
        memcpy(value, diagnosticService_RefreshVal, DIAGNOSTICSERVICE_REFRESH_LEN);
        break;

    case DIAGNOSTICSERVICE_PERIOD_ID:
        memcpy(value, diagnosticService_PeriodVal, DIAGNOSTICSERVICE_PERIOD_LEN);
        break;

    default:
        ret = INVALIDPARAMETER;
        break;
    }
    return ret;
}

/*********************************************************************
 * @fn          diagnosticService_ReadAttrCB
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
static bStatus_t diagnosticService_ReadAttrCB(uint16_t connHandle, gattAttribute_t * pAttr, uint8_t * pValue, uint16_t * pLen,
                                              uint16_t offset, uint16_t maxLen, uint8_t method)
{
    bStatus_t status = SUCCESS;

    // See if request is regarding the HeapFree Characteristic Value
    if (!memcmp(pAttr->type.uuid, diagnosticService_HeapFreeUUID, pAttr->type.len))
    {
        if (offset > DIAGNOSTICSERVICE_HEAPFREE_LEN) // Prevent malicious ATT ReadBlob offsets.
        {
            status = ATT_ERR_INVALID_OFFSET;
        }
        else
        {
            *pLen = MIN(maxLen, DIAGNOSTICSERVICE_HEAPFREE_LEN - offset); // Transmit as much as possible
            memcpy(pValue, pAttr->pValue + offset, *pLen);
        }
    }
    // See if request is regarding the HeapUsed Characteristic Value
    else if (!memcmp(pAttr->type.uuid, diagnosticService_HeapUsedUUID, pAttr->type.len))
    {
        if (offset > DIAGNOSTICSERVICE_HEAPUSED_LEN) // Prevent malicious ATT ReadBlob offsets.
        {
            status = ATT_ERR_INVALID_OFFSET;
        }
        else
        {
            *pLen = MIN(maxLen, DIAGNOSTICSERVICE_HEAPUSED_LEN - offset); // Transmit as much as possible
            memcpy(pValue, pAttr->pValue + offset, *pLen);
        }
    }
    // See if request is regarding the ThreadNetKey Characteristic Value
    else if (!memcmp(pAttr->type.uuid, diagnosticService_ThreadNetKeyUUID, pAttr->type.len))
    {
        if (offset > DIAGNOSTICSERVICE_THREADNETKEY_LEN) // Prevent malicious ATT ReadBlob offsets.
        {
            status = ATT_ERR_INVALID_OFFSET;
        }
        else
        {
            *pLen = MIN(maxLen, DIAGNOSTICSERVICE_THREADNETKEY_LEN - offset); // Transmit as much as possible
            memcpy(pValue, pAttr->pValue + offset, *pLen);
        }
    }
    // See if request is regarding the ThreadPanID Characteristic Value
    else if (!memcmp(pAttr->type.uuid, diagnosticService_ThreadPanIDUUID, pAttr->type.len))
    {
        if (offset > DIAGNOSTICSERVICE_THREADPANID_LEN) // Prevent malicious ATT ReadBlob offsets.
        {
            status = ATT_ERR_INVALID_OFFSET;
        }
        else
        {
            *pLen = MIN(maxLen, DIAGNOSTICSERVICE_THREADPANID_LEN - offset); // Transmit as much as possible
            memcpy(pValue, pAttr->pValue + offset, *pLen);
        }
    }
    // See if request is regarding the ThreadChannel Characteristic Value
    else if (!memcmp(pAttr->type.uuid, diagnosticService_ThreadChannelUUID, pAttr->type.len))
    {
        if (offset > DIAGNOSTICSERVICE_THREADCHANNEL_LEN) // Prevent malicious ATT ReadBlob offsets.
        {
            status = ATT_ERR_INVALID_OFFSET;
        }
        else
        {
            *pLen = MIN(maxLen, DIAGNOSTICSERVICE_THREADCHANNEL_LEN - offset); // Transmit as much as possible
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
 * @fn      diagnosticService_WriteAttrCB
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
static bStatus_t diagnosticService_WriteAttrCB(uint16_t connHandle, gattAttribute_t * pAttr, uint8_t * pValue, uint16_t len,
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
    // See if request is regarding the Refresh Characteristic Value
    else if (!memcmp(pAttr->type.uuid, diagnosticService_RefreshUUID, pAttr->type.len))
    {
        if (offset + len > DIAGNOSTICSERVICE_REFRESH_LEN)
        {
            status = ATT_ERR_INVALID_OFFSET;
        }
        else
        {
            // Copy pValue into the variable we point to from the attribute table.
            memcpy(pAttr->pValue + offset, pValue, len);

            // Only notify application if entire expected value is written
            if (offset + len == DIAGNOSTICSERVICE_REFRESH_LEN)
                notifyApp = diagnosticService_RefreshUUID;
        }
    }
    // See if request is regarding the Period Characteristic Value
    else if (!memcmp(pAttr->type.uuid, diagnosticService_PeriodUUID, pAttr->type.len))
    {
        if (offset + len > DIAGNOSTICSERVICE_PERIOD_LEN)
        {
            status = ATT_ERR_INVALID_OFFSET;
        }
        else
        {
            // Copy pValue into the variable we point to from the attribute table.
            memcpy(pAttr->pValue + offset, pValue, len);

            // Only notify application if entire expected value is written
            if (offset + len == DIAGNOSTICSERVICE_PERIOD_LEN)
                notifyApp = diagnosticService_PeriodUUID;
        }
    }
    else
    {
        // If we get here, that means you've forgotten to add an if clause for a
        // characteristic value attribute in the attribute table that has WRITE permissions.
        status = ATT_ERR_ATTR_NOT_FOUND;
    }

    if (DiagnosticService_AppCBs && DiagnosticService_AppCBs->pfnCfgChangeCb)
    {
        DiagnosticService_AppCBs->pfnCfgChangeCb(); // Call app function from stack task context.
    }

    // If a characteristic value changed then callback function to notify application of change
    if ((notifyApp != NULL) && DiagnosticService_AppCBs && DiagnosticService_AppCBs->pfnChangeCb)
    {
        DiagnosticService_AppCBs->pfnChangeCb((const uint8_t *) diagnosticServiceUUID, notifyApp, len, connHandle);
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
uint8_t diagnosticService_GetParamId(const uint8_t * uuid)
{
    uint8_t paramid = 0xff;
    if (!memcmp(uuid, diagnosticService_HeapFreeUUID, ATT_UUID_SIZE))
    {
        paramid = DIAGNOSTICSERVICE_HEAPFREE_ID;
    }
    else if (!memcmp(uuid, diagnosticService_HeapUsedUUID, ATT_UUID_SIZE))
    {
        paramid = DIAGNOSTICSERVICE_HEAPUSED_ID;
    }
    else if (!memcmp(uuid, diagnosticService_ThreadNetKeyUUID, ATT_UUID_SIZE))
    {
        paramid = DIAGNOSTICSERVICE_THREADNETKEY_ID;
    }
    else if (!memcmp(uuid, diagnosticService_ThreadPanIDUUID, ATT_UUID_SIZE))
    {
        paramid = DIAGNOSTICSERVICE_THREADPANID_ID;
    }
    else if (!memcmp(uuid, diagnosticService_ThreadChannelUUID, ATT_UUID_SIZE))
    {
        paramid = DIAGNOSTICSERVICE_THREADCHANNEL_ID;
    }
    else if (!memcmp(uuid, diagnosticService_RefreshUUID, ATT_UUID_SIZE))
    {
        paramid = DIAGNOSTICSERVICE_REFRESH_ID;
    }
    else if (!memcmp(uuid, diagnosticService_PeriodUUID, ATT_UUID_SIZE))
    {
        paramid = DIAGNOSTICSERVICE_PERIOD_ID;
    }
    return paramid;
}