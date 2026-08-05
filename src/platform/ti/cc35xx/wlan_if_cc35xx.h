/* Copyright (c) 2024, Texas Instruments Incorporated
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
 */
/**
 *  @defgroup UPPER_MAC upper MAC
 *  @brief This module implements the user interface for the upper MAC
 *  @{
 *
 *  @file  		wlan_if.h
 *  @brief      upper MAC user interface
 *
 */

#ifndef USER_INTEFACE_WLAN_IF_H_
#define USER_INTEFACE_WLAN_IF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "osi_type.h"
#include "osi_kernel.h"
/*****************************************************************************/
/* TYPES DEFINITIONS                                                         */
/*****************************************************************************/

#ifndef NULL
#define NULL        (0)
#endif

#ifndef FALSE
#define FALSE       (0)
#endif

#ifndef TRUE
#define TRUE        (!FALSE)
#endif

#ifndef BIT
#define BIT(x) (1U << (x))
#endif

#define DEVICE_ON (1)
#define DEVICE_OFF (0)
#define WLAN_WAIT_FOREVER               (0xFFFFFFFF)
#define WLAN_WAIT_TIME_ROLE_SWITCH      (180000)/*3 minutes*/
#define WLAN_NO_WAIT                    (0x00000000)
#define WLAN_MAX_SCAN_COUNT                (20)
#define WLAN_EXTENDED_MAX_SCAN_COUNT        (31)
#define WLAN_BSSID_LENGTH                   (6)
#define WLAN_SSID_MAX_LENGTH                (32)
#define WLAN_PMK_KEY_SIZE                   (64)
#define WLAN_DOT11_MAX_SUPPORTED_RATES      (32)
#define WLAN_DOT11_HT_CAPABILITIES_ELE_LEN  (26)
#define WLAN_DOT11_ERP_ELE_LEN              (1)
#define WLAN_MAX_BEACON_BODY_LENGTH         (1600)
#define WLAN_DOT11_MAX_RSNE_SIZE            (257)
#define WLAN_DOT11_MAX_BEACON_BODY_LENGTH   (1600)
#define WLAN_DOT11_MAX_WPSIE_SIZE           (350)

#define WLAN_LEN_OF_IE_HEADER               (2)

#define MAC_ADDRESS_LEN (6)
#define WLAN_MAX_MAC_ADDRESSES (4)
#define CSI_MAX_DATA_LENGTH (27)

/* WLAN Disconnect Reason Codes */
#define WLAN_DISCONNECT_UNSPECIFIED                                         (1)
#define WLAN_DISCONNECT_AUTH_NO_LONGER_VALID                                (2)
#define WLAN_DISCONNECT_DEAUTH_SENDING_STA_LEAVING                          (3)
#define WLAN_DISCONNECT_INACTIVITY                                          (4)
#define WLAN_DISCONNECT_TOO_MANY_STA                                        (5)
#define WLAN_DISCONNECT_FRAME_FROM_NONAUTH_STA                              (6)
#define WLAN_DISCONNECT_FRAME_FROM_NONASSOC_STA                             (7)
#define WLAN_DISCONNECT_DISS_SENDING_STA_LEAVING                            (8)
#define WLAN_DISCONNECT_STA_NOT_AUTH                                        (9)
#define WLAN_DISCONNECT_POWER_CAPABILITY_INVALID                            (10)
#define WLAN_DISCONNECT_SUPPORTED_CHANNELS_INVALID                          (11)
#define WLAN_DISCONNECT_INVALID_IE                                          (13)
#define WLAN_DISCONNECT_MIC_FAILURE                                         (14)
#define WLAN_DISCONNECT_FOURWAY_HANDSHAKE_TIMEOUT                           (15)
#define WLAN_DISCONNECT_GROUPKEY_HANDSHAKE_TIMEOUT                          (16)
#define WLAN_DISCONNECT_REASSOC_INVALID_IE                                  (17)
#define WLAN_DISCONNECT_INVALID_GROUP_CIPHER                                (18)
#define WLAN_DISCONNECT_INVALID_PAIRWISE_CIPHER                             (19)
#define WLAN_DISCONNECT_INVALID_AKMP                                        (20)
#define WLAN_DISCONNECT_UNSUPPORTED_RSN_VERSION                             (21)
#define WLAN_DISCONNECT_INVALID_RSN_CAPABILITIES                            (22)
#define WLAN_DISCONNECT_IEEE_802_1X_AUTHENTICATION_FAILED                   (23)
#define WLAN_DISCONNECT_CIPHER_SUITE_REJECTED                               (24)
#define WLAN_DISCONNECT_DISASSOC_QOS                                        (32)
#define WLAN_DISCONNECT_DISASSOC_QOS_BANDWIDTH                              (33)
#define WLAN_DISCONNECT_DISASSOC_EXCESSIVE_ACK_PENDING                      (34)
#define WLAN_DISCONNECT_DISASSOC_TXOP_LIMIT                                 (35)
#define WLAN_DISCONNECT_STA_LEAVING                                         (36)
#define WLAN_DISCONNECT_STA_DECLINED                                        (37)
#define WLAN_DISCONNECT_STA_UNKNOWN_BA                                      (38)
#define WLAN_DISCONNECT_STA_TIMEOUT                                         (39)
#define WLAN_DISCONNECT_STA_UNSUPPORTED_CIPHER_SUITE                        (40)
#define WLAN_DISCONNECT_ROAMING_TRIGGER_NONE                                (100)
#define WLAN_DISCONNECT_ROAMING_TRIGGER_LOW_TX_RATE                         (104)
#define WLAN_DISCONNECT_ROAMING_TRIGGER_LOW_SNR                             (105)
#define WLAN_DISCONNECT_ROAMING_TRIGGER_LOW_QUALITY                         (106)
#define WLAN_DISCONNECT_ROAMING_TRIGGER_TSPEC_REJECTED                      (107)
#define WLAN_DISCONNECT_ROAMING_TRIGGER_MAX_TX_RETRIES                      (108)
#define WLAN_DISCONNECT_ROAMING_TRIGGER_BSS_LOSS                            (109)
#define WLAN_DISCONNECT_ROAMING_TRIGGER_BSS_LOSS_DUE_TO_MAX_TX_RETRY        (110)
#define WLAN_DISCONNECT_ROAMING_TRIGGER_SWITCH_CHANNEL                      (111)
#define WLAN_DISCONNECT_ROAMING_TRIGGER_AP_DISCONNECT                       (112)
#define WLAN_DISCONNECT_ROAMING_TRIGGER_SECURITY_ATTACK                     (113)
#define WLAN_DISCONNECT_USER_INITIATED                                      (200)
#define WLAN_DISCONNECT_AUTH_TIMEOUT                                        (202)
#define WLAN_DISCONNECT_ASSOC_TIMEOUT                                       (203)
#define WLAN_DISCONNECT_SECURITY_FAILURE                                    (204)
#define WLAN_DISCONNECT_WHILE_CONNNECTING                                   (208)
#define WLAN_DISCONNECT_MISSING_CERT                                        (209)
#define WLAN_DISCONNECT_CERTIFICATE_EXPIRED                                 (210)
#define WLAN_DISCONNECT_AP_REMOVE_PEER                                      (255)


#define WLAN_WPA_PROTO_WPA BIT(0) //WPA_PROTO_WPA
#define WLAN_WPA_PROTO_RSN BIT(1) //WPA_PROTO_RSN
#define WLAN_WPA_PROTO_WAPI BIT(2) //WPA_PROTO_WAPI
#define WLAN_WPA_PROTO_OSEN BIT(3) //WPA_PROTO_OSEN

#define WLAN_SEC_TYPE_OPEN                                                        (0)
#define WLAN_SEC_TYPE_WPA                                                         (2) /* deprecated */
#define WLAN_SEC_TYPE_WPA_WPA2                                                    (2)
#define WLAN_SEC_TYPE_WPS_PBC                                                     (3)
#define WLAN_SEC_TYPE_WPS_PIN                                                     (4)
#define WLAN_SEC_TYPE_WPA_ENT                                                 (5)
#define WLAN_SEC_TYPE_P2P_PBC                                                     (6)
#define WLAN_SEC_TYPE_P2P_PIN_KEYPAD                                              (7)
#define WLAN_SEC_TYPE_P2P_PIN_DISPLAY                                             (8)
#define WLAN_SEC_TYPE_P2P_PIN_AUTO                                                (9) /* NOT Supported yet */
#define WLAN_SEC_TYPE_WPA2_PLUS                                                   (11) /* Support to WPA2 WPA2+PMF (Protected Managmant Frame) networks */
#define WLAN_SEC_TYPE_WPA3                                                        (12) /* Support WPA3 only networks */
#define WLAN_SEC_TYPE_WPA_PMK                                                     (15)
#define WLAN_SEC_TYPE_WPA2_WPA3                                                   (16) /*transition mode WPA2 WPA2+PMF WPA3 */
#define WLAN_SEC_TYPE_WPA2_AES_ONLY_PMF                                           (13) 
#define WLAN_SEC_TYPE_WPA2_AES_ONLY_NO_PMF                                        (14) 


/* AP access List Macros */
#define WLAN_AP_ACCESS_LIST_MODE_DISABLED         0
#define WLAN_AP_ACCESS_LIST_MODE_DENY_LIST        1
#define WLAN_AP_ACCESS_LIST_MODE_ALLOW_LIST       2
#define WLAN_AP_ACCESS_LIST_UPDATE_RAM_FLASH      0
#define WLAN_AP_ACCESS_LIST_UPDATE_RAM_ONLY       1
#define WLAN_MAX_ACCESS_LIST_STATIONS             16
#define WLAN_AP_ACCESS_LIST_WILDCARD_LEN          6


/* Scan results security information */
#define SL_WLAN_SCAN_RESULT_SEC_TYPE_MASK              0x3f
#define SL_WLAN_SCAN_RESULT_SEC_TYPE_POSITION          8

#define SL_WLAN_SCAN_RESULT_GROUP_CIPHER_MASK          0xf
#define SL_WLAN_SCAN_RESULT_GROUP_CIPHER_POSITION      0

#define SL_WLAN_SCAN_RESULT_UNICAST_CIPHER_MASK        0xf
#define SL_WLAN_SCAN_RESULT_UNICAST_CIPHER_POSITION    4

#define SL_WLAN_SCAN_RESULT_KEY_MGMT_MASK              0x3
#define SL_WLAN_SCAN_RESULT_KEY_MGMT_POSITION          14


#define WLAN_SCAN_RESULT_GROUP_CIPHER(SecurityInfo)              (SecurityInfo & 0xF)   /* Possible values: NONE,TKIP,CCMP */
#define WLAN_SCAN_RESULT_UNICAST_CIPHER_BITMAP(SecurityInfo)     ((SecurityInfo & 0xF0) >> 4 ) /* Possible values: NONE,WEP40,WEP104,TKIP,CCMP*/
#define WLAN_SCAN_RESULT_HIDDEN_SSID(SecurityInfo)               (SecurityInfo & 0x2000 ) >> 13 /* Possible values: TRUE/FALSE */
#define WLAN_SCAN_RESULT_KEY_MGMT_SUITES_BITMAP(SecurityInfo)    (SecurityInfo & 0x1800 ) >> 11  /* Possible values: OPEN, PSK*/
#define WLAN_SCAN_RESULT_SEC_TYPE_BITMAP(SecurityInfo)           ((SecurityInfo & (SL_WLAN_SCAN_RESULT_SEC_TYPE_MASK << SL_WLAN_SCAN_RESULT_SEC_TYPE_POSITION)) >> SL_WLAN_SCAN_RESULT_SEC_TYPE_POSITION)  /* Possible values: SL_WLAN_SECURITY_TYPE_BITMAP_OPEN, SL_WLAN_SECURITY_TYPE_BITMAP_WEP, SL_WLAN_SECURITY_TYPE_BITMAP_WPA, SL_WLAN_SECURITY_TYPE_BITMAP_WPA2, 0x6 (mix mode) SL_WLAN_SECURITY_TYPE_BITMAP_WPA | SL_WLAN_SECURITY_TYPE_BITMAP_WPA2 */
#define WLAN_SCAN_RESULT_PMF_ENABLE(SecurityInfo)                ((SecurityInfo & 0x4000  ) >> 14)  /* Possible values: TRUE/FALSE */
#define WLAN_SCAN_RESULT_PMF_REQUIRED(SecurityInfo)              ((SecurityInfo & 0x8000  ) >> 15)  /* Possible values: TRUE/FALSE */

