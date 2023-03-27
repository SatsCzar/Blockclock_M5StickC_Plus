#ifndef WIFIMANAGER
#define WIFIMANAGER

#include <string>

#include <M5StickCPlus.h>
#include "esp_wifi.h"
#include "WiFiType.h"

void initWiFi();
void initWiFiSmartConfig();
boolean waitingWiFiConnection(wl_status_t status, int count);
boolean connectionFailed(wl_status_t status);
String getSsidPasswd(String ssidPasswd);

#endif