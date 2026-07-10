/******************************************************************************

 @file  ble_user_config.c

 @brief This file contains user configurable variables for the BLE
        Application.

 Group: WCS, BTS
 Target Device: cc23xx

 ******************************************************************************
 
 Copyright (c) 2016-2025, Texas Instruments Incorporated
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

/*******************************************************************************
 * INCLUDES
 */
#include "ti/ble/stack_util/comdef.h"
#include "ti/ble/app_util/config/ble_user_config.h"
#include <ti/drivers/AESCCM.h>
#include <ti/drivers/AESECB.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
#include <ti/drivers/utils/Random.h>

#include <ti/drivers/aesccm/AESCCMXXF3.h>
#include <ti/drivers/aesecb/AESECBXXF3.h>
#include <ti/drivers/RNG.h>

#if defined( HOST_CONFIG ) && ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
#include "ti/ble/host/l2cap/l2cap.h"
#endif // ( CENTRAL_CFG | PERIPHERAL_CFG )

#ifdef SYSCFG
#include "ti_ble_config.h"
#else
#ifndef CONTROLLER_ONLY
#include "ti/ble/host/gapbondmgr/gapbondmgr.h"
#endif // CONTROLLER_ONLY
#endif // SYSCFG


#ifdef CONFIG_ZEPHYR
#include "rcl_settings_ble.h"
#else
#include "ti_radio_config.h"
#endif // CONFIG_ZEPHYR

#define Swi_restore SwiP_restore
#define Swi_disable SwiP_disable
#include <ti/drivers/dpl/SwiP.h>

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

#ifndef SYSCFG
// Default Tx Power dBm value
#define DEFAULT_TX_POWER               0
#endif // SYSCFG

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */
pfnBMAlloc_t pfnBMAlloc = NULL;
pfnBMFree_t  pfnBMFree = NULL;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

void driverTable_fnSpinlock(void);

/*******************************************************************************
 * GLOBAL VARIABLES
 */


// The Tx Power value
int8 defaultTxPowerDbm = DEFAULT_TX_POWER;

ECCParams_CurveParams eccParams_NISTP256 = {
    .curveType      = ECCParams_CURVE_TYPE_SHORT_WEIERSTRASS_AN3,
    .length         = ECCParams_NISTP256_LENGTH,
    .prime          = ECC_NISTP256_prime.byte,
    .order          = ECC_NISTP256_order.byte,
    .a              = ECC_NISTP256_a.byte,
    .b              = ECC_NISTP256_b.byte,
    .generatorX     = ECC_NISTP256_generatorX.byte,
    .generatorY     = ECC_NISTP256_generatorY.byte,
    .cofactor       = 1
};

#include "ti/ble/stack_util/icall/app/icall.h"

// BLE Stack Configuration Structure
const stackSpecific_t bleStackConfig =
{
  .maxNumConns                          = MAX_NUM_BLE_CONNS,
  .maxNumPDUs                           = MAX_NUM_PDU,
  .maxPduSize                           = 0,
  .maxNumPSM                            = L2CAP_NUM_PSM,
  .maxNumCoChannels                     = L2CAP_NUM_CO_CHANNELS,
  .maxAcceptListElems                   = CFG_MAX_NUM_AL_ENTRIES,
  .maxResolvListElems                   = CFG_MAX_NUM_RL_ENTRIES,
  .pfnBMAlloc                           = &pfnBMAlloc,
  .pfnBMFree                            = &pfnBMFree,
  .eccParams                            = &eccParams_NISTP256,
  .fastStateUpdateCb                    = NULL,
  .bleStackType                         = 0,
  .extStackSettings                     = EXTENDED_STACK_SETTINGS,
  .advReportIncChannel                  = ADV_RPT_INC_CHANNEL,
  .rssiMonitorMaxCliNum                 = 4,
  .rssiMonitorMaxRssiWeight             = 10,
  .pwrCtrlRssiLowThreshold              = -85,
  .pwrCtrlRssiHighThreshold             = 0,
  .pwrCtrlDeltaStepDb                   = 10
};