#define WLAN_BEACON_MAX_SIZE	450
#define UPPER_MAC_VERSION_MAX_SIZE (20)

#define WLAN_CONNECT_FLAG_FAST_TRANSITION_EN   0x00000001
#define WLAN_CONNECT_FLAG_PROFILE_CONNECT 0x00000002
#define WLAN_CONNECT_FLAG_ENTERPRISE_CONNECT 0x00000004


#define WLAN_WPS_PIN_STR_LEN    9

/*****************************************************************************/
/* ENUM DEFINITIONS                                                     */
/*****************************************************************************/
//this is also roleTypes must to be same numbers as in :RoleType_e
typedef enum
{
    WLAN_ROLE_STA                    , /* 0  */
    WLAN_ROLE_IBSS                   , /* 1  */
    WLAN_ROLE_AP                     , /* 2  */
    WLAN_ROLE_DEVICE                 , /* 3  */
    WLAN_ROLE_P2P_CL                 , /* 4  */
    WLAN_ROLE_P2P_GO                 , /* 5  */
    WLAN_ROLE_RESERVED               , /* 6  */
    WLAN_ROLE_NONE                   = 0xff
}WlanRole_e;//this is also roleTypes must to be same numbers as in :RoleType_e

/* CSI defines */
#define WLAN_CSI_MSG_Q_BLOCKING                         1
#define WLAN_CSI_MSG_Q_NONBLOCKING                      0

/* Custom Regulatory domain enum */

//BAND
typedef enum
{
    BAND_24GHZ = 0,
    BAND_5GHZ,
    JAPAN_49GHZ
} WlanBand_e;

//CHANNELS  24GHZ_BAND
typedef enum
{
    CHANNEL_1  = 1,
    CHANNEL_2 ,
    CHANNEL_3 ,
    CHANNEL_4 ,
    CHANNEL_5 ,
    CHANNEL_6 ,
    CHANNEL_7 ,
    CHANNEL_8 ,
    CHANNEL_9 ,
    CHANNEL_10,
    CHANNEL_11,
    CHANNEL_12,
    CHANNEL_13,
    CHANNEL_14
} WlanChannel24G_e;

//CHANNELS  5GHZ_BAND
typedef enum
{
    CHANNEL_36 = 36,
    CHANNEL_40 = 40,
    CHANNEL_44 = 44,
    CHANNEL_48 = 48,
    CHANNEL_52 = 52,
    CHANNEL_56 = 56,
    CHANNEL_60 = 60,
    CHANNEL_64 = 64,
    CHANNEL_100 = 100,
    CHANNEL_104 = 104,
    CHANNEL_108 = 108,
    CHANNEL_112 = 112,
    CHANNEL_116 = 116,
    CHANNEL_120 = 120,
    CHANNEL_124 = 124,
    CHANNEL_128 = 128,
    CHANNEL_132 = 132,
    CHANNEL_136 = 136,
    CHANNEL_140 = 140,
    CHANNEL_144 = 144,
    CHANNEL_149 = 149,
    CHANNEL_153 = 153,
    CHANNEL_157 = 157,
    CHANNEL_161 = 161,
    CHANNEL_165 = 165,
    CHANNEL_169 = 169
} WlanChannel5G_e;

//CHANNELS  49GHZ_BAND
typedef enum
{
    CHANNEL_184 = 184,
    CHANNEL_188 = 188,
    CHANNEL_192 = 192,
    CHANNEL_196 = 196
} WlanChannel49G_e;

typedef enum
{
    WPS_METHOD_PBC,
    WPS_METHOD_PIN
} WlanWpsConfigMethod_e;

typedef enum
{
    SAE_ANTI_CLOGGING_DEFAULT = 5,
    SAE_ANTI_CLOGGING_ALWAYS = 0,
    SAE_ANTI_CLOGGING_LOW = 15
} WlanSaeAntiCloggingType_e;
/*****************************************************************************/
/* STRUCTURE DEFINITIONS                                                     */
/*****************************************************************************/
typedef struct
{
    uint8_t       Ssid[WLAN_SSID_MAX_LENGTH];
    uint8_t       Bssid[WLAN_BSSID_LENGTH];
    uint8_t       SsidLen;
    int8_t        Rssi;
    uint16_t      SecurityInfo;
    uint8_t       Channel;
}WlanNetworkEntry_t;

typedef struct
{
    uint8_t       Ssid[WLAN_SSID_MAX_LENGTH];
    uint8_t       Bssid[WLAN_BSSID_LENGTH];
    uint8_t       SsidLen;
    int8_t        Rssi;
    uint16_t      SecurityInfo;
    uint8_t       Channel;

    uint16_t      mRatesLen;
    uint8_t       mRates[WLAN_DOT11_MAX_SUPPORTED_RATES + WLAN_LEN_OF_IE_HEADER];

    uint16_t      HtCapabilitiesLen;     // HT capabilities IE were received
    uint8_t       HtCapabilitiesIe[WLAN_DOT11_HT_CAPABILITIES_ELE_LEN + WLAN_LEN_OF_IE_HEADER];   /* HT capabilities IE unparsed */

    uint16_t      erpIeLen;     // HT capabilities IE were received
    uint8_t       erpIe[WLAN_DOT11_ERP_ELE_LEN + WLAN_LEN_OF_IE_HEADER];
    Bool_e        mErpPresent;
    uint8_t       mUseProtection; //erp capabilities
    uint8_t/*EPreamble*/    mBarkerPreambleMode; //erp capabilities

    uint8_t       rsnIeLen;//the len includes the header
    uint8_t       rsnIeData[WLAN_DOT11_MAX_RSNE_SIZE + WLAN_LEN_OF_IE_HEADER];

    uint8_t       wpaIeLen;
    uint8_t       wpaIeData[WLAN_DOT11_MAX_RSNE_SIZE + WLAN_LEN_OF_IE_HEADER];

    uint16_t      wpsLen;
    uint8_t       wpsIeData[WLAN_DOT11_MAX_WPSIE_SIZE + WLAN_LEN_OF_IE_HEADER];
    Bool_e        wpsPresent;

    uint16_t      beaconInterval;
    uint16_t      capabilities;

}WlanNetworkEntryExtended_t;

typedef struct
{
    /* Sec Type */
    uint8_t   Type;
    /* Sec Key */
    int8_t*   Key;
    /* Key Length */
    uint8_t   KeyLen;
}WlanSecParams_t;

typedef struct
{
    int8_t*   User;
    uint8_t   UserLen;
    int8_t*   AnonUser;
    uint8_t   AnonUserLen;
    uint8_t   CertIndex;  /* not supported */
    uint32_t  EapMethod;
}WlanSecParamsExt_t;

typedef struct WpsParams
{
    /* WPS UUID */
    uint8_t *uuid;
    /* WPS device name */
    char *deviceName;
    /* WPS manufacturer */
    char *manufacturer;
    /* WPS model name */
    char *modelName;
    /* WPS model number */
    char *modelNumber;
    /* WPS serial number */
    char *serialNumber;
    /* WPS device type */
    uint8_t *deviceType;
    /* WPS os version */
    uint8_t *osVersion;
    /* WPS config methods */
    char *configMethods;
}WpsParams_t;

typedef struct P2pParams
{
    /* P2P Listen Channel */
    uint8_t listenChannel;
    /* P2P Listen Reg Class */
    uint8_t listenReg;
    /* P2P Operational Channel */
    uint8_t operChannel;
    /* P2P Operational Reg Class */
    uint8_t operReg;
    /* P2P GO Intent [0 - 15] */
    uint8_t goIntent;
    /* P2P SSID postfix */
    char *p2pSsidPostfix;

    uint32_t reserved;
}P2pParams_t;

typedef struct RoleUpApCmd
{   /* Ap's SSID */
    uint8_t *ssid;
    /* Determine if AP has hidden SSID */
    uint8_t hidden;
    /* 802.11 WLAN channel [1-12] */
    uint8_t channel;
    /* The AP's TX power */
    uint8_t tx_pow;
    /* Limits the number of stations that the AP's has */
    uint8_t sta_limit;
    /* Security parameters - Security Type and Password */
    WlanSecParams_t secParams;
    /* Set regulatory Domain */
    uint8_t countryDomain[3];
    /* SAE PWE Options */
    uint8_t sae_pwe;
    /* SAE anti-clogging threshold  - see WlanSaeAntiCloggingType_e */
    uint8_t sae_anticlogging_threshold;
    /* Set transition disable */
    uint8_t transitionDisable;
    /* Set AP to start as autonomous P2P Group Owner */
    uint8_t p2p_aGO; //TRUE/FALSE
    /* AP WPS enabled/disabled */
    Bool_e wpsDisabled; //TRUE/FALSE
    /* Set WPS parameters */
    WpsParams_t wpsParams;
    /* P2P enabled/disabled */
    Bool_e p2pDeviceEnabled; //TRUE/FALSE
    /* Set P2P Params */
    P2pParams_t P2pParams;
}RoleUpApCmd_t;

typedef struct RoleUpStaCmd
{
    /* Set regulatory Domain */
    /* The first two characters represent country code: ["00","EU","JP","CS"] */
    /* The third character represent indoor ["I"] */
    /* For example countryDomain = "EUI" (EUrope Indoor) */
    uint8_t countryDomain[3];
    /* STA WPS enabled/disabled */
    Bool_e wpsDisabled; //TRUE/FALSE
    /* Set WPS parameters */
    WpsParams_t wpsParams; //only relevant for CC35xx
    /* P2P enabled/disabled */
    Bool_e p2pDeviceEnabled; //TRUE/FALSE //only relevant for CC35xx
    /* Set P2P Params */
    P2pParams_t P2pParams; //only relevant for CC35xx
}RoleUpStaCmd_t;

typedef struct __PACKED__
{
    /* Event to wake up on */
    uint8_t WakeUpEvent;
    /* number of DTIMs */
    uint8_t ListenInterval;
    uint8_t padding[2];
} WlanLongSleepInterval;

typedef struct __PACKED__
{
    uint8_t enableVal;
    uint8_t padding[3];
}CalibratorSetTransmitOmiParams_t;


typedef struct __PACKED__
{
    uint8_t enableVal;
    uint8_t padding[3];
}CalibratorSetPowerHeaderParams_t;

typedef struct __PACKED__
{
    uint8_t index;
    uint32_t value;
    uint8_t padding[3];
}CalibratorSetPsmParams_t;

typedef struct __PACKED__
{
    uint8_t enableVal;
    uint8_t padding[3];
}CalibratorSetUplinkMuDataParams_t;

typedef struct __PACKED__
{
    uint8_t rateVal;
    uint8_t padding[3];
}CalibratorSetMcsRateParams_t;

