/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/* this file behaves like a config.h, comes first */
#include <platform/internal/CHIPDeviceLayerInternal.h>

#include <platform/ConnectivityManager.h>
#include <platform/internal/GenericConnectivityManagerImpl_UDP.ipp>

#if INET_CONFIG_ENABLE_TCP_ENDPOINT
#include <platform/internal/GenericConnectivityManagerImpl_TCP.ipp>
#endif

#if CHIP_DEVICE_CONFIG_ENABLE_CHIPOBLE
#include <platform/internal/GenericConnectivityManagerImpl_BLE.ipp>
#endif
#include <platform/internal/GenericConnectivityManagerImpl_WiFi.ipp>

#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/ConnectivityManager.h>
#include <platform/internal/BLEManager.h>

#include <platform/ti/cc35xx/CC35XXConfig.h>
#include <platform/ti/cc35xx/ConnectivityManagerImpl.h>
#include <platform/ti/cc35xx/NetworkCommissioningWiFiDriver.h>
#include <platform/ti/cc35xx/ti_wifi_structs.h>

#include <lwip/dns.h>
#include <lwip/ip_addr.h>
#include <lwip/nd6.h>
#include <lwip/netif.h>

#include <type_traits>

#include <network_lwip.h>
#include <ti/drivers/net/wifi/wifi_host_driver/inc_adapt/osi_kernel.h>
#include "wlan_if_cc35xx.h"

#include <ti/drivers/Board.h>

/* Driver Header files */
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerWFF3.h>

#if !CHIP_DEVICE_CONFIG_ENABLE_WIFI_STATION
#error "WiFi Station support must be enabled when building for CC35XX"
#endif

#if !CHIP_DEVICE_CONFIG_ENABLE_WIFI_AP
#error "WiFi AP support must be enabled when building for CC35XX"
#endif

#define SHOW_WARNING(ret, errortype) Report("\n\r[line:%d, error code:%d] %s\n\r", __LINE__, ret, errortype);

#define OS_ERROR ("OS error, please refer \"NETAPP ERRORS CODES\" section in errno.h")

#define HWREG(x) (*((volatile unsigned long *) (x))) // TODO temporary need to be removed
#define ICACHE_BASE 0x41902000                       // TODO temporary need to be removed, only for M3, M33 has different address


using namespace ::chip;
using namespace ::chip::Inet;
using namespace ::chip::System;

extern "C" void cc35xxLog(const char * aFormat, ...);
extern uint32_t isIp;

