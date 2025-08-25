/*
 * Copyright (c) 2022-2025, Texas Instruments Incorporated
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
#ifndef ti_drivers_RCL_commands_ble_cs_h__include
#define ti_drivers_RCL_commands_ble_cs_h__include

#include <ti/drivers/rcl/RCL_Command.h>
#include <ti/drivers/rcl/RCL_Buffer.h>
#include <ti/drivers/rcl/handlers/ble_cs.h>

#include <ti/drivers/utils/List.h>

/* Forward declaration of types */
typedef struct RCL_CMD_BLE_CS_t                           RCL_CmdBleCs;
typedef struct RCL_CMD_BLE_CS_STEP_INTERNAL_t             RCL_CmdBleCs_Step_Internal;
typedef struct RCL_CMD_BLE_CS_STEP_RESULT_INTERNAL_t      RCL_CmdBleCs_StepResult_Internal;
typedef struct RCL_CMD_BLE_CS_STEP_t                      RCL_CmdBleCs_Step;
typedef struct RCL_CMD_BLE_CS_SUBEVENT_RESULTS_t          RCL_CmdBleCs_SubeventResults;
typedef struct RCL_CMD_BLE_CS_SUBEVENT_RESULTS_CONTINUE_t RCL_CmdBleCs_SubeventResultsContinue;
typedef struct RCL_CMD_BLE_CS_S2R_t                       RCL_CmdBleCs_S2r;
typedef struct RCL_CMD_BLE_CS_STATS_t                     RCL_CmdBleCs_Stats;
typedef struct RCL_CMD_BLE_CS_IQ_SAMPLE_t                 RCL_CmdBleCs_IQSample;
typedef struct RCL_CMD_BLE_CS_DC_SAMPLE_t                 RCL_CmdBleCs_DCSample;
typedef struct RCL_CMD_BLE_CS_PRECAL_t                    RCL_CmdBleCs_Precal;
typedef struct RCL_CMD_BLE_CS_PRECAL_TABLE_t              RCL_CmdBleCs_PrecalTable;
typedef struct RCL_CMD_BLE_CS_PRECAL_ENTRY_t              RCL_CmdBleCs_PrecalEntry;

typedef struct RCL_CMD_BLE_CS_STEP_RESULTS_TONE_t         RCL_CmdBleCs_Tone;
typedef struct RCL_CMD_BLE_CS_STEP_RESULTS_t              RCL_CmdBleCs_Result;
typedef struct RCL_CMD_BLE_CS_STEP_RESULTS_I0_t           RCL_CmdBleCs_ResultI0;
typedef struct RCL_CMD_BLE_CS_STEP_RESULTS_R0_t           RCL_CmdBleCs_ResultR0;
typedef struct RCL_CMD_BLE_CS_STEP_RESULTS_IR1_t          RCL_CmdBleCs_ResultIR1;
typedef struct RCL_CMD_BLE_CS_STEP_RESULTS_IR2_t          RCL_CmdBleCs_ResultIR2;
typedef struct RCL_CMD_BLE_CS_STEP_RESULTS_IR3_t          RCL_CmdBleCs_ResultIR3;

/* Command IDs for generic commands */
#define RCL_CMDID_BLE_CS                        0x1101U
#define RCL_CMDID_BLE_CS_PRECAL                 0x1102U

/* Helper macros to convert between time units */
#define RCL_BLE_CS_US_TO_MCE_TIMER(x)           ((x)*48)
#define RCL_BLE_CS_US_TO_PBE_TIMER(x)           ((x)*4)
#define RCL_BLE_CS_MCE_TIMER_TO_US(x)           ((x)/48)
#define RCL_BLE_CS_PBE_TIMER_TO_US(x)           ((x)/12)
#define RCL_BLE_CS_MCE_TIMER_TO_PBE_TIMER(x)    ((x)/4)

/* Helper macros for constants */
#define RCL_BLE_CS_MAX_NUM_ANT                  4
#define RCL_BLE_CS_MAX_NUM_ANT_PATH             5
#define RCL_BLE_CS_MAX_PAYLOAD_SIZE             4
#define RCL_BLE_CS_NUM_CORR                     3
#define RCL_BLE_CS_NUM_STIM                     2
#define RCL_BLE_CS_MAX_S2R_LEN                  1024
#define RCL_BLE_CS_NUM_RX_GAIN_LEVEL            2
#define RCL_BLE_CS_STEP_RX_GAIN_DB              3
#define RCL_BLE_CS_NUM_PRECAL_CHANNELS          8

/**
 *  @brief BLE Channel Sounding IQ Sample
 *
 *  Container to store complex vectors
 */