typedef struct __PACKED__
{
    uint8_t enableVal;
    uint8_t padding[3];
}CalibratorOperationModeControlParams_t;

typedef struct __PACKED__
{
    uint8_t enableVal;
    uint8_t padding[3];
}CalibratorUplinkMuParams_t;

typedef struct __PACKED__
{
    uint8_t value;
    uint8_t padding[3];
}CalibratorGiLtfModeParams_t;

typedef struct __PACKED__
{
    uint8_t beaconRssi;
    uint8_t padding[3];
}CalibratorBeaconRssiParams_t;

typedef struct __PACKED__
{
    uint8_t overEnable;
    uint8_t bw;
    uint8_t preamble;
    uint8_t rate;
    uint8_t dcm;
    uint8_t txPower;
    uint8_t giLTF;
    uint8_t padding;
}CalibratorRateOverrideParams_t;

typedef struct __PACKED__
{
    uint32_t receivedTotalPacketsNumber;
    uint32_t receivedFcsErrorPacketsNumber;
    uint32_t receivedAddressMismatchPacketsNumber;
    uint32_t receivedGoodPackets;
    int16_t  averageDataCtrlRssi;
    int16_t  averageDataCtrlSNR;
}CalibratorGetRxParams_t;

typedef struct __PACKED__
{
    uint8_t channel;
    uint8_t band;
    uint8_t bandwidth;
}CalibratorChannelTuneParams_t;

typedef struct __attribute__((packed))
{
    uint8_t roleType;
    uint8_t                     roleId;
    uint16_t                    channelNum;
         
   //uint8_t                     padding[1];
} WlanRole_current_channel_number;

typedef struct __PACKED__
{
    uint64_t    bitmask;
    uint8_t     preambleType;
    uint8_t     phyRate;
    int8_t      txPower;
    uint8_t     GI_LTF_Type;
    uint8_t     DCM;
    uint16_t    startLength;
    uint16_t    endLength;
    uint32_t    delay;
    uint8_t     packetMode;
    uint16_t    numberOfPackets;
    uint8_t     dataMode;
    uint8_t     dataConstValue;
    uint8_t     enableCCA;
    uint8_t     bssColor;
    uint16_t    SU_ER_Bandwidth;
    uint8_t     partialAID;
    uint8_t     srcAddr[MAC_ADDRESS_LEN];
    uint8_t     dstAddr[MAC_ADDRESS_LEN];
    uint8_t     nominalPacketExtension;
    uint8_t     feedbackStatus;
    uint16_t    aid;
    uint8_t     groupId;
    uint8_t     ltfMode;
    uint8_t     heLtfNum;
    uint8_t     preFecPaddingFactor;
    uint16_t    commonInfoLen;
    uint16_t    ruAlloc;
    uint8_t     ulBw;
    uint8_t     startsStsNum;
    uint8_t     tbAutoMode;
    uint8_t     disamb;
    uint8_t     eht_sig_rate;
    uint8_t     ru_80_subblock;
    uint8_t     regulatory_TxPowerLimit;
    uint8_t	    padding[2];//padding
}CalibratorStartTxParams_t;


typedef struct __PACKED__
{
	uint8_t mode;
	int8_t offset;
	int8_t output_power;
}CalibratorStartTxToneParams_t;

typedef struct  __PACKED__
{
	uint16_t io_id;
	uint8_t  direction;
	uint8_t  state;
}CalibratorSetIoCfgParams_t;

typedef struct  __PACKED__
{
	uint16_t io_id;
	uint8_t  direction;
	uint8_t  value;
}CalibratorGetIoCfgParams_t;

typedef struct __PACKED__
{
    uint8_t     ackEnable;
    uint8_t     macAddress[MAC_ADDRESS_LEN];
    uint16_t    aid;
    uint8_t     preambleType;
    uint8_t     rate;
    uint8_t     padding;
}CalibratorStartRxParams_t;

typedef struct __PACKED__
{
    uint8_t block_ack_rx;
    uint8_t block_ack_tx;
    uint8_t padding[2];
}CalibratorBASessionparams_t;

typedef enum
{
    CAL_FORCE_DISABLE_DCM          = 0,
    CAL_FORCE_DISABLE_ER           = 1,
    CAL_FORCE_DISABLE_ER_UPPER     = 2,
    CAL_FORCE_LONG_TERM_POLICY     = 3,
    CAL_FORCE_NOMINAL_PADDING      = 4,
    CAL_ENABLE_DEBUG_TRACE         = 5
}LinkAdaptCalDebug_e ;

typedef struct __PACKED__
{
    uint32_t param1;
    uint32_t param2;
    uint8_t type;
    uint8_t padding[3];
}CalibratorLinkAdaptParams_t;

typedef struct __PACKED__
{
    int16_t  highThr_24G;
    int16_t  lowThr_24G;
    int16_t  highThr_5G;
    int16_t  lowThr_5G;
    int16_t  HighSampleThr;
    uint8_t  enable_value;
    uint8_t  padding[1];
}CalibratorThermalParams_t;


typedef struct
{
    uint8_t calibratorAction;
    void    *calibratorCommandParams;
}CalibratorCommandWrapper_t;

/*****************************************************************************/
/* EVENTS DEFINITIONS                                                        */
/*****************************************************************************/
typedef enum
{
    WLAN_EVENT_CONNECT = 1,
    WLAN_EVENT_DISCONNECT,
    WLAN_EVENT_SCAN_RESULT,
    WLAN_EVENT_ADD_PEER,
    WLAN_EVENT_REMOVE_PEER,
    WLAN_EVENT_AUTHENTICATION_REJECTED,
    WLAN_EVENT_CONNECTING,
    WLAN_EVENT_ASSOCIATION_REJECTED,
    WLAN_EVENT_ACTION_FRAME_RX,
    WLAN_EVENT_ASSOCIATED,
    WLAN_EVENT_WPS_INVALID_PIN,
    WLAN_EVENT_AP_WPS_START_FAILED,
    WLAN_EVENT_AP_EXT_WPS_SETTING_FAILED,
    WLAN_EVENT_BLE_ENABLED,
    WLAN_EVENT_CS_FINISH,
    WLAN_EVENT_ROC_DONE,
    WLAN_EVENT_CROC_DONE,
    WLAN_EVENT_SEND_ACTION_DONE,
    WLAN_EVENT_EXTENDED_SCAN_RESULT,
	WLAN_EVENT_P2P_GROUP_STARTED,
    WLAN_EVENT_P2P_GROUP_REMOVED,   
    WLAN_EVENT_P2P_SCAN_COMPLETED,
    WLAN_EVENT_P2P_GROUP_FORMATION_FAILED,
    WLAN_EVENT_P2P_PEER_NOT_FOUND,
    WLAN_EVENT_CONNECT_PERIODIC_SCAN_COMPLETE,
    WLAN_EVENT_FW_CRASH,
    WLAN_EVENT_COMMAND_TIMEOUT,
    WLAN_EVENT_GENERAL_ERROR,
    WLAN_EVENT_BSS_TRANSITION_INITIATED,
    WLAN_EVENT_PEER_AGING,
    WLAN_EVENT_ERROR,	
    WLAN_EVENT_MAX = 0xFFFF
} WlanEventId_e;

/*****************************************************************************/
/* MODULES DEFINITIONS                                                        */
/*****************************************************************************/
typedef enum
{
    WLAN_MODULE_TX_THREAD = 1,
    WLAN_MODULE_FWEVENT,
    WLAN_MODULE_CMDSM,
    WLAN_MODULE_TMR,
    WLAN_MODULE_RX,
    WLAN_MODULE_BLE,
    WLAN_MODULE_DEINIT,
    WLAN_MODULE_MAX = 0xFFFFFFFF
}WlanModule_e;

/*****************************************************************************/
/* MODULES DEFINITIONS                                                        */
/*****************************************************************************/
typedef enum
{
    WLAN_SEVERITY_HIGH = 1,
    WLAN_SEVERITY_MID,
    WLAN_SEVERITY_LOW,
    WLAN_SEVERITY_MAX = 0xFFFFFFFF
}WlanSeverity_e;

typedef struct
{
    uint8_t     SsidLen;
    uint8_t     SsidName[WLAN_SSID_MAX_LENGTH];
    uint8_t     Bssid[WLAN_BSSID_LENGTH];
    uint8_t     Channel;
    int32_t     Status;
} WlanEventConnect_t;

typedef struct
{
    uint8_t     SsidLen;
    uint8_t     SsidName[WLAN_SSID_MAX_LENGTH];
    uint8_t     Bssid[WLAN_BSSID_LENGTH];
} WlanEventConnecting_t;


typedef struct
{
    uint8_t                 SsidLen;
    uint8_t                 SsidName[WLAN_SSID_MAX_LENGTH];
    uint8_t                 Bssid[WLAN_BSSID_LENGTH];
    int16_t                 ReasonCode;
    uint8_t                 IsStaIsDiscnctInitiator;
} WlanEventDisconnect_t;

typedef struct
{
    uint32_t                 inactiveTime;//for how long the sTA was inactive in ms
    uint8_t                  peer_mac[6];
} WlanEventPeerAging_t;

typedef struct
{
    uint32_t           NetworkListResultLen;
    WlanNetworkEntry_t NetworkListResult[WLAN_MAX_SCAN_COUNT];

} WlanEventScanResult_t;

typedef struct
{
    uint32_t           NetworkListResultLen;
    WlanNetworkEntryExtended_t* NetworkListResult;//[WLAN_EXTENDED_MAX_SCAN_COUNT];

}WlanEventExtendedScanResult_t;

typedef struct
{
    uint8_t   Mac[6];
    uint8_t   isExtWPSInProgress;    // IsWPS is only meaningful when external WPS is enabld
    uint8_t   Padding[1];
}WlanEventSTAAdded_t;

typedef struct
{
    uint8_t   Mac[6];
    uint8_t   Padding[2];
}WlanEventSTARemoved_t;

typedef struct
{
    uint8* frame;//(struct ieee80211_mgmt *)
    uint32 frame_len;
    uint32 dataRate;
    int32  rssi_signal;
    int32  rx_freq;
}WlanEvenWfdMgmtFrame_t;

typedef struct
{
    WlanModule_e module;
    int error_num;
    WlanSeverity_e severity;
} WlanEventError_t;

typedef struct
{
    uint8_t     SsidLen;
    uint8_t     SsidName[WLAN_SSID_MAX_LENGTH];
    uint8_t     Bssid[WLAN_BSSID_LENGTH];
    uint8_t     Channel;
    uint8_t     P2pRoleGo;
} WlanEventFormationComplete_t;

typedef struct
{
    WlanRole_e   RoleType;
} WlanEventGroupRemoved_t;

typedef struct
{
    uint8_t *log_buffer;
    uint32_t log_buffer_len;
} WlanEventFwCrash_t;

typedef struct
{
    uint8_t     SsidLen;
    uint8_t     SsidName[WLAN_SSID_MAX_LENGTH];
    uint8_t     Bssid[WLAN_BSSID_LENGTH];
    uint8_t     Channel;
    uint8_t     isExtWPSInProgress;  // IsWPS is only meaningful when external WPS is enabld
} WlanEventAssociated_t;