namespace chip {
namespace DeviceLayer {

ConnectivityManagerImpl ConnectivityManagerImpl::sInstance;

ConnectivityManager::WiFiStationMode ConnectivityManagerImpl::_GetWiFiStationMode(void)
{
    cc35xxLog("ConnectivityManagerImpl::_GetWiFiStationMode()\n\r");
    return IS_BIT_SET(ActiveNetIfBitMap, NET_IF_STA_BIT) ? kWiFiStationMode_Enabled : kWiFiStationMode_Disabled;
}

bool ConnectivityManagerImpl::_IsWiFiStationEnabled(void)
{
    cc35xxLog("ConnectivityManagerImpl::_IsWiFiStationEnabled()\n\r");
    return GetWiFiStationMode() == kWiFiStationMode_Enabled;
}

CHIP_ERROR ConnectivityManagerImpl::_SetWiFiStationMode(WiFiStationMode val)
{
    cc35xxLog("ConnectivityManagerImpl::_SetWiFiStationMode(%d)\n\r", val);
    return CHIP_NO_ERROR;
}

bool ConnectivityManagerImpl::_IsWiFiStationProvisioned(void)
{
    cc35xxLog("ConnectivityManagerImpl::_IsWiFiStationProvisioned()\n\r");
    return NetworkCommissioning::CC35XXWiFiDriver::GetInstance().HasStagedNetwork();
}

void ConnectivityManagerImpl::_ClearWiFiStationProvision(void)
{
    cc35xxLog("ConnectivityManagerImpl::_ClearWiFiStationProvision()\n\r");
    NetworkCommissioning::CC35XXWiFiDriver::GetInstance().ClearNetworkConfig();
}

CHIP_ERROR ConnectivityManagerImpl::_SetWiFiAPMode(WiFiAPMode val)
{
    cc35xxLog("ConnectivityManagerImpl::_SetWiFiAPMode(%d)\n\r", val);
    return CHIP_NO_ERROR;
}

void ConnectivityManagerImpl::_DemandStartWiFiAP(void)
{
    cc35xxLog("ConnectivityManagerImpl::_DemandStartWiFiAP()\n\r");
}

void ConnectivityManagerImpl::_StopOnDemandWiFiAP(void)
{
    cc35xxLog("ConnectivityManagerImpl::_StopOnDemandWiFiAP()\n\r");
}

void ConnectivityManagerImpl::_MaintainOnDemandWiFiAP(void)
{
    cc35xxLog("ConnectivityManagerImpl::_MaintainOnDemandWiFiAP()\n\r");
}

void ConnectivityManagerImpl::_SetWiFiAPIdleTimeout(System::Clock::Timeout val)
{
    cc35xxLog("ConnectivityManagerImpl::_SetWiFiAPIdleTimeout()\n\r");
}
CHIP_ERROR ConnectivityManagerImpl::_GetAndLogWifiStatsCounters(void)
{
    cc35xxLog("ConnectivityManagerImpl::_GetAndLogWifiStatsCounters()\n\r");
    return CHIP_NO_ERROR;
}

System::Clock::Timeout ConnectivityManagerImpl::_GetWiFiStationReconnectInterval(void)
{
    cc35xxLog("ConnectivityManagerImpl::_GetWiFiStationReconnectInterval()\n\r");
    return System::Clock::Seconds16(15);
}

CHIP_ERROR ConnectivityManagerImpl::_SetWiFiStationReconnectInterval(System::Clock::Timeout val)
{
    cc35xxLog("ConnectivityManagerImpl::_SetWiFiStationReconnectInterval()\n\r");
    return CHIP_NO_ERROR;
}

System::Clock::Timeout ConnectivityManagerImpl::_GetWiFiAPIdleTimeout(void)
{
    cc35xxLog("ConnectivityManagerImpl::_GetWiFiAPIdleTimeout()\n\r");
    return System::Clock::Seconds16(0);
}

bool ConnectivityManagerImpl::_HaveIPv4InternetConnectivity(void)
{
    cc35xxLog("ConnectivityManagerImpl::_HaveIPv4InternetConnectivity()\n\r");
    return IS_STA_CONNECTED(app_CB.Status) && GET_STATUS_BIT(app_CB.Status, STATUS_BIT_IP_ACQUIRED);
}

bool ConnectivityManagerImpl::_HaveIPv6InternetConnectivity(void)
{
    cc35xxLog("ConnectivityManagerImpl::_HaveIPv6InternetConnectivity()\n\r");
    return GET_STATUS_BIT(app_CB.Status, STATUS_BIT_IPV6_ACQUIRED);
}

bool ConnectivityManagerImpl::_HaveServiceConnectivity(void)
{
    cc35xxLog("ConnectivityManagerImpl::_HaveServiceConnectivity()\n\r");
    return _HaveIPv4InternetConnectivity() || _HaveIPv6InternetConnectivity();
}

bool ConnectivityManagerImpl::_CanStartWiFiScan(void)
{
    cc35xxLog("ConnectivityManagerImpl::_CanStartWiFiScan()\n\r");
    return IS_BIT_SET(ActiveNetIfBitMap, NET_IF_STA_BIT) && !GET_STATUS_BIT(app_CB.Status, STATUS_BIT_SCAN_RUNNING);
}

ConnectivityManager::WiFiAPMode ConnectivityManagerImpl::_GetWiFiAPMode(void)
{
    cc35xxLog("ConnectivityManagerImpl::_GetWiFiAPMode()\n\r");
    return kWiFiAPMode_NotSupported;
}

// ==================== ConnectivityManager Platform Internal Methods ====================

/*
 *  ======== WlanStackEventHandler ========
 *
 *  Callback from the Wi-Fi Stack to deliver events to the application.
 *  This is registered via Wlan_Start(WlanStackEventHandler)
 *
 */
void WlanStackEventHandler(WlanEvent_t * pWlanEvent)
{
    void * staif = NULL;
    if (!pWlanEvent)
    {
        return;
    }

    Report("\n\r--> WlanStackEventHandler Id = %d\n\r", pWlanEvent->Id);

    switch (pWlanEvent->Id)
    {
    case WLAN_EVENT_CONNECT: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_CONNECT\n\r");
        WlanEventConnect_t * pWlanEventConnect = &pWlanEvent->Data.Connect;

        // Check connection status
        if (pWlanEventConnect->Status < 0)
        {
            Report("\n\r[WLAN EVENT HANDLER] Connection failed with status: %d\n\r", pWlanEventConnect->Status);
            NetworkCommissioning::CC35XXWiFiDriver::GetInstance().OnConnectResult(false);
            osi_SyncObjSignal(&app_CB.CON_CB.connectEventSyncObj);
            break;
        }

        char ssid[WLAN_SSID_MAX_LENGTH + 1];
        char bssid[WLAN_BSSID_LENGTH + 1];

        os_memset(ssid, 0, sizeof(ssid));
        os_memcpy(ssid, pWlanEventConnect->SsidName, pWlanEventConnect->SsidLen);
        os_memset(bssid, 0, sizeof(bssid));
        os_memcpy(bssid, pWlanEventConnect->Bssid, WLAN_BSSID_LENGTH);

        Report("[WLAN EVENT HANDLER] STA Connected to the AP: %s, "
               "BSSID: %x:%x:%x:%x:%x:%x, "
               "Channel : %d"
               "\n\r",
               ssid, bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5], pWlanEventConnect->Channel);

        // Set connection status bit
        SET_STATUS_BIT(app_CB.Status, STATUS_BIT_STA_CONNECTION);

        // Copy connection info
        os_memcpy(app_CB.CON_CB.ConnectionSSID, pWlanEventConnect->SsidName, pWlanEventConnect->SsidLen);
        os_memcpy(app_CB.CON_CB.ConnectionBSSID, pWlanEventConnect->Bssid, WLAN_BSSID_LENGTH);

        staif = network_get_sta_if();
        if (staif != NULL)
        {
            network_set_up(staif);
        }

        NetworkCommissioning::CC35XXWiFiDriver::GetInstance().OnConnectResult(true);
        osi_SyncObjSignal(&app_CB.CON_CB.connectEventSyncObj);
    }
    break;

