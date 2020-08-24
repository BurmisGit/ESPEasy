#ifdef HAS_ETHERNET

#include "ESPEasyEth.h"


#include "ESPEasy-Globals.h"
#include "ESPEasyNetwork.h"
#include "ETH.h"
#include "eth_phy/phy.h"
#include "src/Globals/NetworkState.h"
#include "src/Helpers/StringConverter.h"

bool ethUseStaticIP() {
  return Settings.ETH_IP[0] != 0 && Settings.ETH_IP[0] != 255;
}

void ethSetupStaticIPconfig() {
  if (!ethUseStaticIP()) { return; }
  const IPAddress ip     = Settings.ETH_IP;
  const IPAddress gw     = Settings.ETH_Gateway;
  const IPAddress subnet = Settings.ETH_Subnet;
  const IPAddress dns    = Settings.ETH_DNS;

  if (loglevelActiveFor(LOG_LEVEL_INFO)) {
    String log = F("ETH IP   : Static IP : ");
    log += formatIP(ip);
    log += F(" GW: ");
    log += formatIP(gw);
    log += F(" SN: ");
    log += formatIP(subnet);
    log += F(" DNS: ");
    log += formatIP(dns);
    addLog(LOG_LEVEL_INFO, log);
  }
  ETH.config(ip, gw, subnet, dns);
}

bool ethCheckSettings() {
  bool result = true;
  if (Settings.ETH_Phy_Type != 0 && Settings.ETH_Phy_Type != 1)
    result = false;
  if (!isValid(Settings.ETH_Clock_Mode))
    result = false;
  if (!isValid(Settings.ETH_Wifi_Mode))
    result = false;
  if (Settings.ETH_Pin_mdc > MAX_GPIO)
    result = false;
  if (Settings.ETH_Pin_mdio > MAX_GPIO)
    result = false;
  if (Settings.ETH_Pin_power > MAX_GPIO)
    result = false;
  return result;
}

bool ethPrepare() {
  if (!ethCheckSettings())
  {
    addLog(LOG_LEVEL_ERROR, F("ETH: Settings not correct!!!"));
    return false;
  }
  char hostname[40];
  safe_strncpy(hostname, NetworkCreateRFCCompliantHostname().c_str(), sizeof(hostname));
  ETH.setHostname(hostname);
  ETH.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
  ethSetupStaticIPconfig();
  return true;
}

void ethPrintSettings() {
  String settingsDebugLog;
  settingsDebugLog.reserve(115);
  settingsDebugLog += F("Eth Wifi mode: ");
  settingsDebugLog += toString(eth_wifi_mode);
  settingsDebugLog += F(" ETH: PHY Type: ");
  settingsDebugLog += Settings.ETH_Phy_Type == 0 ? F("ETH_PHY_LAN8720") : F("ETH_PHY_TLK110");
  settingsDebugLog += F(" PHY Addr: ");
  settingsDebugLog += Settings.ETH_Phy_Addr;
  settingsDebugLog += F(" Eth Clock mode: ");
  settingsDebugLog += toString(Settings.ETH_Clock_Mode);
  settingsDebugLog += F(" MDC Pin: ");
  settingsDebugLog += String(Settings.ETH_Pin_mdc);
  settingsDebugLog += F(" MIO Pin: ");
  settingsDebugLog += String(Settings.ETH_Pin_mdio);
  settingsDebugLog += F(" Power Pin: ");
  settingsDebugLog += String(Settings.ETH_Pin_power);
  addLog(LOG_LEVEL_INFO, settingsDebugLog);
}

uint8_t * ETHMacAddress(uint8_t* mac) {
    esp_eth_get_mac(mac);
    return mac;
}

void ETHConnectRelaxed() {
  ethPrintSettings();
  ETH.begin(Settings.ETH_Phy_Addr,
            Settings.ETH_Pin_power,
            Settings.ETH_Pin_mdc,
            Settings.ETH_Pin_mdio,
            (eth_phy_type_t)Settings.ETH_Phy_Type,
            (eth_clock_mode_t)Settings.ETH_Clock_Mode);
  addLog(LOG_LEVEL_INFO, F("After ETH.begin"));
  if (!ethPrepare()) {
    // Dead code for now...
    addLog(LOG_LEVEL_ERROR, F("ETH : Could not prepare ETH!"));
    return;
  }
}

bool ETHConnected() {
  return eth_connected;
}

#endif