typedef union
{
    /* STA Role */
    WlanEventConnect_t                    Connect; /* WLAN_EVENT_CONNECT */
    uint16_t                              AuthStatusCode; /* WLAN_EVENT_AUTHENTICATION_REJECTED */  
    WlanEventConnecting_t                 Connecting; /* WLAN_EVENT_CONNECTING */
    uint16_t                              AssocStatusCode; /* WLAN_EVENT_ASSOCIATION_REJECTED */
    WlanEventDisconnect_t                 Disconnect; /* WLAN_EVENT_DISCONNECT */
    WlanEventScanResult_t                 ScanResult; /* WLAN_EVENT_SCAN_RESULT */
    WlanEventExtendedScanResult_t*        pExtendedScanResult; /* WLAN_EVENT_EXTENDED_SCAN_RESULT */
    uint8                                 newChannel; /* WLAN_EVENT_CS_FINISH */
    WlanEventAssociated_t                 Associated;

    /* AP Role */
    WlanEventSTAAdded_t          AddPeer; /* WLAN_EVENT_ADD_PEER */
    WlanEventSTARemoved_t        RemovePeer; /* WLAN_EVENT_REMOVE_PEER */
    WlanEvenWfdMgmtFrame_t       WfdActionFrame;/* WLAN_EVENT_ACTION_FRAME_RX - this event will be triggered only if defined WlanExternalApp_t*/
    int8_t                       wpsFailCode; /* WLAN_EVENT_AP_WPS_START_FAILED/WLAN_EVENT_AP_EXT_WPS_SETTING_FAILED - reason code arrives from supplicant */
    int32_t                      bleEnableStatus;
    int32_t                      dppCROCevenReasonTimeout;//dppCROCevenReasonTimeout=1 if triggered due to timeout
    /* P2P */
    WlanEventFormationComplete_t FormationComplete; /* WLAN_EVENT_P2P_GROUP_STARTED */
    WlanEventGroupRemoved_t      GroupRemoved;      /* WLAN_EVENT_P2P_GROUP_REMOVED */
    WlanEventPeerAging_t         peerAging;
    /* General*/
    WlanEventError_t       error;  /* WLAN_EVENT_ERROR */
    /* General */
    WlanEventFwCrash_t           FwCrashLog; /* WLAN_EVENT_FW_CRASH */  
} WlanEventData_u;

typedef struct
{
    WlanEventId_e        Id;
    WlanEventData_u    Data;

} WlanEvent_t;

/* Host Event handler Callback*/
typedef void (* WlanEventHandlerCB_t)(WlanEvent_t * pWlanEvent);

/*****************************************************************************/
/* OTHER DEFINITIONS                                                         */
/*****************************************************************************/
typedef enum
{
    WLAN_MAC_OFFSET_ROLE_STA,
    WLAN_MAC_OFFSET_ROLE_BLE,
    WLAN_MAC_OFFSET_ROLE_DEVICE,
    WLAN_MAC_OFFSET_ROLE_AP,
    WLAN_MAC_OFFSET_ROLE_P2P_GO = WLAN_MAC_OFFSET_ROLE_AP,
}WlanMacAddOffset_e;

typedef enum
{
    WLAN_STATION_AUTO_PS_MODE,
    WLAN_STATION_ACTIVE_MODE,
    WLAN_STATION_POWER_SAVE_MODE,
    WLAN_STATION_NUM_OF_MODES
}WlanPowerSave_e;

typedef enum
{
    POWER_MANAGEMENT_ALWAYS_ACTIVE_MODE,
    POWER_MANAGEMENT_POWER_DOWN_MODE,
    POWER_MANAGEMENT_ELP_MODE,
    POWER_MANAGEMENT_NUM_OF_MODES
} WlanPowerManagement_e;

typedef struct
{
    uint32_t* channelArray;
    uint32_t  length;
} WlanSelectedChannelsArray_t;

typedef enum
{
    WAKE_UP_EVENT_BEACON    = 0, /* Wake on every Beacon */
    WAKE_UP_EVENT_DTIM      = 1, /* Wake on every DTIM */
    WAKE_UP_EVENT_N_DTIM    = 2, /* Wake every Nth DTIM */
    WAKE_UP_EVENT_LIMIT     = WAKE_UP_EVENT_N_DTIM,
}WlanWakeUpEvent_e;

typedef struct
{
    /* Saved the configured role bitmap - each bit indicates specific role */
    uint32_t             hostConfiguredRolesBitmap;
    /* Saved the event handler Callback function */
    WlanEventHandlerCB_t eventHandlerCB;
    /* Saved the device current power save mode */
    WlanPowerSave_e      devicePowerSaveMode;
} StartRoleApps_t;

#define WLAN_MAX_SCAN_SSID 5
typedef struct
{
    uint8_t ssidLen;
    uint8_t pSsid[WLAN_SSID_MAX_LENGTH];
    uint8_t mHidden;// 1 = SSID_TYPE_HIDDEN, 0 = SSID_TYPE_PUBLIC
    uint8_t padding[2];
}scan_ssid_t;

//Taken from dot11CommonRadioApi.h
typedef enum
{
  BAND_SEL_ONLY_2_4GHZ,
  BAND_SEL_ONLY_5GHZ,
  BAND_SEL_BOTH
} WlanBandCfg_e;

typedef struct
{
    scan_ssid_t ssidLst[WLAN_MAX_SCAN_SSID];
    WlanBandCfg_e Band; //added 0  - 2.4, 1 = 5G , 2 = Both
    uint8_t extWpsMode;
}scanCommon_t;

//--------------------------------------------------------------
// p2p related command
//used by WLAN_SET_P2P_CMD
//--------------------------------------------------------------
typedef struct
{
    uint32_t    operChannel;
    uint32_t    operClass;
    uint32_t    listenChannel;
    uint32_t    listenClass;
    uint8_t     goIntent; 
} WlanP2pConfigChannelParam_t; // P2P_CMD_ID_CONFIG_CHANNELS

typedef enum
{
    P2P_CMD_ID_SCAN_STOP,
    P2P_CMD_ID_GROUP_REMOVE,
    P2P_CMD_ID_CONFIG_CHANNELS, //channels
    P2P_CMD_ID_LISTEN,
    P2P_CMD_ID_CANCEL
} WlanP2pCmdId_e;

typedef union
{
    WlanP2pConfigChannelParam_t cfgParams; //P2P_CMD_ID_CONFIG_CHANNELS
} WlanP2PCmdData_u;
typedef struct
{
    WlanP2pCmdId_e      Id;
    WlanP2PCmdData_u    Data;
} WlanP2pCmd_t;

// end p2p related commands
//--------------------------------------------------------------

typedef struct
{
    char    *channel_list;
} WlanNonPrefChannels_t;


typedef struct {
    uint8_t    max_dwell_time_passive_msec;
    uint8_t    min_dwell_time_passive_msec;

    uint8_t    max_dwell_time_active_msec ;
    uint8_t    min_dwell_time_active_msec ;

    uint8_t    dwell_time_dfs_passive_msec;
}WlanScanDwellTime_t;

typedef struct
{
    WlanRole_e roleType;
    uint16_t vendorElements_length;
    uint8_t* pVendorElements;
} WlanSetVendorElements_t;

typedef struct
{
    uint8_t fastPolicy;
    uint8_t autoPolicy;
    uint8_t fastPersistant;
    uint8_t reserved;
} WlanPolicySetGet_t;

typedef struct
{
    char pin[WLAN_WPS_PIN_STR_LEN]; 
    uint32_t timeout; 
} WlanSetWpsApPinParam_t;

typedef enum
{
    WLAN_SET_MACADDRESS,
    WLAN_SET_POWER_SAVE,
    WLAN_SET_CUSTOM_DOMAIN_ENTRY,
    WLAN_SET_POWER_MANAGEMENT,
    WLAN_SET_LSI,
    WLAN_SET_CALIBRATOR_POWER_MODE,
    WLAN_SET_CALIBRATOR_CHANNEL_TUNE,
    WLAN_SET_CALIBRATOR_TX_START,
    WLAN_SET_CALIBRATOR_TX_TONE_START,
    WLAN_SET_CALIBRATOR_TX_STOP,
    WLAN_SET_CALIBRATOR_START_RX,
    WLAN_SET_CALIBRATOR_STOP_RX,
    WLAN_SET_CALIBRATOR_TX_PARAMS,
    WLAN_SET_CALIBRATOR_TB_TX_PARAMS,
    WLAN_SET_CALIBRATOR_RATE_OVERRIDE,
    WLAN_SET_CALIBRATOR_GI_LTF,
    WLAN_SET_CALIBRATOR_UPLINK_MU,
    WLAN_SET_CALIBRATOR_OPERATION_MODE_CONTROL,
    WLAN_SET_CALIBRATOR_SWITCH_ANT,
    WLAN_SET_CALIBRATOR_MCS_RATE,
    WLAN_SET_CALIBRATOR_UPLINK_MU_DATA,
    WLAN_SET_CALIBRATOR_PSM,
    WLAN_SET_CALIBRATOR_UPLINK_POWER_HEADER,
    WLAN_SET_CALIBRATOR_TRANSMIT_OSI,
    WLAN_SET_CALIBRATOR_MANUAL_CALIBRATION,
    WLAN_SET_CALIBRATOR_SET_IO_CFG,
    WLAN_SET_CALIBRATOR_BLE_ENABLE,
    WLAN_SET_RESET_COUNTERS,
    WLAN_SET_VENDOR_IE,
    WLAN_SET_TX_CTRL,
    WLAN_SET_EXTERNAL,
    WLAN_SET_EXT_P2P_BUILD_ACTION_FRAME,
    WLAN_SET_EXT_OFFCHANNEL_ROC,
    WLAN_SET_EXT_OFFCHANNEL_SEND_RAW_ACTION,
    WLAN_SET_WPS_SESSION,
    WLAN_SET_EXT_WPS_SESSION,
    WLAN_SET_EXTENDED_SCAN_RESULTS,
    WLAN_SET_BLE_ENABLE,
    WLAN_SET_RESET_DECRYPT_PACKETS_COUNT,
    WLAN_SET_ENABLE_CHANNEL_UTILIZATION_NEXT_SCAN,
    WLAN_SET_P2P_CMD,
    WLAN_SET_SCAN_RESULTS_SIZE,
    WLAN_SET_NON_PREFFERED_CHANNELS,
    WLAN_SET_ENABLE_ANT_DIV,
    WLAN_SET_ANT_DIV_RSSI_THRESHOLD,
    WLAN_SET_SELECT_DEFAULT_ANTENNA,
    WLAN_SET_CALIBRATOR_BA_SESSION,
    WLAN_SET_CALIBRATOR_LINK_ADAPT,
    WLAN_SET_STA_WIFI_BAND,
    WLAN_SET_TRIGGER_FW_ASSERT,
    WLAN_SET_PSD_METHOD,
    WLAN_SET_CSI,
    WLAN_SET_WIRELESS_PROTOCOL,
    WLAN_SET_PEER_AGING_TIMEOUT,
    WLAN_SET_CALIBRATOR_THERMAL_CFG,
    WLAN_SET_PRIMARY_CONNECTIVITY_FW,
    WLAN_SET_CONNECTION_SCAN_EARLY_TERMINATION,
    WLAN_SET_CONNECTION_POLICY,
    WLAN_SET_SCAN_DWELL_TIME,
    WLAN_SET_LISTED_CHANNELS_FOR_SCAN,
    WLAN_SET_WPS_AP_PIN,
    WLAN_SET_SCHED_SCAN_PLANS,
    WLAN_SET_CSI_SOLICITATION,
    WLAN_SET_CSI_SOLICITATION_MAC
} WlanSet_e;