    case WLAN_EVENT_DISCONNECT: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_DISCONNECT\n\r");
        WlanEventDisconnect_t * pWlanEventDisconnect = &pWlanEvent->Data.Disconnect;

        Report("[WLAN EVENT HANDLER] STA Disconnected - Reason Code: %d\n\r", pWlanEventDisconnect->ReasonCode);

        CLR_STATUS_BIT(app_CB.Status, STATUS_BIT_STA_CONNECTION);
        osi_SyncObjSignal(&app_CB.CON_CB.disconnectEventSyncObj);
    }
    break;
    case WLAN_EVENT_SCAN_RESULT: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_SCAN_RESULT\n\r");
        WlanEventScanResult_t * pEventScanResult = &pWlanEvent->Data.ScanResult;

        uint32_t numResults = pEventScanResult->NetworkListResultLen;
        Report("[WLAN EVENT HANDLER] Number of scan results received: %d \n\r", numResults);
        Report("[WLAN SCAN] Results:\n\r");

        // Store scan results in the WiFi driver
        NetworkCommissioning::CC35XXWiFiDriver & wifiDriver = NetworkCommissioning::CC35XXWiFiDriver::GetInstance();
        NetworkCommissioning::WiFiScanResponse * scanResults = nullptr;
        uint8_t maxScanResults = wifiDriver.GetScanResultsBuffer(scanResults);
        uint8_t scanCount    = (numResults < maxScanResults) ? numResults : maxScanResults;