struct RCL_CMD_BLE_CS_IQ_SAMPLE_t {
    int16_t i;   /*!< Sample I-branch */
    int16_t q;   /*!< Sample Q-branch */
};

/**
 *  @brief BLE Channel Sounding
 *
 *  Command to execute a BLE Channel Sounding event
 */
struct RCL_CMD_BLE_CS_t {
    RCL_Command common;
    struct {
        uint16_t role:2;                         /*!< Role of the device @ref RCL_CmdBleCs_Role */
        uint16_t phy:2;                          /*!< Phy used for packet exchange @ref RCL_CmdBleCs_Phy */
        uint16_t repeatSteps:1;                  /*!< Enable continuous repetition of step list */
        uint16_t chFilterEnable:1;               /*!< Enable filtering of restricted channels at (2402, 2403, 2425, 2426, 2427, 2479, 2480 MHz) */
        uint16_t precal:1;                       /*!< Enable usage of DC precalibration values */
        uint16_t reserved:1;
        uint16_t nSteps:8;                       /*!< Total number of steps within the BLE CS Sub-Event */
    } mode;

    struct {
        uint8_t select;                          /*!< Antenna path configuration by index @ref RCL_CmdBleCs_AntennaConfig */
        uint8_t gpoMask;                         /*!< IO mask defining the actively used pins for antenna control */
        uint8_t gpoVal[RCL_BLE_CS_MAX_NUM_ANT];  /*!< IO value to be used to activate the corresponding antenna ([0]:ANT0, [1]:ANT1, [2]:ANT3, [3]:ANT3) */
    } antennaConfig;

    struct {
        uint16_t tFcs;                           /*!< Config of time of Frequency Change period @ref RCL_CmdBleCs_Tfcs */
        uint16_t tFm;                            /*!< Config of time of Frequency Measurement period */
        uint16_t tPm;                            /*!< Config of time of Phase Measurement Duration @ref RCL_CmdBleCs_Tpm */
        uint16_t tIp1;                           /*!< Config of time of Interlude Period 1 @ref RCL_CmdBleCs_Tip */
        uint16_t tIp2;                           /*!< Config of time of Interlude Period 2 @ref RCL_CmdBleCs_Tip */
        uint16_t tRxWideningR0;                  /*!< Config of additional time of RX timeout of mode-0 steps on reflector. */
        uint16_t tSw;                            /*!< Config of time of Antenna Switching duration @ref RCL_CmdBleCs_Tsw */
        uint16_t tSwAdjustA;                     /*!< Time adjustment of first set of antenna switching within a step (Pkt-Tn / ...) */
        uint16_t tSwAdjustB;                     /*!< Time adjustment of second set of antenna switching within a step (... / Tn-Pkt) */
    } timing;

    struct {
        RCL_Command_TxPower txPower;              /*!< Transmit power */
        uint8_t  rxGain;                          /*!< 0: Automatic Gain Control enabled, 1...15: Index value of manual RX gain @ref RCL_CmdBleCs_RxGain */
    } frontend;

    int16_t foffOverride;                         /*!< Frequency offset compensation override value in [4xFOFF = 4x (FRF/2^21)] units. */
    uint8_t foffOverrideEnable:1;                 /*!< Disables automatic frequency offset estimation and enforces the use of the provided override value */

    uint8_t reportFormat;                         /*!< Select a vendor specific report format instead of the default HCI LE CS subevent result */

    RCL_CmdBleCs_PrecalTable *precalTable;        /*!< Pointer to a table contains DC values from precalibration */
    RCL_CmdBleCs_StepResult_Internal *results;    /*!< Pointer to result list */
    RCL_CmdBleCs_Stats *stats;                    /*!< Pointer to statistics structure */
    List_List           stepBuffers;              /*!< Linked list of steps to be executed */
    List_List           stepBuffersDone;          /*!< Linked list of steps have been executed */
    List_List           resultBuffers;            /*!< Linked list of empty result buffers */
    List_List           resultBuffersDone;        /*!< Linked list of result buffers containing data */
    List_List           s2rBuffers;               /*!< Linked list of empty s2r containers */
    List_List           s2rBuffersDone;           /*!< Linked list of s2r containers with data */
};

/* Default configuration of command */
#define RCL_CmdBleCs_Default()                          \
{                                                       \
    .common = RCL_Command_Default(RCL_CMDID_BLE_CS,     \
                                  RCL_Handler_BLE_CS),  \
}
#define RCL_CmdBleCs_DefaultRuntime() (RCL_CmdBleCs) RCL_CmdBleCs_Default()

/**
 *  @brief BLE Channel Sounding Step
 *
 *  Descriptor to configure a single step within the BLE Channel Sounding Event
 */