typedef enum
{
    WLAN_GET_MACADDRESS,
    WLAN_GET_POWER_SAVE,
    WLAN_GET_CUSTOM_DOMAIN_ENTRY,
    WLAN_GET_CALIBRATOR_TX_PARAMS,
    WLAN_GET_CALIBRATOR_RX_STATS,
    WLAN_GET_CALIBRATOR_GET_IO_CFG,
    WLAN_GET_RSSI,
#ifdef NOISE_FLOOR //not defined on CC3xxx
    WLAN_GET_NOISE_FLOOR,
#endif
    WLAN_GET_FWVERSION,
    WLAN_GET_SPVERSION,
    WLAN_GET_DEVICE_INFO,
    WLAN_GET_CALIBRATOR_BEACON_RSSI,
    WLAN_GET_COUNTERS,
    WLAN_GET_CONNECTION_KEY_MANAGEMENT,
    WLAN_GET_EXTERNAL,
    WLAN_GET_SCAN_CHANNEL_UTILIZATION_RESULTS,
    WLAN_GET_DECRYPTED_PACKETS_COUNT,
    WLAN_GET_ROLE_CHANNEL_NUMBER,
    WLAN_GET_P2P_CONFIG_CHANNEL_PARAMS,
    WLAN_GET_ANT_DIV_STATUS,
    WLAN_GET_ANT_DIV_RSSI_THRESHOLD,
    WLAN_GET_DEFAULT_ANTENNA,
    WLAN_GET_HOST_VERSION,
    WLAN_GET_CSI,
    WLAN_GET_PRIMARY_CONNECTIVITY_FW,
    WLAN_GET_CONNECTION_SCAN_EARLY_TERMINATION,
    WLAN_GET_CONNECTION_POLICY,
    WLAN_GET_SYSINFO
} WlanGet_e;

typedef struct __attribute__((packed))
{
    uint8_t                           channelNumber;     /* number of the channel*/
    uint8_t                           channelLoad;       /* channel load, percentage of cca busy from total cca cycles  */
    int8_t                            noiseFloorAvg;     /* noise floor indication measurement: average noise power */
    uint8_t                           BssCount;          /* BSS count - diffrent BSS's counted during Servery */
}WlanChannel_utilize_res_t;

typedef struct __attribute__((packed))
{
    uint32_t roleid;
    uint32_t decrypted_ok_unicast_count;
    uint32_t decrypted_error_unicast_count;
    uint32_t decrypted_ok_multicast_count;
    uint32_t decrypted_error_multicast_count;
    uint32_t decrypted_ok_broadcast_count;
    uint32_t decrypted_error_broadcast_count ;
	//uint8 padding[3];
} WlanDecrypted_packetsCounters_t ;

typedef struct
{
    WlanRole_e roleType;
    unsigned char pMacAddress[6];
}WlanMacAddress_t;


typedef struct __PACKED__
{
    uint16 major_version;
    uint16 minor_version;
    uint16 api_version;
    uint16 build_version;

    uint8 phy_version[8];
    uint8_t container_type;
    uint8_t padding[3];
} WlanFWVersions_t;

typedef struct __PACKED__
{
    uint8  major_version;
    uint8  minor_version;
    uint16 revision_version;
    uint32 build_version;

    uint8 phy_version[8];

} WlanSPVersions_t;

typedef struct __PACKED__
{
    uint8      role_id;
    uint8      pad[3];
    uint32     missed_beacons;
    uint8      snr_data;
    uint8      snr_beacon;
    int8       rssi_data;
    int8       rssi_beacon;
} WlanBeaconRssi_t;

typedef struct __PACKED__
{
    uint8      role_id;
    uint8      pad[2];
    int8_t     noiseFloorAvg;     /* noise floor indication measurement: average noise power */
} WlanNoiseFloor_t;

typedef enum
{
    CALIBRATOR_ACTION_SET_POWER_MODE,
    CALIBRATOR_ACTION_CHANNEL_TUNE,
    CALIBRATOR_ACTION_TX_START,
    CALIBRATOR_ACTION_TX_STOP,
    CALIBRATOR_ACTION_TX_TONE_START,
    CALIBRATOR_ACTION_START_RX,
    CALIBRATOR_ACTION_STOP_RX,
    CALIBRATOR_ACTION_GET_RX_STATS,
    CALIBRATOR_ACTION_SET_TX_PARAMS,
    CALIBRATOR_ACTION_GET_TX_PARAMS,
    CALIBRATOR_ACTION_SET_TB_TX_PARAMS,
    CALIBRATOR_ACTION_RATE_OVERRIDE,
    CALIBRATOR_ACTION_GET_BEACON_RSSI,
    CALIBRATOR_ACTION_SET_GI_LTF,
    CALIBRATOR_ACTION_SET_UPLINK_MU,
    CALIBRATOR_ACTION_SET_OPERATION_MODE_CONTROL,
    CALIBRATOR_ACTION_SET_MCS_RATE,
    CALIBRATOR_ACTION_SET_UPLINK_MU_DATA,
    CALIBRATOR_ACTION_SET_PSM,
    CALIBRATOR_ACTION_SET_UPLINK_POWER_HEADER,
    CALIBRATOR_ACTION_SET_TRANSMIT_OMI,
    CALIBRATOR_ACTION_MANUAL_CALIBRATION,
    CALIBRATOR_ACTION_SWITCH_ANT,
    CALIBRATOR_ACTION_SET_IO_CFG,
    CALIBRATOR_ACTION_GET_IO_CFG,
    CALIBRATOR_ACTION__BLE_ENABLE,
    CALIBRATOR_ACTION_BA_SESSION,
    CALIBRATOR_ACTION_LINK_ADAPT,
    CALIBRATOR_ACTION_THERMAL_PROTECTION,
} CalibratorAction_e;

typedef enum
{
    PLT_OFF,
    PLT_ON
} CalibratorPowerModes_e;

typedef enum {
    CALIBRATOR_SET_TX_PREAMBLE_TYPE_BIT,
    CALIBRATOR_SET_TX_PHY_RATE_BIT,
    CALIBRATOR_SET_TX_TX_POWER_BIT,
    CALIBRATOR_SET_TX_GI_LTF_TYPE_BIT,
    CALIBRATOR_SET_TX_DCM_BIT,
    CALIBRATOR_SET_TX_LENGTH_BIT,
    CALIBRATOR_SET_TX_DELAY_BIT,
    CALIBRATOR_SET_TX_PACKET_MODE_BIT,
    CALIBRATOR_SET_TX_NUMBER_OF_PACKETS_BIT,
    CALIBRATOR_SET_TX_DATA_MODE_BIT,
    CALIBRATOR_SET_TX_DATA_CONST_VALUE_BIT,
    CALIBRATOR_SET_TX_ENABLE_CCA_BIT,
    CALIBRATOR_SET_TX_BSS_COLOR_BIT,
    CALIBRATOR_SET_TX_SU_ER_BANDWIDTH_BIT,
    CALIBRATOR_SET_TX_PARTIAL_AID_BIT,
    CALIBRATOR_SET_TX_SRC_ADDR_BIT,
    CALIBRATOR_SET_TX_DST_ADDR_BIT,
    CALIBRATOR_SET_TX_NOMINAL_PACKET_EXTENSION_BIT,
    CALIBRATOR_SET_TX_FEEDBACK_STATUS_BIT,
    CALIBRATOR_SET_TX_AID_BIT,
    CALIBRATOR_SET_TX_GROUP_ID_BIT,
    CALIBRATOR_SET_TX_LTF_MODE_BIT,
    CALIBRATOR_SET_TX_HE_LTF_NUM_BIT,
    CALIBRATOR_SET_TX_PRE_FEC_PADDING_FACTOR_BIT,
    CALIBRATOR_SET_TX_COMMON_INFO_LEN_BIT,
    CALIBRATOR_SET_TX_RU_ALLOC_BIT,
    CALIBRATOR_SET_TX_UL_BW_BIT,
    CALIBRATOR_SET_TX_STARTS_STS_NUM_BIT,
    CALIBRATOR_SET_TX_TB_AUTO_MODE_BIT,
    CALIBRATOR_SET_TX_TB_DISAMB_MODE_BIT
}CalibratorSetTxBits_e;

typedef struct
{
    /* Threshold of transition between TCP thread and TRANSPORT thread */
    uint32_t TxSendPaceThresh;
    /* Tx-Send pacing threshold */
    uint32_t TxSendPaceTimeoutMsec;
    /* Indicate if run schedualer after TX Complete */
    uint32_t TransmitQOnTxComplete ;
} WlanCtrlBlk_t;

typedef struct
{

    uint32_t    counters_timeLastReset_Ms;
    uint32_t    counters_timeLastUpdate_Ms;

    ////////////////////  TX TX //////////////////
    ////////////////////  TX TX //////////////////
    ////////////////////  TX TX //////////////////

    //configured tx rate, not actual !!!
    uint32_t    txRate;

    //entrance to the stack after LWIP
    uint32_t    link_tx_dbgStackTimeFirstPacket_Ms;
    uint32_t    link_tx_dbgStackTimeLastPacket_Ms;
    uint32_t    link_tx_dbgStackEntrNumByteSent ;
    uint32_t    link_tx_dbgStackEntNumPktsXfered ;

    //transport - before sending to the FW
    uint32_t    link_tx_dbgTimeFirstPacket_Ms;
    uint32_t    link_tx_dbgTimeLastPacket_Ms ;
    uint32_t    link_tx_dbgNumPktsSent       ;
    uint32_t    link_tx_dbgNumBytesSent      ;
    uint32_t    link_tx_dbgNumPktsBusy       ;
    uint32_t    link_tx_dbgNumPktsAcBusy     ;
    uint32_t    link_tx_dbgNumPktsXfered     ;
    uint32_t    link_tx_dbgNumPktsSuccess    ;
    uint32_t    link_tx_dbgNumPktsError      ;


   //transport -after Tx complete
    uint32_t    link_tx_dbgNumTxCmplt        ;
    uint32_t    link_tx_dbgNumTxCmpltOk      ;
    uint32_t    link_tx_dbgNumTxCmpltError   ;
    uint32_t    link_tx_dbgNumTxCmpltOkBytes ;
    uint32_t    link_tx_dbgNumTxCmpltFailBytes;

    ////////////////////  RX RX //////////////////
    ////////////////////  RX RX //////////////////
    ////////////////////  RX RX //////////////////

    uint32_t      rxRate;
    /* the number of frames that the NIC receives without errors */
    uint32_t      rxstat_RecvOk;
    /* the number of bytes in directed packets that are received without errors */
    uint32_t      rxstat_DirectedBytesRecv;
    /* the number of directed packets that are received without errors */
    uint32_t     rxstat_DirectedFramesRecv;
    /* the number of bytes in multicast/functional packets that are received without errors */
    uint32_t      rxstat_MulticastBytesRecv;
    /* the number of multicast/functional packets that are received without errors */
    uint32_t      rxstat_MulticastFramesRecv;
    /* the number of bytes in broadcast packets that are received without errors. */
    uint32_t      rxstat_BroadcastBytesRecv;
    /* the number of broadcast packets that are received without errors. */
    uint32_t      rxstat_BroadcastFramesRecv;

    ///////////////  link Rx ////////////////////////
    uint32_t      link_rx_recvFromWlan;
    uint32_t      link_rx_recvBytesFromWlan;
    uint32_t      link_rx_sendToUwd;
    uint32_t      link_rx_sendToDataDispatcher;
    uint32_t      link_rx_discardWrongHlid;
    uint32_t      link_rx_discardHlidClose;
    uint32_t      link_rx_discardBaEvent;
    uint32_t      link_rx_discardUnknownClass;
}dbg_cntr_trnspt_t;


typedef struct
{
    WlanRole_e          inp_role;
    dbg_cntr_trnspt_t   out_Param;
}dbg_counters_get_t;

