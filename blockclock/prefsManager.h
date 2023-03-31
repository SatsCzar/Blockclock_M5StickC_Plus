#ifndef PREFSMANAGER
#define PREFSMANAGER


#include <M5StickCPlus.h>

void wipeWiFiData();
String getPrefsSsidPasswd(String ssidPasswd);
void saveWiFiDataInStorage(String ssid, String password);
boolean dontHaveWiFiDataInPrefs();

#endif