uint16_t bleUserCfg_maxPduSize = MAX_PDU_SIZE;

// SDAA module configuration - default values
sdaaUsrCfg_t sdaaCfgTable =
{
  .rxWindowDuration        = SDAA_RX_WINDOW_DURATION,
  .txUsageLimitPercent     = SDAA_MAX_THRESHOLD,
  .blockingchanneltime     = SDAA_MAX_BLOCKED_CHANNEL_TIME,
  .dynamicObservationTime  = SDAA_CONST_OBSERV_TIME,
  .observationtime         = SDAA_OBSERVATION_TIME,
  .externalTxPowerGain     = SDAA_TX_POWER_GAIN,
  .externalRxPowerGain     = SDAA_RX_POWER_GAIN,
};

/*******************************************************************************
 * FUNCTIONS
 */

/*******************************************************************************
 * @fn      setBleUserConfig
 *
 * @brief   Set the user configurable variables for the BLE
 *          Controller and Host.
 *
 *          Note: This function should be called at the start
 *                of stack_main.
 *
 * @param   userCfg - pointer to user configuration
 *
 * @return  none
 */
void setBleUserConfig( icall_userCfg_t *userCfg )
{
  if ( userCfg != NULL )
  {
    stackSpecific_t *stackConfig = (stackSpecific_t*) userCfg->stackConfig;

#if defined( HOST_CONFIG ) && ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
    l2capUserCfg_t l2capUserCfg;

    // user reconfiguration of Host variables
    l2capUserCfg.maxNumPSM = stackConfig->maxNumPSM;
    l2capUserCfg.maxNumCoChannels = stackConfig->maxNumCoChannels;

    L2CAP_SetUserConfig( &l2capUserCfg );

#endif // ( CENTRAL_CFG | PERIPHERAL_CFG )
    if ( stackConfig->pfnBMAlloc != NULL )
    {
      *stackConfig->pfnBMAlloc = bleDispatch_BMAlloc;
    }

    if ( stackConfig->pfnBMFree != NULL )
    {
      *stackConfig->pfnBMFree = bleDispatch_BMFree;
    }

    // user reconfiguration of Controller variables
    llUserConfig.maxNumConns   = stackConfig->maxNumConns;
    llUserConfig.numTxEntries  = stackConfig->maxNumPDUs;
    llUserConfig_maxPduSize    = bleUserCfg_maxPduSize;
    llUserConfig.maxAlElems    = stackConfig->maxAcceptListElems;
    llUserConfig.maxRlElems    = stackConfig->maxResolvListElems;
    llUserConfig.advReportIncChannel = stackConfig->advReportIncChannel;

    // ECC Driver Parameter
    llUserConfig.eccCurveParams     = stackConfig->eccParams;

    // Fast State Update Callback
    llUserConfig.fastStateUpdateCb = stackConfig->fastStateUpdateCb;

    // BLE Stack Type
    llUserConfig.bleStackType = stackConfig->bleStackType;

    // Extended stack settings
    llUserConfig.extStackSettings = stackConfig->extStackSettings;


    llUserConfig.lrfTxPowerTablePtr = &LRF_txPowerTable;
    llUserConfig.lrfConfigPtr = &LRF_config;

    // Multiple antennas
#if defined NUM_ANTENNAS
    llUserConfig.numAntennas        = NUM_ANTENNAS;
#else
    llUserConfig.numAntennas        = 1;
#endif

    // Antennas muxing values. 0xFF by default (invalid)
    llUserConfig.antennasMuxValues  = LL_ANTENNAS_MUX_INVALID;

#ifdef CHANNEL_SOUNDING
    llUserConfig.lrfConfigCsPtr = &LRF_configBleCsHp;

#if defined NUM_ANTENNAS && NUM_ANTENNAS > 1

#if !defined(CONFIG_PBEGPO2) || !defined(CONFIG_PBEGPO3)
    #error More than one antenna are configured but PBEGPO2 or PBEGPO3 are not defined in RCL. Please make sure to enable PBEGPO2 and PBEGPO3 in RCL Observables.
#endif

    llUserConfig.numAntennas        = NUM_ANTENNAS;
    llUserConfig.antennasMuxValues  = ANT_MUX_VALUES;
#endif

    llUserConfig.csStepsResultsFormat = CS_STEPS_RESULTS_FORMAT;
#endif

    llUserConfig.defaultTxPowerDbm = defaultTxPowerDbm;
    llUserConfig.defaultTxPowerFraction = FALSE;
    llUserConfig.rclPhyFeature1MBPS = RCL_PHY_FEATURE_SUB_PHY_1_MBPS;
    llUserConfig.rclPhyFeature2MBPS = RCL_PHY_FEATURE_SUB_PHY_2_MBPS;
    llUserConfig.rclPhyFeatureCoded = RCL_PHY_FEATURE_SUB_PHY_CODED;
    llUserConfig.rclPhyFeatureCodedS8 = RCL_PHY_FEATURE_CODED_TX_RATE_S8;
    llUserConfig.rclPhyFeatureCodedS2 = RCL_PHY_FEATURE_CODED_TX_RATE_S2;

#ifdef SDAA_ENABLE
    llUserConfig.sdaaCfgPtr = &sdaaCfgTable; // SDAA module configuration
#endif

    // save off the application's assert handler
    halAssertInit( **userCfg->appServiceInfo->assertCback, HAL_ASSERT_LEGACY_MODE_ENABLED );

    osal_timer_init( userCfg->appServiceInfo->timerTickPeriod , userCfg->appServiceInfo->timerMaxMillisecond );
  }
  else
  {
      LL_ASSERT( FALSE );
  }

  llUserConfig.useSrcClkLFOSC = SRC_CLK_IS_LFOSC;
  llUserConfig.cfgLFOSCExtraPPM = USER_CFG_LFOSC_EXTRA_PPM;

#ifdef USE_AE
  llUserConfig.useAE = TRUE;
#else
  llUserConfig.useAE = FALSE;
#endif

  return;
}