typedef struct
{
    //mac_address[0] --- for STA role
    //mac_address[1] --- for BLE Role
    //mac_address[2] ---- for P2P
    //mac_address[3] ----- for AP or STA-2
    uint8_t   mac_address[WLAN_MAX_MAC_ADDRESSES][MAC_ADDRESS_LEN];//efuse mac address
    uint8_t   struct_is_updateed; //means that this global was update form BTL
    uint8_t   fuse_rom_structure_version;
    uint8_t   device_part_number;
    uint8_t   boot_rom_version;
    uint8_t   pg_version;
    uint8_t   metal_version;
    uint8_t   disable_5g;
    uint8_t   disable_6g;
    uint8_t   disable_ble;
    uint64_t  udi;
}device_info_t;

typedef struct
{
    char pVersion[UPPER_MAC_VERSION_MAX_SIZE + 1];
}wlanHostVersion_t;

typedef struct
{
    WlanRole_e  role_type;
    uint16_t    length;
    uint8_t     data[WLAN_BEACON_MAX_SIZE];
}wlanSetVendorInfo_t;

typedef struct
{
    uint32_t    peerAgingTimeoutMs;
}wlanSetStaAgingTimout_t;

typedef struct
{
    WlanWpsConfigMethod_e   wpsMethod;
    char                    pin[WLAN_WPS_PIN_STR_LEN];
}wlanWpsSession_t;

typedef struct
{
    uint8_t enable;
    uint8_t peerMac[MAC_ADDRESS_LEN];
    uint8_t wpsMethod; // 0 - PBC, 1 - PIN
    unsigned char *pin;
}wlanSetExtWpsSession_t;


typedef enum wlan_p2p_action_frame_type {
    WLAN_P2P_GO_NEG_REQ = 0,
    WLAN_P2P_GO_NEG_RESP = 1,
    WLAN_P2P_GO_NEG_CONF = 2,
    WLAN_P2P_INVITATION_REQ = 3,
    WLAN_P2P_INVITATION_RESP = 4,
    WLAN_P2P_DEV_DISC_REQ = 5,
    WLAN_P2P_DEV_DISC_RESP = 6,
    WLAN_P2P_PROV_DISC_REQ = 7,
    WLAN_P2P_PROV_DISC_RESP = 8
}WlanP2pActionFrameType_t;


typedef enum wlan_p2p_status_code {// this enum need to be the same as this one :p2p_status_code
    WLAN_P2P_SC_SUCCESS = 0,
    WLAN_P2P_SC_FAIL_INFO_CURRENTLY_UNAVAILABLE = 1,
    WLAN_P2P_SC_FAIL_INCOMPATIBLE_PARAMS = 2,
    WLAN_P2P_SC_FAIL_LIMIT_REACHED = 3,
    WLAN_P2P_SC_FAIL_INVALID_PARAMS = 4,
    WLAN_P2P_SC_FAIL_UNABLE_TO_ACCOMMODATE = 5,
    WLAN_P2P_SC_FAIL_PREV_PROTOCOL_ERROR = 6,
    WLAN_P2P_SC_FAIL_NO_COMMON_CHANNELS = 7,
    WLAN_P2P_SC_FAIL_UNKNOWN_GROUP = 8,
    WLAN_P2P_SC_FAIL_BOTH_GO_INTENT_15 = 9,
    WLAN_P2P_SC_FAIL_INCOMPATIBLE_PROV_METHOD = 10,
    WLAN_P2P_SC_FAIL_REJECTED_BY_USER = 11,
    WLAN_P2P_SC_SUCCESS_DEFERRED = 12,
}WlanP2pStatusCode_t;


typedef struct {
    uint8_t*  device_address;
    uint8_t   connection_capability;
    uint32_t  session_id;
    uint32_t  advertisement_id;
    uint8_t*  session_mac;
    uint8_t*  adv_mac;
    uint8_t*  persist_ssid;
    uint32_t  persist_ssid_len;
    uint8_t   dialog_token;
} WlanPeer_t;

typedef struct
{
    uint8_t      dialog_token;
    uint8_t*     p2p_device_addr;
    uint8_t*     oper_ssid; // 32 char+ null
    uint32_t     oper_ssid_len;
}WlanGOPeer_t;


typedef struct
 {
     WlanRole_e                 roleId;
     uint8_t                    msgType; /*see : wlan_p2p_action_frame_type*/
     uint8_t                    status; /*see WlanP2pStatusCode_t*/
     uint8_t                    provDiscReqSa[6];
     uint8_t*                   provDiscReqData;
     size_t                     provDiscReqLen;
     int32_t                    provDiscReqRxFreq;
 }WlanProvDiscResp_t;

 typedef struct
 {
     WlanRole_e   roleId;
     uint8_t      msgType; /*see : wlan_p2p_action_frame_type*/
 }WlanProvDiscReq_t;

typedef union
{

    WlanProvDiscReq_t  ProvDiscReq;
    WlanProvDiscResp_t ProvDiscResp;

}WlanActionParam_t;

typedef struct
{
    WlanP2pActionFrameType_t      msgType;
    WlanActionParam_t             actionParam;
}WlanBuildActionBuffer_t;

typedef struct
{
    uint32_t         enableROC;
    uint32_t         channel;
    uint8_t*         countryDomain;
}WlanROCExt_t;

typedef struct
{
    uint8_t* pBuff; // buf is allocate by the stack, expected to be freed by the app
    uint32_t bufLen;
} WlanActionBuff_t;

typedef struct
{
    uint8_t            apDst[MAC_ADDRESS_LEN];              // Destination address
    WlanActionBuff_t   actionBuff;          // the action data
}WlanActionTxExt_t;

typedef struct
{
    uint8_t extWps;
    uint8_t extP2P;
    uint8_t extWpsDone;
    uint8_t extWpsInProgress;
}WlanExternalApp_t;


typedef struct
{
    uint32_t  timestamp;
    uint16_t  packetLength;    // length of packet including Frame Check Sequence(FCS)
    uint16_t  csiInfoLength;
    uint16_t  seqNum;
    uint8_t   tMacAddr[MAC_ADDRESS_LEN]; // transmitter mac address
    uint8_t   rMacAddr[MAC_ADDRESS_LEN]; // receiver mac address
    int8_t    rssi;
    uint8_t   rate;       // Non HT, 11.b,g
    uint8_t   sig_mode;   // preamble
    uint8_t   mcs;
    uint8_t   sgi;
    uint8_t   channel;
    uint8_t   ant;
    uint8_t   cw;
    uint32_t  csiDataBuf[CSI_MAX_DATA_LENGTH];

}WlanGetCSIData_t;

typedef struct
{
    uint8_t   csiEnable;
}WlanCfgCsi_t;

typedef struct
{
    uint8_t   csiSolEnable;
    uint8_t   csiSolPeriod;
    uint8_t   csiSolExpiry;
}WlanCfgCsiSol_t;

typedef struct
{
    uint8_t   csiSolAddRemoveMac;
    uint8_t   csiSolMacAddress[MAC_ADDRESS_LEN];
}WlanCfgCsiSolSetMac_t;

typedef struct
{
    uint8_t  band;
    uint8_t  MaxTxPower;
    uint8_t  IsDfsChannel;
    uint16_t minChannel;
    uint16_t maxChannel;
    uint8_t  numOfChannels;
    uint32_t chanBitmap;
#ifdef CC35XX   
    uint8_t  customIndex; // This param is used in CC35xx only
    uint8_t  resetEntry;  // This param is used in CC35xx only
#endif
} WlanSetRegDomainCustomEntry_t;

typedef struct
{
    uint32_t    Connectivityslot;
}WlanConnectivityFWSlot_t;


#define WLAN_MAX_USER_LEN 32
#define WLAN_MAX_ANON_LEN 32
#define WLAN_MAX_PASS_LEN 32



typedef enum
{
    WLAN_TLS,
    WLAN_TTLS,
    WLAN_PEAP0,//PEAP0_MSCHAP
    WLAN_PEAP1,//not supported
    WLAN_PEAP2,
}WlanEapPhase1Val_t;

typedef enum
{
    WLAN_MSCHAPV2_TYPE,
    WLAN_TLS_TYPE,//not supported
    WLAN_PSK_TYPE,//not supported
    WLAN_GTC_TYPE
}WlanEapPhase2Val_t;

typedef struct
{
    WlanEapPhase1Val_t  eap_phase1_val;
    WlanEapPhase2Val_t  eap_phase2_val;

    uint8_t*      pEap_client_cert;  //pointer to the client certificate
    uint32_t            eap_client_cert_len;//length of the certificate without the null termination

    uint8_t*      pEap_ca_cert; //pointer to the ca certificate
    uint32_t            eap_ca_cert_len; //length of ca certificate without the null termination

    uint8_t*      pEap_private_key;///pointer to the key
    uint32_t            eap_private_key_len;//length of the key certificate without the null termination

    uint8_t             eapIdentity[WLAN_MAX_USER_LEN + 1]; //identity string
    uint32_t            eapIdentityLen; //identity len
    uint8_t             eapAnonymous[WLAN_MAX_ANON_LEN + 1];//anonymous string
    uint32_t            eapAnonUserLen;//anonymous string length
    uint32_t             KeyLength;
}WlanEapConnectParams_t;




/*****************************************************************************/
/* Data path CB function                                                     */
/*****************************************************************************/
typedef void (* WlanNetworkDataRecvCB_t)(WlanRole_e role_id,uint8_t *input,uint32_t input_len);

/******************************************************************************
                  Custom Regulatory Domain Macros and Functions
******************************************************************************/
#define WLAN_REG_CUSTOM_SET_BAND(_band, _DomainCustomEntry) (_DomainCustomEntry).band = (_band)

#define WLAN_REG_CUSTOM_SET_DFS(_IsDfsChannel, _DomainCustomEntry) (_DomainCustomEntry).IsDfsChannel = (_IsDfsChannel)

#define WLAN_REG_CUSTOM_SET_MAX_TX(_MaxTxPower, _DomainCustomEntry) (_DomainCustomEntry).MaxTxPower = (_MaxTxPower)

static inline void WlanRegCustomSetChannel(int channel, WlanSetRegDomainCustomEntry_t *DomainCustomEntry)
{
    if ((DomainCustomEntry->minChannel == 0 || DomainCustomEntry->minChannel > channel))
    {
        //Find Minimum channel entry
        DomainCustomEntry->minChannel = (uint16_t)channel;
    }
    if ((DomainCustomEntry->maxChannel == 0 || DomainCustomEntry->maxChannel < channel))
    {
        // Find Maximum channel entry
        DomainCustomEntry->maxChannel = (uint16_t)channel;
    }
    switch (DomainCustomEntry->band)
    {
    //Set bit for specific band
        case BAND_24GHZ:
        {
            DomainCustomEntry->chanBitmap |= 1 << (channel - 1); //set bit for channels 1-14 (2.4G)
            DomainCustomEntry->numOfChannels++; //Add 1 to number of channels
        }
        break;
        case BAND_5GHZ:
        {
            if (channel >= 36 && channel <= 64)
            {
                //5G channels 36-64
                DomainCustomEntry->chanBitmap |= 1 << ((channel - 36) / 4); //set bit for channels 36-64 (5G)
            }
            else if (channel >= 100 && channel <= 144)
            {
                //5G channels 100-144
                DomainCustomEntry->chanBitmap |= 1 << (((channel - 100) / 4) + 8); //set bit for channels 100-144 (5G)
            }
            else if (channel >= 149 && channel <= 169)
            {
                //5G channels 149-169
                DomainCustomEntry->chanBitmap |= 1 << (((channel - 149) / 4) + 20); //set bit for channels 149-169 (5G)
            }
            DomainCustomEntry->numOfChannels++; //Add 1 to number of channels
        }
        break;
        case JAPAN_49GHZ:
        {
            DomainCustomEntry->chanBitmap |= 1 << ((channel - 184) / 4 + 26); //set bit for channels 184-196 (6G)
            DomainCustomEntry->numOfChannels++; //Add 1 to number of channels
        }
        break;
    }
}
/*****************************************************************************/
/* USER API                                                                  */
/*****************************************************************************/
/*!
    \brief start the wlan

    This function is turn on the wlan device, initialize the upper mac thread,
    and initialize the SPI interface
    \param[in]      eventHandlerCB      event callback function

    \return         Zero on success, or a negative value if an error occurred

    \sa             Wlan_Stop

    \note           This API will power up the device and invoke the initialize the SPI i/f

    \warning
*/
int Wlan_Start(WlanEventHandlerCB_t eventHandlerCB);