struct RCL_CMD_BLE_CS_STEP_INTERNAL_t {
    uint16_t channelIdx;                             /*!< Integer index of channel information (0: 2402MHz) */
    uint16_t mode;                                   /*!< Step mode @ref RCL_CmdBleCs_StepMode */
    uint16_t toneExtension;                          /*!< Configuration of tone extension */
    uint16_t payloadLen;                             /*!< Length of payload in units of 32bit words @ref RCL_CmdBleCs_Payload */
    uint16_t foffErr;                                /*!< Internal! Used for frequency offset compensation */
    uint16_t tAdjustA;                               /*!< Internal! Used for timegrid adjustment */
    uint16_t tAdjustB;                               /*!< Internal! Used for timegrid adjustment */
    uint16_t reserved0;
    RCL_CmdBleCs_IQSample dcComp[RCL_BLE_CS_NUM_RX_GAIN_LEVEL]; /*!< Internal! Used for DC compensation with precalibrated values */
    uint32_t payloadTx[RCL_BLE_CS_MAX_PAYLOAD_SIZE]; /*!< Payload to transmit containing random bit sequence (TX) */
    uint32_t payloadRx[RCL_BLE_CS_MAX_PAYLOAD_SIZE]; /*!< Expected payload to receive containing random bit sequence (RX) */
    uint32_t aaTx;                                   /*!< Access Address to be transmitted */
    uint32_t aaRx;                                   /*!< Access Address to be received */
    uint8_t  antennaPermIdx;                         /*!< Index of entry to be used from the antenna permutation table @ref RCL_CmdBleCs_AntennaConfig */
    uint8_t  antennaPacket;                          /*!< Index of physical antenna to be used for all packet exchanges @ref RCL_CMD_BLE_CS_PacketAntenna_e */
    uint16_t antennaSequence;                        /*!< Internal! Decoded antenna control sequence based on permutation table */
    uint16_t tStep;                                  /*!< Internal! The total duration of step dynamically calculated */
    uint16_t tAntennaA;                              /*!< Internal! Antenna timing adjustment */
    uint16_t tAntennaB;                              /*!< Internal! Antenna timing adjustment */
    uint16_t reserved1;
};

/**
 *  @brief BLE Channel Sounding QQ data
 *
 *  Container to store RSSI data for postprocess of the quality of tone
 */
typedef struct {
    int16_t magnMin;     /*!< Minimum of signal magnitude during TPM */
    int16_t magnMax;     /*!< Maximum of signal magnitude during TPM */
    int16_t magnAvg;     /*!< Average of signal magnitude during TPM */
    int16_t magnAvgdB;   /*!< Average of signal magnitude during TPM in dB */
} MagnData;

/**
 *  @brief BLE Channel Sounding Step Result
 *
 *  Container to store the results of a single step within the BLE Channel Sounding Event
 */
struct RCL_CMD_BLE_CS_STEP_RESULT_INTERNAL_t {
    uint16_t channelIdx;                                    /*!< Integer index of channel information (0: 2402MHz) */
    uint16_t mode;                                          /*!< Step mode @ref RCL_CmdBleCs_StepMode */
    uint16_t toneExtension;                                 /*!< Configuration of tone extension */
    uint16_t payloadLen;                                    /*!< Length of payload in units of 32bit words @ref RCL_CmdBleCs_Payload */
    uint16_t reserved;
    int16_t  foffMeasured;                                  /*!< Frequency offset between devices (only set on Initiator Mode 0) */
    uint16_t pktResult;                                     /*!< Result of packet reception @ref RCL_CmdBleCs_PacketResult */
    uint8_t  gain;                                          /*!< LNA gain used (Low/High)*/
    int8_t   pktRssi;                                       /*!< Receiver Signal Strength Indicator [dBm] captured during packet reception */
    uint16_t rtt;                                           /*!< RTT timestamp */
    uint16_t corr[RCL_BLE_CS_NUM_CORR];                     /*!< Correlator before-peak-after values for fractional time estimation */
    RCL_CmdBleCs_IQSample dc;                               /*!< DC offset measured on packet */
    RCL_CmdBleCs_IQSample pct[RCL_BLE_CS_MAX_NUM_ANT_PATH]; /*!< Phase Correction Terms per antenna path (incl. extension) */
    uint32_t payload[RCL_BLE_CS_MAX_PAYLOAD_SIZE];          /*!< Payload containing random bit sequence (RX) */
    MagnData magn[RCL_BLE_CS_MAX_NUM_ANT_PATH];             /*!< Received magnitude data of tone per antenna path for quality estimation (incl. extension) */

    /* Echoed by PBE */
    uint8_t  antennaPermIdx;                                /*!< Index of entry to be used from the antenna permutation table @ref RCL_CmdBleCs_AntennaConfig */
    uint8_t  antennaPacket;                                 /*!< Index of physical antenna to be used for all packet exchanges @ref RCL_CMD_BLE_CS_PacketAntenna_e */
    uint16_t reserved1;
};

