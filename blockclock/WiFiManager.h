#ifndef WIFIMANAGER
#define WIFIMANAGER

#include "userBoardDefines.h"
#ifdef M5STACK
#include <M5StickCPlus.h>
#endif
#ifdef GENERIC_ESP32
#include <Arduino.h>
#endif

#include "WiFiType.h"
#include "blockClockTypes.h"
#include "esp_wifi.h"

void initWiFi();
void initWiFiSmartConfig();
bool waitingWiFiConnection(wl_status_t status, int count);
bool connectionFailed(wl_status_t status);
String getSsidPasswd(String ssidPasswd);
bool isWiFiConnected();
WiFiData getWiFiData();

#endif