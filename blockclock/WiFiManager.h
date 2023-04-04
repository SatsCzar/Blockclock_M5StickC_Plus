#ifndef WIFIMANAGER
#define WIFIMANAGER

#include <M5StickCPlus.h>

#include "WiFiType.h"
#include "esp_wifi.h"
#include "blockClockTypes.h"

void initWiFi();
void initWiFiSmartConfig();
bool waitingWiFiConnection(wl_status_t status, int count);
bool connectionFailed(wl_status_t status);
String getSsidPasswd(String ssidPasswd);
bool isWiFiConnected();
WiFiData getWiFiData();

#endif