/**
 *  @brief BLE Channel Sounding S2R Sample type definition
 *
 *  One S2R sample can be interpreted multiple ways, depending of the signal source
 */
typedef union {
    uint32_t word;                 /*!< Sample format for copying data */
    uint8_t  byte[4];              /*!< Sample format of the Decoding Stage */
    RCL_CmdBleCs_IQSample iq;      /*!< Sample format of the Front End Stage */
} S2RSample;

/**
 *  @brief BLE Channel Sounding S2R IQ Data
 *
 *  Container to store raw S2R samples of a BLE Channel Sounding Step
 */
struct RCL_CMD_BLE_CS_S2R_t {
    uint8_t   idx;                 /*!< The index of entry in the step list which the data belongs to */
    uint8_t   phy;                 /*!< Phy the samples captured on */
    uint8_t   channel;             /*!< Frequency channel */
    uint8_t   payloadLen;          /*!< Payload length in bits */
    uint32_t  payload0;            /*!< Payload bits [00:31] */
    uint32_t  payload1;            /*!< Payload bits [32:63] */
    uint32_t  payload2;            /*!< Payload bits [64:95] */
    uint32_t  payload3;            /*!< Payload bits [96:127] */
    uint16_t  iqLength;            /*!< Number of valid iq samples in data[] */
    uint16_t  reserved;
    S2RSample data[];              /*!< Raw samples captured */
};

/**
 *  @brief BLE Channel Sounding Statistics
 *
 *  Container to store the statistical outputs of the BLE Channel Sounding Event
 */
struct RCL_CMD_BLE_CS_STATS_t {
    uint16_t nStepsWritten;                             /*!< Number of steps sent to the PBE through the FIFO */
    uint16_t nResultsRead;                              /*!< Number of results read from the PBE through the FIFO */
    uint16_t nStepsDone;                                /*!< Number of steps have been executed by the PBE */
    uint16_t nRxOk;                                     /*!< Number of steps where pktResult == OK, filled by the CM0 */
    uint16_t nRxNok;                                    /*!< Number of steps where pktResult != OK, filled by the CM0 */
    uint8_t  nS2RDone;                                  /*!< Number of Samples-To-Ram containers filled by the CM0 */
    int8_t   lastRssi;                                  /*!< Last valid RSSI received by the PBE */
    int16_t  lastFoff;                                  /*!< Last valid frequency offset received by the PBE */
    int16_t  foffComp;                                  /*!< Frequency offset compensation value */
    uint8_t  numAntennaPath;                            /*!< Number of true antenna paths (1...4) */
    uint8_t  rplScaler;                                 /*!< Log2 scaler of the internal PCT values when converting to HCI format */
    uint16_t reserved;
};

/**
 *  @brief Enumerator of phy types
 *
 *  Describes the phy packets shall be transmitted
 */
enum RCL_CMD_BLE_CS_Phy_e {
    RCL_CmdBleCs_Phy_1M,    /*!< (M) 1Mbps BT=0.5 */
    RCL_CmdBleCs_Phy_2M,    /*!< (O) 2Mbps BT=0.5 */
    RCL_CmdBleCs_Phy_2M2BT, /*!< (O) 2Mbps BT=2.0 */
    RCL_CmdBleCs_Phy_Length
};

/**
 *  @brief Enumerator of role types
 *
 *  Describes the role of the device during a BLE channel sounding event
 */
enum RCL_CMD_BLE_CS_Role_e {
    RCL_CmdBleCs_Role_Initiator, /*!< (C.1) TX-RX */
    RCL_CmdBleCs_Role_Reflector, /*!< (C.2) RX-TX */
    RCL_CmdBleCs_Role_Length
};

/**
 *  @brief Enumerator of step types
 *
 *  Describes the available modes of a BLE CS step
 */
enum RCL_CMD_BLE_CS_StepMode_e {
    RCL_CmdBleCs_StepMode_0,       /*!< (M) Pkt-Pkt-Tn */
    RCL_CmdBleCs_StepMode_1,       /*!< (M) Pkt-Pkt */
    RCL_CmdBleCs_StepMode_2,       /*!< (M) Tn-Tn */
    RCL_CmdBleCs_StepMode_3,       /*!< (O) Pkt-Tn-Tn-Pkt */
    RCL_CmdBleCs_StepMode_Length
};

/**
 *  @brief Enumerator of packet status options
 *
 *  Describes the available packet status words
 */