/*!
    \brief stop the wlan

    This function is turn off the wlan device, and deactivate the SPI interface

    \param[in]      isRecovery      Type of wlan_stop.
                                    TRUE for Recovery mode
                                    FALSE for non recovery mode

    \return         Zero on success, or a negative value if an error occurred

    \sa             Wlan_Activate

    \note           This API will power off the device and invoke the deactivate the SPI i/f

    \warning
*/
int Wlan_Stop(uint32_t flags);

/*!
    \brief role up the wlan

    This function start the specific role with the parameters per role type.

    \param[in]      Configuration - role type
                    Configuration - *params (per role)
                                    RoleUpApCmd_t for AP
                                    RoleUpStaCmd_t for STA
                    timeout - Blocking API

    \return         role id, Zero on success, or a negative value if an error occurred

    \sa             Wlan_RoleDown

    \note           This API will start STA/AP depend on the role type

    \warning

    =====================================================

    ## Explanation of Custom Regulatory domain

        The Custom Regulatory domain feature is available only for station role and on CC33xx platforms.
        To utilize this feature, set the countryDomain field in the RoleUpStaParams struct to the following values:

        RoleUpStaParams.countryDomain[0] = 'C'
        RoleUpStaParams.countryDomain[1] = 'S'
        RoleUpStaParams.countryDomain[2] = 'I'

        --note!!! Custom domain configuration is only effective if at least one entry is configured \ref "Wlan_Set Type 'WLAN_SET_CUSTOM_DOMAIN_ENTRY'"
*/
int Wlan_RoleUp(WlanRole_e roleType, void * params, unsigned long int timeout);


/*!
    \brief role down the wlan

    This function is stop the specific role

    \param[in]      Configuration - role type
                    timeout - Blocking API

    \return         Zero on success, or a negative value if an error occurred

    \sa             Wlan_RoleUp

    \note           This API deactivate the role

    \warning
*/
int Wlan_RoleDown(WlanRole_e roleType, unsigned long int timeout);

/*!
    \brief Connect to wlan network as a station

    \param[in]      pName       Up to 32 bytes in case of STA the name is the SSID of the Access Point
    \param[in]      NameLen     Name length
    \param[in]      pMacAddr    6 bytes for MAC address
    \param[in]      SecType     Security type
                                security types options:
                                    WLAN_SEC_TYPE_OPEN
                                    WLAN_SEC_TYPE_WPA
                                    WLAN_SEC_TYPE_WPA_WPA2
                                    WLAN_SEC_TYPE_WPS_PBC
                                    WLAN_SEC_TYPE_WPS_PIN
                                    WLAN_SEC_TYPE_WPA2_PLUS
                                    WLAN_SEC_TYPE_WPA3
                                    WLAN_SEC_TYPE_WPA2_WPA3

    \param[in]      pPass       The password to be used for connection. There are two types of passwords:
                                  1. Regular Password: A string containing up to 63 characters.
                                  2. Pairwise Master Key (PMK): A string of exactly 64 characters (WLAN_PMK_KEY_SIZE).
                                     for example -  "e23fc3086d2e8296c00b700c39ade3564fc39303ba75a90b45bd1b9008c9f3fc"

    \param[in]      PassLen     The length of the password.
                                  For regular passwords, this value should be up to 63.
                                  For Pairwise Master Keys (PMK), use WLAN_PMK_KEY_SIZE.

    \param[in]      flags     Bitfield for connection options

    \return         Zero on success, or negative error code on failure


    \sa             Wlan_Disconnect

    \note          for enterprise connection please use Wlan_Connect_extended

*/
int Wlan_Connect(const signed char *pName, const int NameLen,
                        const unsigned char *pMacAddr, char SecType, const char *pPass,
                        const char PassLen, unsigned int flags);

/*!
    \brief Connect to wlan network as a station, it extended Wlan_Connect and enables to connect to enterprise networks

    \param[in]      pName       Up to 32 bytes in case of STA the name is the SSID of the Access Point
    \param[in]      NameLen     Name length
    \param[in]      pMacAddr    6 bytes for MAC address
    \param[in]      SecType     Security type
                                security types options:
                                    WLAN_SEC_TYPE_OPEN
                                    WLAN_SEC_TYPE_WPA
                                    WLAN_SEC_TYPE_WPA_WPA2
                                    WLAN_SEC_TYPE_WPS_PBC
                                    WLAN_SEC_TYPE_WPS_PIN
                                    WLAN_SEC_TYPE_WPA2_PLUS
                                    WLAN_SEC_TYPE_WPA3
                                    WLAN_SEC_TYPE_WPA2_WPA3

    \param[in]      pPass       The password to be used for connection. There are two types of passwords:
                                  1. Regular Password: A string containing up to 63 characters.
                                  2. Pairwise Master Key (PMK): A string of exactly 64 characters (WLAN_PMK_KEY_SIZE).
                                     for example -  "e23fc3086d2e8296c00b700c39ade3564fc39303ba75a90b45bd1b9008c9f3fc"

    \param[in]      PassLen     The length of the password.
                                  For regular passwords, this value should be up to 63.
                                  For Pairwise Master Keys (PMK), use WLAN_PMK_KEY_SIZE.

    \param[in]      flags     Bitfield for connection options

    \param[in]      params    parameters for enterprise connection


    \param[in]ENT   eap_phase1_val
    \param[in]ENT   eap_phase2_val
    \param[in]ENT
    \param[in]ENT   pEap_client_cert      pointer to the client certificate
    \param[in]ENT   eap_client_cert_len   length of the certificate without the null termination
    \param[in]ENT
    \param[in]ENT   pEap_ca_cert          pointer to the ca certificate
    \param[in]ENT   eap_ca_cert_len       length of ca certificate without the null termination
    \param[in]ENT
    \param[in]ENT   pEap_private_key      pointer to the key
    \param[in]ENT   eap_private_key_len   length of the key certificate without the null termination
    \param[in]ENT
    \param[in]ENT   eapIdentity[WLAN_MAX_USER_LEN + 1]   identity string
    \param[in]ENT   eapIdentityLen;                      identity len
    \param[in]ENT   eapAnonymous[WLAN_MAX_ANON_LEN + 1]  anonymous string
    \param[in]ENT   eapAnonUserLen;                      anonymous string length

    \return         Zero on success, or negative error code on failure


    \sa             Wlan_Disconnect

    \note for enterprise connection please pass on the params structure of type WlanEapConnectParams_t*

*/
int Wlan_Connect_extended(const signed char *pName, const int NameLen, const unsigned char *pMacAddr,
                 char SecType, const char *pPass, const char PassLen, unsigned int flags, void* params);


/*!
    \brief Wlan disconnect

    Disconnect connection

    \return         Zero disconnected done successfully, other already disconnected

    \sa             WlanConnect
    \note           belongs to \ref ext_api
    \warning
*/
int Wlan_Disconnect(WlanRole_e roleType,void* params);

/*!
    \brief Wlan scan

    scan the network for APs

    \param[in]      pPass     Up to 32 bytes password
    \param[in]      PassLen   Password length
    \param[in]      params    scan params

    \return         Zero disconnected done successfully, other already disconnected

    \sa             Wlan_Connect
    \note           belongs to \ref ext_api
    \warning
*/
int Wlan_Scan(WlanRole_e role, void* params,const unsigned char ScanResultsSize);

/*!
    \brief Wlan_Get

    Get commands and parameters to the FW

    \param[in]      wlanGetType  Wlan Get type (WlanGet_e)
    \param[in]      params       pointer to params

    \return         Zero on success,
                    other when failed


    \sa             Wlan_Set
    \note           belongs to \ref ext_api
    \warning
*/
int Wlan_Get(WlanGet_e wlanGetType, void *params);

