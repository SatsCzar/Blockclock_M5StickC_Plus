#ifndef WIFIMANAGER
#define WIFIMANAGER

#include <M5StickCPlus.h>

#include "WiFiType.h"
#include "esp_wifi.h"

void initWiFi();
void initWiFiSmartConfig();
boolean waitingWiFiConnection(wl_status_t status, int count);
boolean connectionFailed(wl_status_t status);
String getSsidPasswd(String ssidPasswd);
boolean isWiFiConnected();

#endif