enum RCL_CMD_BLE_CS_PacketResult_e {
    RCL_CmdBleCs_PacketResult_Error     = -1,   /*!< NA for mode-2 steps, otherwise command execution error. */
    RCL_CmdBleCs_PacketResult_Ok        =  0,   /*!< The packet reception was succesfull */
    RCL_CmdBleCs_PacketResult_BitError  =  1,   /*!< The packet was received with one or more bit errors */
    RCL_CmdBleCs_PacketResult_Lost      =  2,   /*!< The packet reception was terminated by timeout */
    RCL_CmdBleCs_PacketResult_Length
};

/**
 *  @brief Enumerator of tFcs duration options
 *
 *  Describes the available durations for frequency change
 */
enum RCL_CMD_BLE_CS_Tfcs_e {
    RCL_CmdBleCs_Tfcs_80us,     /*!< (C.1) */
    RCL_CmdBleCs_Tfcs_100us,
    RCL_CmdBleCs_Tfcs_120us,
    RCL_CmdBleCs_Tfcs_150us,    /*!< (M) */
    RCL_CmdBleCs_Tfcs_Length,
};

/**
 *  @brief Enumerator of tPm duration options
 *
 *  Describes the available durations for phase measurement
 */
enum RCL_CMD_BLE_CS_Tpm_e {
    RCL_CmdBleCs_Tpm_10us,
    RCL_CmdBleCs_Tpm_20us,
    RCL_CmdBleCs_Tpm_40us,      /*!< (M) */
    RCL_CmdBleCs_Tpm_Length,
};

/**
 *  @brief Enumerator of tIp duration
 *
 *  Describes the available durations for interlude period
 */
enum RCL_CMD_BLE_CS_Tip_e {
    RCL_CmdBleCs_Tip_40us,      /*!< (C.1) */
    RCL_CmdBleCs_Tip_50us,
    RCL_CmdBleCs_Tip_60us,
    RCL_CmdBleCs_Tip_80us,      /*!< (C.1) */
    RCL_CmdBleCs_Tip_145us,     /*!< (M) */
    RCL_CmdBleCs_Tip_Length,
};

/**
 *  @brief Enumerator of antenna switch duration
 *
 *  Describes the available durations for antenna switching transient
 */
enum RCL_CMD_BLE_CS_Tsw_e {
    RCL_CmdBleCs_Tsw_0us,       /*!< (M) for 1x1 */
    RCL_CmdBleCs_Tsw_1us,
    RCL_CmdBleCs_Tsw_2us,
    RCL_CmdBleCs_Tsw_4us,
    RCL_CmdBleCs_Tsw_10us,      /*!< (M) */
    RCL_CmdBleCs_Tsw_Length
};

/**
 *  @brief Enumerator of antenna configuration
 *
 *  Describes the available values for configuration of
 *  antenna control for PCT measurement.
 */
enum RCL_CMD_BLE_CS_AntennaConfig_e {
    RCL_CmdBleCs_AntennaConfig_1x1,     /*!< (M) */
    RCL_CmdBleCs_AntennaConfig_2x1,
    RCL_CmdBleCs_AntennaConfig_3x1,
    RCL_CmdBleCs_AntennaConfig_4x1,
    RCL_CmdBleCs_AntennaConfig_1x2,
    RCL_CmdBleCs_AntennaConfig_1x3,
    RCL_CmdBleCs_AntennaConfig_1x4,
    RCL_CmdBleCs_AntennaConfig_2x2,
    RCL_CmdBleCs_AntennaConfig_Length,
};

/**
 *  @brief Enumerator of payload length
 *
 *  Describes the available payload lengths
 */
enum RCL_CMD_BLE_CS_Payload_e {
    RCL_CmdBleCs_Payload_None,
    RCL_CmdBleCs_Payload_32bit,
    RCL_CmdBleCs_Payload_64bit,
    RCL_CmdBleCs_Payload_96bit,
    RCL_CmdBleCs_Payload_128bit,
    RCL_CmdBleCs_Payload_Length
};

/**
 *  @brief Enumerator of RX gain
 *
 *  Describes the available RX gain settings
 */
enum RCL_CMD_BLE_CS_RxGain_e {
    RCL_CmdBleCs_RxGain_Auto = 0,
    RCL_CmdBleCs_RxGain_Low  = 7,
    RCL_CmdBleCs_RxGain_High = 15
};

/**
 *  @brief Enumerator of tone quality
 *
 *  Describes the classification of tone quality
 */
enum RCL_CMD_BLE_CS_ToneQuality_e {
    RCL_CmdBleCs_ToneQuality_High,
    RCL_CmdBleCs_ToneQuality_Medium,
    RCL_CmdBleCs_ToneQuality_Low,
    RCL_CmdBleCs_ToneQuality_Unavailable,
    RCL_CmdBleCs_ToneQuality_Length
};