        for (uint32_t index = 0; index < scanCount; index++)
        {
            const WlanNetworkEntry_t & entry = pEventScanResult->NetworkListResult[index];

            Report("SCAN : SSID = %s, CH = %d, RSSI = %d\n\r", entry.Ssid, entry.Channel, entry.Rssi);

            uint8_t ssidLen = entry.SsidLen < WLAN_SSID_MAX_LENGTH ? entry.SsidLen : WLAN_SSID_MAX_LENGTH;

            scanResults[index].ssidLen = ssidLen;
            memcpy(scanResults[index].ssid, entry.Ssid, ssidLen);
            memcpy(scanResults[index].bssid, entry.Bssid, 6);
            scanResults[index].channel = entry.Channel;
            scanResults[index].rssi    = entry.Rssi;

            // Map SDK SecurityInfo bits to Matter WiFiSecurityBitmap
            using WiFiSecBitmap = chip::app::Clusters::NetworkCommissioning::WiFiSecurityBitmap;
            uint8_t secBitmap   = WLAN_SCAN_RESULT_SEC_TYPE_BITMAP(entry.SecurityInfo);
            scanResults[index].security.ClearAll();
            if (secBitmap == 0)
                scanResults[index].security.Set(WiFiSecBitmap::kUnencrypted);
            if (secBitmap & 0x01)
                scanResults[index].security.Set(WiFiSecBitmap::kWep);
            if (secBitmap & 0x02)
                scanResults[index].security.Set(WiFiSecBitmap::kWpaPersonal);
            if (secBitmap & 0x04)
                scanResults[index].security.Set(WiFiSecBitmap::kWpa2Personal);
            if (secBitmap & 0x08)
                scanResults[index].security.Set(WiFiSecBitmap::kWpa3Personal);

            wifiDriver.IncrementScanResultCount();
        }