/*!
    \brief Wlan_Set

    Set commands and parameters to the FW

    \param[in]      wlanSetType  Wlan Set type (WlanSet_e)
    \param[in]      params       pointer to params

    \return         Zero on success,
                    other when failed

    \sa             Wlan_Get
    \note           belongs to \ref ext_api
    \warning

    =====================================================

    ## Explanation of Specific Types

            ### Wlan_Set Type 'WLAN_SET_PRIMARY_CONNECTIVITY_FW'
            Set the primary connectivity firmware container.
            This function allows the user to choose between slot 1 and slot 2 to determine
            the primary container to use for connectivity firmware.
            The user must call Wlan_Set(WLAN_SET_PRIMARY_CONNECTIVITY_FW, &primaryStruct)
            before calling wlan_start in order to activate the selected primary slot.
            By default, the primary connectivity firmware container is set to slot 1.

            \param[in] WlanConnectivityFWSlot_t A struct containing the firmware slot parameter (1 or 2).

            ## Example of how to use for selecting slot 2 as primary.
            WlanConnectivityFWSlot_t primarySetStruct;
            primarySetStruct.Connectivityslot = 2;
            Wlan_Set(WLAN_SET_PRIMARY_CONNECTIVITY_FW, &primarySetStruct);

            ## Example of how to get the primary slot
            WlanConnectivityFWSlot_t primaryGetStruct;
            Wlan_Get(WLAN_GET_PRIMARY_CONNECTIVITY_FW, &primaryGetStruct);

            ### Wlan_Set Type 'WLAN_SET_CUSTOM_DOMAIN_ENTRY'

            Create your own custom domain by setting up to 7 entries in our table. Each entry allows you to specify your unique requirements.
            To set up a new entry using the WLAN_SET_CUSTOM_DOMAIN_ENTRY type, please provide the following parameters:

            \param[in] WlanSetRegDomainCustomEntry_t    Parameters of Entry

                           band             -     Entry Band to use - \param[in] WlanBand_e
                           chanBitmap       -     Channel bitmap contains supported channels

                           chanBitmap 2.4G:

                             +-----------------+------------------------------------------------------------+
                             |                 |                          Channel                           |
                             |     RESERVED    |                                                            |
                             |                 |ch14|ch13|ch12|ch11|ch10|ch9|ch8|ch7|ch6|ch5|ch4|ch3|ch2|ch1|
                             +-----------------+------------------------------------------------------------+
                        Bits:        14-31                                 0-13

                           chanBitmap 5G:

                             +-----------------+---------------------------------------------------------------------------------------------------------------------------------------------------+
                             |                 |                                                                 Channel                                                                           |
                             |     RESERVED    |                                                                                                                                                   |
                             |                 |ch169|ch165|ch161|ch157|ch153|ch149|ch144|ch140|ch136|ch132|ch128|ch124|ch120|ch116|ch112|ch108|ch104|ch100|ch64|ch60|ch56|ch52|ch48|ch44|ch40|ch36|
                             +-----------------+---------------------------------------------------------------------------------------------------------------------------------------------------+
                        Bits:       26-31                                                                           0-25


                           chanBitmap 4.9G:

                             +----------*-----------------------*---------------------------------------+
                             |          |     Channel           |                                       |
                             | RESERVED |     RESERVED          |                RESERVED               |
                             |          |ch196|ch192|ch188|ch184|                                       |
                             +----------*-----------------------*---------------------------------------+
                        Bits:   30-31           26-29                              0-25



                           MaxTxPower - Max Tx power to use
                           IsDfsChannel - Are the channels in the entry are DFS (TRUE - passive scan, FALSE - active scan)
                           minChannel - Min channel in chanBitmap
                           maxChannel - Max channel in chanBitmap
                           numOfChannels - Number of channels in chanBitmap

            ## Example of how to use MACROS and functions to set WlanSetRegDomainCustomEntry_t and set an entry

                Configure your custom domain with the following settings:
                Frequency Band: 2.4 GHz
                Channels: 2, 6, and 7 (selected channels)
                Maximum Transmit Power: 19 dBm
                DFS Mode: Disabled (Active Scan enabled)

                --note!!! WlanRegCustomSetChannel updates "minChannel","maxChannel", "numOfChannels" for each 'WlanSetRegDomainCustomEntry_t'
                *************************************************
                int ret_domain_entry_status;
                WlanSetRegDomainCustomEntry_t test_entry = {0};
                WLAN_REG_CUSTOM_SET_MAX_TX(19, test_entry);
                WLAN_REG_CUSTOM_SET_DFS(FALSE, test_entry);
                WLAN_REG_CUSTOM_SET_BAND(BAND_24GHZ, test_entry);    //It is required to set Band before using WlanRegCustomSetChannel
                WlanRegCustomSetChannel(CHANNEL_2, &test_entry);
                WlanRegCustomSetChannel(CHANNEL_6, &test_entry);
                WlanRegCustomSetChannel(CHANNEL_7, &test_entry);
                ret_domain_entry_status = Wlan_Set(WLAN_SET_CUSTOM_DOMAIN_ENTRY, (void *)&test_entry);

                ***************************************************

                Configure your custom domain with the following settings:
                Frequency Band: 5 GHz
                Channels: 40, 52, 56, 112, 120, and 124 (selected channels)
                Maximum Transmit Power: 16 dBm
                DFS Mode: Disabled (Passive Scan enabled)

                --note!!! WlanRegCustomSetChannel updates "minChannel","maxChannel", "numOfChannels" for each 'WlanSetRegDomainCustomEntry_t'
                *************************************************
                int ret_domain_entry_status;
                WlanSetRegDomainCustomEntry_t test_entry = {0};
                WLAN_REG_CUSTOM_SET_MAX_TX(16, test_entry);
                WLAN_REG_CUSTOM_SET_DFS(TRUE, test_entry);
                WLAN_REG_CUSTOM_SET_BAND(BAND_5GHZ, test_entry);      //It is required to set Band before using WlanRegCustomSetChannel
                WlanRegCustomSetChannel(CHANNEL_40, &test_entry);
                WlanRegCustomSetChannel(CHANNEL_52, &test_entry);
                WlanRegCustomSetChannel(CHANNEL_56, &test_entry);
                WlanRegCustomSetChannel(CHANNEL_112, &test_entry);
                WlanRegCustomSetChannel(CHANNEL_120, &test_entry);
                WlanRegCustomSetChannel(CHANNEL_124, &test_entry);
                ret_domain_status = Wlan_Set(WLAN_SET_CUSTOM_DOMAIN_ENTRY, (void *)&test_entry)

                *************************************************


                --note!!! When configuring channels using two or more entries, note that if the same channel number is specified in multiple entries,
                 the configuration from the last entry will override all previous entries.
                 ##Example
                *************************************************
                ///Configure test_entry_1 for Channel 40, Maximum TX Power of 16 dBm, and DFS Enable.
                int ret_domain_entry_status;

                WlanSetRegDomainCustomEntry_t test_entry_1 = {0};
                WLAN_REG_CUSTOM_SET_MAX_TX(16, test_entry_1);
                WLAN_REG_CUSTOM_SET_DFS(TRUE, test_entry_1);
                WLAN_REG_CUSTOM_SET_BAND(BAND_5GHZ, test_entry_1);      //It is required to set Band before using WlanRegCustomSetChannel
                WlanRegCustomSetChannel(CHANNEL_40, &test_entry_1);
                ret_domain_status = Wlan_Set(WLAN_SET_CUSTOM_DOMAIN_ENTRY, (void *)&test_entry_1)

                /// Configure test_entry_1 for Channel 40, Maximum TX Power of 20 dBm, and DFS Disable.
                WlanSetRegDomainCustomEntry_t test_entry_2 = {0};
                WLAN_REG_CUSTOM_SET_MAX_TX(20, test_entry_2);
                WLAN_REG_CUSTOM_SET_DFS(FALSE, test_entry_2);
                WLAN_REG_CUSTOM_SET_BAND(BAND_5GHZ, test_entry_2);      //It is required to set Band before using WlanRegCustomSetChannel
                WlanRegCustomSetChannel(CHANNEL_40, &test_entry_2);
                ret_domain_status = Wlan_Set(WLAN_SET_CUSTOM_DOMAIN_ENTRY, (void *)&test_entry_2)

                //RESULTS//
                Channel 40 will be configured based on test_entry_2, with a Maximum TX Power of 20 dBm and DFS Disabled.

                ### Wlan_Set Type 'WLAN_SET_LISTED_CHANNELS_FOR_SCAN'
                Set a list of channels for next/upcoming Scan/Connect
                This command sets a list of channels that will only be set when doing scan or connect, the list is used only
                one time, meaning you need to set it again if you cant to list specific channels for scan/connect, after you 
                scan or connect the listed channels will be resetted and the next scan/connect will be on all possible channels

                the WLAN_SET_LISTED_CHANNEL_FOR_SCAN set command takes struct of WlanSelectedChannelsArray_t which includes
                an allocated array of the list of channels we want to scan and the length that represents channels being set or not
                
                \param[in]  WlanSelectedChannelsArray_t

                ## Example of how to set a channels 1,3 ,5

                uint32_t channelsArray[3] = {1,3,5};

                WlanSelectedChannelsArray_t selectedChannels = {

                    .channelArray = channelsArray
                    .length = 3

                };
                Wlan_Set(WLAN_SET_LISTED_CHANNELS_FOR_SCAN, (void *)&selectedChannels);

                for setting all the channels to be scanned we can
                do the following

                Wlan_Set(WLAN_SET_LISTED_CHANNELS_FOR_SCAN, NULL);
                
*/
int Wlan_Set(WlanSet_e wlanSetType, void *params);


/*!
    \brief Wlan Ethernet Packet Send

    This function is used for the network stack to send ethernet packets over link in a certain role

    \param[in]      role          WLAN_ROLE_STA or WLAN_ROLE_AP
    \param[in]      inbuf         input buffer of ethernet packet
    \param[in]      inbuf_len     input buffer length of ethernet packet
    \param[in]      flags         reserved

    \return         Zero on success other, error

    \sa             Wlan_EtherPacketRecvRegisterCallback
    \note           this function copies the ethernet frame
    \warning
*/
int Wlan_EtherPacketSend(WlanRole_e role, uint8_t *inbuf, uint32_t inbuf_len,uint32_t flags);


/*!
    \brief Wlan Ethernet Packet recv callback

    This function is used for the network stack to attach the receive callback of ethernet
    packets to be called upon RX

    \param[in]      role                      WLAN_ROLE_STA or WLAN_ROLE_AP
    \param[in]      WlanNetworkDataRecvCB_t   recv callback function

    \return         Zero on success other, error

    \sa             Wlan_EtherPacketSend
    \warning
*/
int Wlan_EtherPacketRecvRegisterCallback(WlanRole_e role, WlanNetworkDataRecvCB_t cb);

/*!
    \brief Add profile

    When auto start is enabled, the device connects to a
    station from the profiles table. Up to SL_WLAN_MAX_PROFILES profiles 
    are supported.
    If several profiles configured the device chose
    the highest priority profile, within each priority group,
    device will chose profile based on security policy, signal
    strength, etc parameters.

    \param[in]      pName       Up to 32 bytes in case of STA the name is the SSID of the Access Point
    \param[in]      NameLen     Name length
    \param[in]      pMacAddr    6 bytes for MAC address
    \param[in]      pSecParam   Security parameters
                                security types options:
                                - CME_SEC_TYPE_OPEN
                                - CME_SEC_TYPE_WEP
                                - CME_SEC_TYPE_WPA_WPA2
    
    \param[in]      pSecExtParams  Enterprise parameters - identity, identity length,
                                   Anonymous, Anonymous length, CertIndex (not supported,
                                   certificates need to be placed in a specific file ID),
                                   EapMethod.
                                   Use NULL in case Enterprise parameters is not in use

    \param[in]      Priority    Profile priority. Lowest priority: 0, Highest priority: 15.
    \param[in]      Options     Not supported

    \return         Profile stored index on success, or negative error code on failure.

    \warning        Only one Enterprise profile is supported.
                    Please Note that in case of adding an existing profile (compared by pName,pMACAddr and security type)
                    the old profile will be deleted and the same index will be returned.

    \sa             Wlan_ProfileAdd      

*/
int Wlan_ProfileAdd(const signed char *pName, const int NameLen,
                        const unsigned char *pMacAddr, const WlanSecParams_t* pSecParams, const WlanSecParamsExt_t* pSecExtParams,
                        const uint32_t Priority, const uint8_t Hidden, const uint32_t Options);

/*!
    \brief Get profile

    Read profile from the device

    \param[in]      Index          Profile stored index, if index does not exists error is return
    \param[out]     pName          Up to 32 bytes, in case of sta mode the name of the Access Point
                                   (In case of p2p mode the name of the Remote Device)
    \param[out]     pNameLen       Name length
    \param[out]     pMacAddr       6 bytes for MAC address
    \param[out]     pSecParams     Security parameters. Security types options: 
									- WLAN_SEC_TYPE_OPEN
									- WLAN_SEC_TYPE_WPA_WPA2
									- WLAN_SEC_TYPE_WPA_ENT

								   Key and key length are not return. In case of p2p security type pin the key refers to pin code
                                   return due to security reasons.
    \param[out]     pSecExtParams  Enterprise parameters - identity, identity length,
                                   Anonymous, Anonymous length
                                   CertIndex (not supported), EapMethod.
    \param[out]     pPriority      Profile priority

    \param[out]     pHidden        Profile scan_ssid

    \return         Profile security type is returned (0 or positive number) on success, or negative error code on failure

    \sa             Wlan_ProfileGet

    \warning
*/

int Wlan_ProfileGet(const uint16_t Index, signed char* pName, int *pNameLen,
                        uint8_t *pMacAddr, WlanSecParams_t* pSecParams, WlanSecParamsExt_t* pSecExtParams,
                        uint32_t *pPriority, uint32_t *pHidden);


/*!
    \brief Delete WLAN profile

    Delete WLAN profile

    \param[in]      Index   number of profile to delete. Possible values are 0 to TBD
                            Index value SL_WLAN_DEL_ALL_PROFILES will delete all saved profiles

    \return         Zero on success or a negative error code on failure

    \sa             Wlan_ProfileDel

    \warning
*/
int Wlan_ProfileDel(const uint8_t Index);

#ifdef __cplusplus
}
#endif

#endif /* USER_INTEFACE_WLAN_IF_H_ */

/** @} End UPPER_MAC */