/**
 *  @brief Enumerator of tone extension slot
 *
 *  Describes the classification of tone extension slot
 */
enum RCL_CMD_BLE_CS_ToneExtensionSlot_e {
    RCL_CmdBleCs_ToneExtensionSlot_Disabled,
    RCL_CmdBleCs_ToneExtensionSlot_Enabled_NoToneExpected,
    RCL_CmdBleCs_ToneExtensionSlot_Enabled_ToneExpected,
    RCL_CmdBleCs_ToneExtensionSlot_Length
};

/**
 *  @brief Enumerator of report format
 *
 *  Selects the format of step results
 */
enum RCL_CMD_BLE_CS_ReportFormat_e {
    RCL_CmdBleCs_ReportFormat_HCI = 0,
    RCL_CmdBleCs_ReportFormat_Custom = 1
};

/*****************************************************
    DC precalibration
*****************************************************/

/**
 *  @brief Callback function for DC precalibration feature
 *
 *  Describes the method that can select the right compensation value from the available precalibration table.
 */
typedef void (*RCL_CmdBleCs_PrecalCallback)(RCL_CmdBleCs_PrecalTable *table, uint8_t channel, RCL_CmdBleCs_IQSample *pHigh, RCL_CmdBleCs_IQSample *pLow);

/**
 *  @brief BLE Channel Sounding DC Sample
 *
 *  Container to store DC information
 */
struct RCL_CMD_BLE_CS_DC_SAMPLE_t {
    int16_t  i;             /*!< DC I-branch */
    int16_t  q;             /*!< DC Q-branch */
    int8_t   phaseStart;    /*!< Cordic at start of integration */
    int8_t   phaseStop;     /*!< Cordic at stop of integration */
    uint16_t magnMin;       /*!< Minimum value of signal magnitude during integration */
    uint16_t magnMax;       /*!< Maximum value of signal magnitude during integration */
    uint16_t magnAvg;       /*!< Average value of signal magnitude during integration */
};

/**
 *  @brief DC precalibration entry
 *
 *  Data structure to store a single DC precalibration entry.
 */
struct RCL_CMD_BLE_CS_PRECAL_ENTRY_t {
    uint8_t channel;
    uint8_t lowValid  : 1;
    uint8_t highValid : 1;
    RCL_CmdBleCs_DCSample high;
    RCL_CmdBleCs_DCSample low;
};

/**
 *  @brief DC precalibration table
 *
 *  Data structure to store and use the DC precalibration table.
 */
struct RCL_CMD_BLE_CS_PRECAL_TABLE_t {
    RCL_CmdBleCs_PrecalCallback callback;
    uint32_t timestamp;
    uint8_t  highThreshold;
    uint8_t  lowThreshold;
    uint8_t  rxGain;
    int8_t   temperature;
    uint8_t  chSpacing;
    uint8_t  numEntries : 7;
    uint8_t  valid      : 1;
    RCL_CmdBleCs_PrecalEntry entries[];
};

/* Default callback implemented in the driver */
void RCL_Handler_BLE_CS_PrecalDefaultCallback(RCL_CmdBleCs_PrecalTable *table, uint8_t channel, RCL_CmdBleCs_IQSample *pHigh, RCL_CmdBleCs_IQSample *pLow);

/* Default configuration of DC precalibration */
#define RCL_CmdBleCs_PrecalTable_Default()                     \
{                                                              \
    .callback      = RCL_Handler_BLE_CS_PrecalDefaultCallback, \
    .timestamp     = 0,                                        \
    .highThreshold = 50,                                       \
    .lowThreshold  = 10,                                       \
    .rxGain        = 0xF7,                                     \
    .temperature   = 0,                                        \
    .chSpacing     = 10,                                       \
    .numEntries    = 8,                                        \
    .valid         = 0,                                        \
    .entries       = {{.channel =  5},                         \
                      {.channel = 15},                         \
                      {.channel = 25},                         \
                      {.channel = 35},                         \
                      {.channel = 45},                         \
                      {.channel = 55},                         \
                      {.channel = 65},                         \
                      {.channel = 75}},                        \
}

/**
 *  @brief DC precalibration command
 *
 *  Command to run precalibration for a list of channels.
 */
struct RCL_CMD_BLE_CS_PRECAL_t {
    RCL_Command               common;
    RCL_CmdBleCs_PrecalTable *table;
};

/* Default configuration of DC precalibration command */
#define RCL_CmdBleCs_Precal_Default(pTable)                    \
{                                                              \
    .common = RCL_Command_Default(RCL_CMDID_BLE_CS_PRECAL,     \
                                  RCL_Handler_BLE_CS_Precal),  \
    .table  = pTable                                           \
}
#define RCL_CmdBleCs_Precal_DefaultRuntime(pTable) (RCL_CmdBleCs_Precal) RCL_CmdBleCs_Precal_Default(pTable)