        // No separate WLAN_EVENT_SCAN_COMPLETED event exists for STA scanning.
        // All results are delivered in this single event, so invoke the callback now.
        CLR_STATUS_BIT(app_CB.Status, STATUS_BIT_SCAN_RUNNING);
        osi_SyncObjSignal(&app_CB.eventCompletedScanObj);
        auto scanCallback = wifiDriver.GetScanCallback();
        if (scanCallback != nullptr)
        {
            auto * cb  = scanCallback;
            auto * iter = wifiDriver.SetupAndGetScanIterator();
            wifiDriver.SetScanCallback(nullptr);
            wifiDriver.SetScanInProgress(false);
            DeviceLayer::SystemLayer().ScheduleLambda([cb, iter]() {
                    cb->OnFinished(NetworkCommissioning::Status::kSuccess, CharSpan(), iter);
            });
        }
        else
        {
            ChipLogError(DeviceLayer, "[WLAN EVENT HANDLER] No scan callback registered!");
        }
    }
    break;
    case WLAN_EVENT_ADD_PEER: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_ADD_PEER\n\r");
    }
    break;
    case WLAN_EVENT_REMOVE_PEER: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_REMOVE_PEER\n\r");
    }
    break;
    case WLAN_EVENT_CONNECTING: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_CONNECTING\n\r");
        WlanEventConnecting_t * pWlanEventConnecting = &pWlanEvent->Data.Connecting;

        char ssid[WLAN_SSID_MAX_LENGTH + 1];
        char bssid[WLAN_BSSID_LENGTH + 1];

        /* Copy new connection SSID and BSSID to global parameters */
        os_memset(ssid, 0, sizeof(ssid));
        os_memcpy(ssid, pWlanEventConnecting->SsidName, pWlanEventConnecting->SsidLen);
        os_memset(bssid, 0, sizeof(bssid));
        os_memcpy(bssid, pWlanEventConnecting->Bssid, WLAN_BSSID_LENGTH);

        Report("[WLAN EVENT HANDLER] STA Connecting to the AP: %s, "
               "BSSID: %x:%x:%x:%x:%x:%x, "
               "\n\r",
               ssid, bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
    }
    break;
    case WLAN_EVENT_ACTION_FRAME_RX: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_ACTION_FRAME_RX\n\r");
    }
    break;
    case WLAN_EVENT_ASSOCIATED: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_ASSOCIATED\n\r");
    }
    break;
    case WLAN_EVENT_AP_EXT_WPS_SETTING_FAILED: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_AP_EXT_WPS_SETTING_FAILED\n\r");
    }
    break;
    case WLAN_EVENT_BLE_ENABLED: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_BLE_ENABLED\n\r");
    }
    break;
    case WLAN_EVENT_CS_FINISH: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_CS_FINISH\n\r");
    }
    break;
    case WLAN_EVENT_ROC_DONE: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_ROC_DONE\n\r");
    }
    break;
    case WLAN_EVENT_CROC_DONE: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_CROC_DONE\n\r");
    }
    break;
    case WLAN_EVENT_SEND_ACTION_DONE: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_SEND_ACTION_DONE\n\r");
    }
    break;
    case WLAN_EVENT_EXTENDED_SCAN_RESULT: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_EXTENDED_SCAN_RESULT\n\r");
    }
    break;
    case WLAN_EVENT_P2P_GROUP_STARTED: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_P2P_GROUP_STARTED\n\r");
    }
    break;
    case WLAN_EVENT_P2P_GROUP_REMOVED: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_P2P_GROUP_REMOVED\n\r");
    }
    break;
    case WLAN_EVENT_P2P_SCAN_COMPLETED: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_P2P_SCAN_COMPLETED\n\r");
    }
    break;
    case WLAN_EVENT_P2P_GROUP_FORMATION_FAILED: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_P2P_GROUP_FORMATION_FAILED\n\r");
    }
    break;
    case WLAN_EVENT_P2P_PEER_NOT_FOUND: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_P2P_PEER_NOT_FOUND\n\r");
    }
    break;
    case WLAN_EVENT_CONNECT_PERIODIC_SCAN_COMPLETE: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_CONNECT_PERIODIC_SCAN_COMPLETE\n\r");
    }
    break;
    case WLAN_EVENT_FW_CRASH: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_FW_CRASH\n\r");
    }
    break;
    case WLAN_EVENT_COMMAND_TIMEOUT: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_COMMAND_TIMEOUT\n\r");
    }
    break;
    case WLAN_EVENT_GENERAL_ERROR: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_GENERAL_ERROR\n\r");
    }
    break;
    case WLAN_EVENT_BSS_TRANSITION_INITIATED: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_BSS_TRANSITION_INITIATED\n\r");
    }
    break;
    case WLAN_EVENT_PEER_AGING: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_PEER_AGING\n\r");
    }
    break;
    case WLAN_EVENT_ERROR: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_ERROR\n\r");
    }
    break;
    case WLAN_EVENT_AUTHENTICATION_REJECTED: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_AUTHENTICATION_REJECTED\n\r");
        Report("\n\r[WLAN EVENT] Authentication rejected - check credentials\n\r");
        NetworkCommissioning::CC35XXWiFiDriver::GetInstance().OnConnectResult(false);
        osi_SyncObjSignal(&app_CB.CON_CB.connectEventSyncObj);
    }
    break;
    case WLAN_EVENT_ASSOCIATION_REJECTED: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_ASSOCIATION_REJECTED\n\r");
        Report("\n\r[WLAN EVENT] Association rejected\n\r");
        NetworkCommissioning::CC35XXWiFiDriver::GetInstance().OnConnectResult(false);
        osi_SyncObjSignal(&app_CB.CON_CB.connectEventSyncObj);
    }
    break;
    case WLAN_EVENT_WPS_INVALID_PIN: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_WPS_INVALID_PIN\n\r");
    }
    break;
    case WLAN_EVENT_AP_WPS_START_FAILED: {
        Report("\n\r--> WlanStackEventHandler WLAN_EVENT_AP_WPS_START_FAILED\n\r");
    }
    break;
    default: {
        Report("\n\r--> WlanStackEventHandler !! UNEXPECTED !! Event ID: %d\n\r", pWlanEvent->Id);
    }
    break;
    }
}