/*******************************************************************************
 * @fn          RegisterAssertCback
 *
 * @brief       This routine registers the Application's assert handler.
 *
 * input parameters
 *
 * @param       appAssertHandler - Application's assert handler.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void RegisterAssertCback(assertCback_t appAssertHandler)
{
  appAssertCback = appAssertHandler;

#ifdef EXT_HAL_ASSERT
  // also set the Assert callback pointer used by halAssertHandlerExt
  // Note: Normally, this pointer will be intialized by the stack, but in the
  //       event HAL_ASSERT is used by the Application, we initialize it
  //       directly here.
  halAssertCback = appAssertHandler;
#endif // EXT_HAL_ASSERT

  return;
}

/*******************************************************************************
 * @fn          driverTable_fnSpinLock
 *
 * @brief       This routine is used to trap calls to unpopulated indexes of
 *              driver function pointer tables.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void driverTable_fnSpinlock(void)
{
  volatile uint8 i = 1;

  while(i);
}

/*******************************************************************************
 * @fn          DefaultAssertCback
 *
 * @brief       This is the Application default assert callback, in the event
 *              none is registered.
 *
 * input parameters
 *
 * @param       assertCause    - Assert cause as defined in assert.h.
 * @param       assertSubCause - Optional assert subcause (see assert.h).
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void DefaultAssertCback(uint8 assertCause, uint8 assertSubCause)
{
#ifdef HAL_ASSERT_SPIN
  driverTable_fnSpinlock();
#endif // HAL_ASSERT_SPIN

  return;
}

// Application Assert Callback Function Pointer
assertCback_t appAssertCback = DefaultAssertCback;

/*******************************************************************************
 */