/*****************************************************
    HCI interface
*****************************************************/

/* Special values for categorical parameters */
enum RCL_CMD_BLE_CS_PacketAntenna_e {
    RCL_CmdBleCs_PacketAntenna_1 = 1, /*!< Default choice */
    RCL_CmdBleCs_PacketAntenna_2 = 2,
    RCL_CmdBleCs_PacketAntenna_3 = 3,
    RCL_CmdBleCs_PacketAntenna_4 = 4,
};

enum RCL_CMD_BLE_CS_Nadm_e {
    RCL_CmdBleCs_Nadm_ExtremelyUnlikely,
    RCL_CmdBleCs_Nadm_VeryUnlikely,
    RCL_CmdBleCs_Nadm_Unlikely,
    RCL_CmdBleCs_Nadm_Possible,
    RCL_CmdBleCs_Nadm_Likely,
    RCL_CmdBleCs_Nadm_VeryLikely,
    RCL_CmdBleCs_Nadm_ExtremelyLikely,
    RCL_CmdBleCs_Nadm_Unknown = 0xFF /*!< Default value for RTT types that do not have a random or sounding sequence. */
};

/* Special values for linear parameters */
#define RCL_CMD_BLE_CS_FREQCOMP_NA  0xC000
#define RCL_CMD_BLE_CS_TOAD_NA      0x8000
#define RCL_CMD_BLE_CS_RSSI_NA      0x7F

/* Opcodes */
#define RCL_CMD_BLE_CS_SUBEVENT_RESULTS_OPCODE           0x31
#define RCL_CMD_BLE_CS_SUBEVENT_RESULTS_CONTINUE_OPCODE  0x32

/**
 *  @brief Container format for a single step in the subevent
 *
 *  Compressed format used within a multibuffer
 */
struct RCL_CMD_BLE_CS_STEP_t {
    uint32_t channelIdx : 7;                         /*!< Integer index of channel information (0: 2402MHz) */
    uint32_t mode : 2;                               /*!< Step mode @ref RCL_CmdBleCs_StepMode */
    uint32_t antennaPacket: 3;                       /*!< Index of physical antenna for the packet exchange @ref RCL_CMD_BLE_CS_PacketAntenna_e */
    uint32_t antennaPermIdx : 5;                     /*!< Index of entry to be used from the antenna permutation table @ref RCL_CmdBleCs_AntennaConfig */
    uint32_t toneExtension : 2;                      /*!< Enable tone extension, [0]=first tone (transmitted by initiator), [1]=second tone (transmitted by reflector) */
    uint32_t payloadLen : 3;                         /*!< Length of payload in units of 32bit words @ref RCL_CmdBleCs_Payload */
    uint32_t reserved : 10;
    uint32_t aaTx;                                   /*!< Access Address to be transmitted */
    uint32_t aaRx;                                   /*!< Access Address to be received */
    uint32_t payloadTx[RCL_BLE_CS_MAX_PAYLOAD_SIZE]; /*!< Payload to transmit containing random bit sequence (TX) */
    uint32_t payloadRx[RCL_BLE_CS_MAX_PAYLOAD_SIZE]; /*!< Expected payload to receive containing random bit sequence (RX) */
};

/**
 *  @brief Container format for a batch of step results in the subevent (first segment)
 *
 *  The data[] field contains the results in mode specific format (size varies with step-mode)
 */
struct RCL_CMD_BLE_CS_SUBEVENT_RESULTS_t {
    uint8_t  subeventCode;
    uint16_t connectionHandle;
    uint8_t  configID;
    uint16_t startAclConnectionEvent;
    uint16_t procedureCounter;
    int16_t  frequencyCompensation;
    int8_t   referencePowerLevel;
    uint8_t  procedureDoneStatus;
    uint8_t  subeventDoneStatus;
    uint8_t  abortReason;
    uint8_t  numAntennaPath;
    uint8_t  numStepsReported;
    uint8_t  data[];
} __attribute__ ((packed));

/**
 *  @brief Container format for a batch of step results in the subevent (second+ segment)
 *
 *  The data[] field contains the results in mode specific format (size varies with step-mode)
 */
struct RCL_CMD_BLE_CS_SUBEVENT_RESULTS_CONTINUE_t {
    uint8_t  subeventCode;
    uint16_t connectionHandle;
    uint8_t  configID;
    uint8_t  procedureDoneStatus;
    uint8_t  subeventDoneStatus;
    uint8_t  abortReason;
    uint8_t  numAntennaPath;
    uint8_t  numStepsReported;
    uint8_t  data[];
} __attribute__ ((packed));