int32_t initAppVariables(void)
{
    int32_t ret = 0;

    app_CB.Status = 0;
    app_CB.Role   = WLAN_ROLE_RESERVED;
    app_CB.Exit   = FALSE;

    os_memset(&app_CB.CmdBuffer, 0x0, CMD_BUFFER_LEN);
    os_memset(&app_CB.gDataBuffer, 0x0, sizeof(app_CB.gDataBuffer));
    os_memset(&app_CB.CON_CB, 0x0, sizeof(app_CB.CON_CB));

    ret = osi_SyncObjCreate(&app_CB.CON_CB.disconnectEventSyncObj);
    if (ret != 0)
    {
        SHOW_WARNING(ret, OS_ERROR);
        return (-1);
    }

    ret = osi_SyncObjCreate(&app_CB.CON_CB.connectEventSyncObj);
    if (ret != 0)
    {
        SHOW_WARNING(ret, OS_ERROR);
        return (-1);
    }

    ret = osi_SyncObjCreate(&app_CB.CON_CB.eventCompletedSyncObj);
    if (ret != 0)
    {
        SHOW_WARNING(ret, OS_ERROR);
        return (-1);
    }
    ret = osi_SyncObjCreate(&app_CB.eventCompletedScanObj);
    if (ret != 0)
    {
        SHOW_WARNING(ret, OS_ERROR);
        return (-1);
    }
    ret = osi_SyncObjCreate(&app_CB.CON_CB.dhcpIprecvSyncObj);
    if (ret != 0)
    {
        SHOW_WARNING(ret, OS_ERROR);
        return (-1);
    }

    ret = osi_SyncObjCreate(&app_CB.CON_CB.staRoleupSyncObj);
    if (ret != 0)
    {
        SHOW_WARNING(ret, OS_ERROR);
        return (-1);
    }

    ret = osi_SyncObjCreate(&app_CB.CON_CB.staRoledownSyncObj);
    if (ret != 0)
    {
        SHOW_WARNING(ret, OS_ERROR);
        return (-1);
    }

    return (ret);
}

CHIP_ERROR ConnectivityManagerImpl::_Init()
{
    int32_t ret = 0;

    HWREG(ICACHE_BASE + 0x84) |= 0x00000001;
    HWREG(ICACHE_BASE + 0x4) |= 0xc0000000;

    Board_init();

    // Initialize LWIP
    initAppVariables();
    network_stack_init();

    Report("\n\r\n\r");
    Report("**** CC35XX Wi-Fi Init ****\n\r");

    Report("Disabling Power Management (DISABLE_SLEEP)\n\r");
    Power_setConstraint(PowerWFF3_DISALLOW_SLEEP);

    Report("\n\r** Wlan_Start(<StackHandler>)\n\r");
    ret = Wlan_Start(WlanStackEventHandler);
    if (ret == 0)
    {
        SET_BIT_IN_BITMAP(ActiveNetIfBitMap, NET_IF_IS_UP);
        Report("Wlan_Start success!\n\r");

        // Configure power management (ELP mode for CC35XX)
        uint32_t powerManagement = (uint32_t) POWER_MANAGEMENT_ELP_MODE;
        ret                      = Wlan_Set(WLAN_SET_POWER_MANAGEMENT, &powerManagement);
        if (ret == 0)
        {
            Report("Power management (ELP mode) configured successfully\n\r");
        }
        else
        {
            Report("Power management configuration failed: %d\n\r", ret);
        }
    }
    else
    {
        Report("Wlan_Start failed: %d\n\r", ret);
        return CHIP_ERROR_INTERNAL;
    }

    Report("\n\r** Wlan_Set(WLAN_SET_TX_CTRL) **\n\r");
    WlanCtrlBlk_t CtrlBlkParam;
    CtrlBlkParam.TxSendPaceThresh      = 1;
    CtrlBlkParam.TransmitQOnTxComplete = 1; // Changed from 0 to 1 to match working example
    CtrlBlkParam.TxSendPaceTimeoutMsec = 1; // Changed from 16 to 1 to match working example
    ret                                = Wlan_Set(WLAN_SET_TX_CTRL, &CtrlBlkParam);
    if (ret == 0)
    {
        Report("Wlan_Set(WLAN_SET_TX_CTRL) success!\n\r");
    }
    else
    {
        Report("Wlan_Set(WLAN_SET_TX_CTRL) failed: %d\n\r", ret);
    }

    // Load saved WiFi credentials from KVS
    NetworkCommissioning::CC35XXWiFiDriver & wifiDriver = NetworkCommissioning::CC35XXWiFiDriver::GetInstance();
    wifiDriver.Init();
    
     if (!wifiDriver.HasStagedNetwork())
    {
        // No commissioned credentials. Delete any stale profiles from a previous
        // boot or failed commissioning attempt to prevent auto-connect to an old AP.
        wifiDriver.DeleteWifiProfile();        
    }   

    Report("\n\r** Wlan_RoleUp(WLAN_ROLE_STA) **\n\r");

    // Check if network station is already active
    if (IS_BIT_SET(ActiveNetIfBitMap, NET_IF_STA_BIT))
    {
        Report("\n\rNetwork Station Is Already Active.\n\r");
    }
    else
    {
        CLR_STATUS_BIT(app_CB.Status, STATUS_BIT_STA_CONNECTION);

        RoleUpStaCmd_t RoleUpStaParams;
        os_memset(&RoleUpStaParams, 0, sizeof(RoleUpStaCmd_t));

        // Set 2.4G and 5G bands for CC35XX
        uint8_t sta_wifi_band = (uint8_t) BAND_SEL_BOTH;
        Wlan_Set(WLAN_SET_STA_WIFI_BAND, &sta_wifi_band);

        // Configure WPS parameters (disabled by default for Matter)
        RoleUpStaParams.wpsDisabled      = TRUE;
        RoleUpStaParams.countryDomain[0] = '\0';
        RoleUpStaParams.countryDomain[1] = '\0';

        // Add network interface and role up
        network_stack_add_if_sta();

        // Wait for network stack interface to be ready
        ret = osi_SyncObjWait(&(app_CB.CON_CB.staRoleupSyncObj), OSI_WAIT_FOR_SECOND * 10);
        if (OSI_OK != ret)
        {
            Report("\n\r[ERROR]_Init: Failed waiting for staRoleup sync object (%d)\n\r", ret);
            network_stack_remove_if_sta();
            return CHIP_ERROR_TIMEOUT;
        }

        ret = Wlan_RoleUp(WLAN_ROLE_STA, &RoleUpStaParams, WLAN_WAIT_FOREVER);
        if (ret < 0)
        {
            network_stack_remove_if_sta();
            Report("\n\r[ERROR]_Init: Wlan_RoleUp Failed with error code: %d\n\r", ret);
            return CHIP_ERROR_INTERNAL;
        }

        SET_BIT_IN_BITMAP(ActiveNetIfBitMap, NET_IF_STA_BIT);
        app_CB.Role = WLAN_ROLE_STA;

        // Short delay after role up
        os_sleep(1, 0);

        Report("Wlan_RoleUp success!\n\r");
    }

    // Set autoPolicy=1 to enable CME auto-connect when a Wi-Fi profile exists.
    // Must be set even if the device is not commissioned to allow auto-connect
    // to the Wi-Fi profile added during BLE commissioning
    WlanPolicySetGet_t connPolicy;
    os_memset(&connPolicy, 0, sizeof(connPolicy));
    connPolicy.autoPolicy = 1;
    connPolicy.fastPolicy  = 0;
    Wlan_Set(WLAN_SET_CONNECTION_POLICY, &connPolicy);    

    // Check if the device was commissioned before and has a staged network
    if (!wifiDriver.HasStagedNetwork())
    {
        Report("No saved WiFi credentials; device awaiting commissioning.\n\r");
        return CHIP_NO_ERROR;
    }

    // Profile already in flash from previous commissioning. autoPolicy=1 is set,
    // so when STA role activates, CME auto-connect fires automatically via
    // cmeProfileManagerConfigChange(). Just wait for WLAN_EVENT_CONNECT.
    osi_SyncObjClear(&(app_CB.CON_CB.connectEventSyncObj));

    if (!IS_STA_CONNECTED(app_CB.Status))
    {
        Report("Waiting for connection event...\n\r");
        ret = osi_SyncObjWait(&(app_CB.CON_CB.connectEventSyncObj), OSI_WAIT_FOR_SECOND * 60);
        if (ret != OSI_OK)
        {
            Report("\n\r[ERROR]_Init: Timeout connecting to AP (error: %d)\n\r", ret);
            Wlan_Disconnect(WLAN_ROLE_STA, nullptr);
            // Factory reset won't work on stack init failure, so return no error
            return CHIP_NO_ERROR;
        }
    }

    if (!IS_STA_CONNECTED(app_CB.Status))
    {
        Report("\n\r[ERROR]_Init: Connection event fired but not connected (bad credentials?)\n\r");
        // Factory reset won't work on stack init failure, so return no error
        return CHIP_NO_ERROR;
    }

    Report("Connected to AP successfully!\n\r");
    Report("Waiting for IP address assignment...\n\r");
    const int kMaxIpWaitSeconds = 30;
    for (int i = 0; i < kMaxIpWaitSeconds && isIp == 0; i++)
    {
        Report(".");
        os_sleep(1, 0);
    }

    if (isIp == 0)
    {
        Report("\n\r[ERROR]_Init: Timeout waiting for IP address\n\r");
        // Factory reset won't work on stack init failure, so return no error
        return CHIP_NO_ERROR;
    }    

    Report("\n\rReceived IP address successfully!\n\r");
    return CHIP_NO_ERROR;
}