/**
 *  @brief Container format for tones
 *
 */
struct RCL_CMD_BLE_CS_STEP_RESULTS_TONE_t {
    uint32_t i       : 12;
    uint32_t q       : 12;
    uint32_t quality : 8;
};

/**
 *  @brief Container format for common section of step results
 *
 */
struct RCL_CMD_BLE_CS_STEP_RESULTS_t {
    /* Common */
    uint8_t mode;
    uint8_t channel;
    uint8_t dataLength;
    uint8_t data[];
} __attribute__((packed));

/**
 *  @brief Container format for mode-0 step results
 *
 */
struct RCL_CMD_BLE_CS_STEP_RESULTS_I0_t {
    /* Common */
    uint8_t mode;
    uint8_t channel;
    uint8_t dataLength;
    /* Packet */
    uint8_t packetAAQuality;
    int8_t  packetRssi;
    uint8_t packetAntenna;
    /* Frequency */
    int16_t measuredFreqOffset;
};

/**
 *  @brief Container format for mode-0 step results
 *
 */
struct RCL_CMD_BLE_CS_STEP_RESULTS_R0_t {
    /* Common */
    uint8_t mode;
    uint8_t channel;
    uint8_t dataLength;
    /* Packet */
    uint8_t packetAAQuality;
    int8_t  packetRssi;
    uint8_t packetAntenna;
};

/**
 *  @brief Container format for mode-1 step results
 *
 */
struct RCL_CMD_BLE_CS_STEP_RESULTS_IR1_t {
    /* Common */
    uint8_t mode;
    uint8_t channel;
    uint8_t dataLength;
    /* Packet */
    uint8_t packetAAQuality;
    uint8_t nadm;
    int8_t  packetRssi;
    int16_t packetToF;
    uint8_t packetAntenna;
};

/**
 *  @brief Container format for mode-2 step results
 *
 */
struct RCL_CMD_BLE_CS_STEP_RESULTS_IR2_t {
    /* Common */
    uint8_t mode;
    uint8_t channel;
    uint8_t dataLength;
    /* Tones */
    uint8_t antennaPermutationIndex;
    RCL_CmdBleCs_Tone tones[];
};

/**
 *  @brief Container format for mode-3 step results
 *
 */
struct RCL_CMD_BLE_CS_STEP_RESULTS_IR3_t {
    /* Common */
    uint8_t mode;
    uint8_t channel;
    uint8_t dataLength;
    /* Packet */
    uint8_t packetAAQuality;
    uint8_t nadm;
    int8_t  packetRssi;
    int16_t packetToF;
    uint8_t packetAntenna;
    /* Tones */
    uint8_t antennaPermutationIndex;
    RCL_CmdBleCs_Tone tones[];
};

typedef enum   RCL_CMD_BLE_CS_Phy_e                       RCL_CmdBleCs_Phy;
typedef enum   RCL_CMD_BLE_CS_Role_e                      RCL_CmdBleCs_Role;
typedef enum   RCL_CMD_BLE_CS_StepMode_e                  RCL_CmdBleCs_StepMode;
typedef enum   RCL_CMD_BLE_CS_PacketResult_e              RCL_CmdBleCs_PacketResult;
typedef enum   RCL_CMD_BLE_CS_Tfcs_e                      RCL_CmdBleCs_Tfcs;
typedef enum   RCL_CMD_BLE_CS_Tpm_e                       RCL_CmdBleCs_Tpm;
typedef enum   RCL_CMD_BLE_CS_Tip_e                       RCL_CmdBleCs_Tip;
typedef enum   RCL_CMD_BLE_CS_Tsw_e                       RCL_CmdBleCs_Tsw;
typedef enum   RCL_CMD_BLE_CS_AntennaConfig_e             RCL_CmdBleCs_AntennaConfig;
typedef enum   RCL_CMD_BLE_CS_Payload_e                   RCL_CmdBleCs_Payload;
typedef enum   RCL_CMD_BLE_CS_RxGain_e                    RCL_CmdBleCs_RxGain;
typedef enum   RCL_CMD_BLE_CS_ToneQuality_e               RCL_CmdBleCs_ToneQuality;
typedef enum   RCL_CMD_BLE_CS_ToneExtensionSlot_e         RCL_CmdBleCs_ToneExtensionSlot;

typedef enum   RCL_CMD_BLE_CS_ReportFormat_e              RCL_CmdBleCs_ReportFormat;
typedef enum   RCL_CMD_BLE_CS_PacketAntenna_e             RCL_CmdBleCs_PacketAntenna;
typedef enum   RCL_CMD_BLE_CS_Nadm_e                      RCL_CmdBleCs_Nadm;

#endif