void ConnectivityManagerImpl::_OnPlatformEvent(const ChipDeviceEvent * event)
{

    if (event->Type == DeviceLayer::DeviceEventType::kCommissioningComplete)
    {
        ChipLogProgress(AppServer, "Commissioning completed successfully");
    }
}

void ConnectivityManagerImpl::_OnWiFiScanDone()
{
    cc35xxLog("ConnectivityManagerImpl::_OnWiFiScanDone()\n\r");
}
void ConnectivityManagerImpl::_OnWiFiStationProvisionChange()
{
    cc35xxLog("ConnectivityManagerImpl::_OnWiFiStationProvisionChange()\n\r");
}

// ==================== ConnectivityManager Private Methods ====================

void ConnectivityManagerImpl::_OnIpAcquired()
{
    cc35xxLog("ConnectivityManagerImpl::OnIpAcquired() : Start DNS Server");
    ChipDeviceEvent event;
    event.Type                           = DeviceEventType::kInterfaceIpAddressChanged;
    event.InterfaceIpAddressChanged.Type = InterfaceIpChangeType::kIpV4_Assigned;
    PlatformMgr().PostEventOrDie(&event);
}

// C-callable entry point invoked from network_lwip.c status_callback when DHCP
// assigns an IP. Safe to call from any OS task context — PostEventOrDie only
// enqueues to the CHIP event queue.
extern "C" void cc35xx_on_ip_acquired(void)
{
    ConnectivityManagerImpl::_OnIpAcquired();
}

void ConnectivityManagerImpl::OnStationConnected()
{
    cc35xxLog("ConnectivityManagerImpl::OnStationConnected()\n\r");
}

CHIP_ERROR ConnectivityManagerImpl::ConfigureWiFiAP()
{
    cc35xxLog("ConnectivityManagerImpl::ConfigureWiFiAP()\n\r");
    return CHIP_NO_ERROR;
}

void ConnectivityManagerImpl::ChangeWiFiAPState(WiFiAPState newState)
{
    cc35xxLog("ConnectivityManagerImpl::ChangeWiFiAPState()\n\r");
}

void ConnectivityManagerImpl::UpdateInternetConnectivityState(void)
{
    cc35xxLog("ConnectivityManagerImpl::UpdateInternetConnectivityState()\n\r");
}

void ConnectivityManagerImpl::OnStationIPv4AddressAvailable()
{
    cc35xxLog("ConnectivityManagerImpl::OnStationIPv4AddressAvailable()\n\r");
}

void ConnectivityManagerImpl::OnStationIPv4AddressLost(void)
{
    cc35xxLog("ConnectivityManagerImpl::OnStationIPv4AddressLost()\n\r");
}

void ConnectivityManagerImpl::OnIPv6AddressAvailable()
{
    cc35xxLog("ConnectivityManagerImpl::OnIPv6AddressAvailable()\n\r");
}

void ConnectivityManagerImpl::RefreshMessageLayer(void)
{
    cc35xxLog("ConnectivityManagerImpl::RefreshMessageLayer()\n\r");
}

} // namespace DeviceLayer
} // namespace chip
