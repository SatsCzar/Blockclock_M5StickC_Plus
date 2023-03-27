#include "prefsManager.h"

#include <M5StickCPlus.h>
#include <Preferences.h>
#include <string>

Preferences preferences;

void wipeWiFiData() {
  preferences.begin("wifi");
  preferences.clear();
  preferences.end();
}

String getPrefsSsidPasswd(String ssidPasswd) {
  ssidPasswd.toUpperCase();
  preferences.begin("wifi");

  String value;

  if (ssidPasswd == "SSID") {
    value = preferences.getString("ssid", "none");
  }
  if (ssidPasswd == "PASS") {
    value = preferences.getString("password", "none");
  }

  preferences.end();

  return value;
}

void saveWiFiDataInStorage(String ssid, String password) {
  preferences.begin("wifi");
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.end();
}

boolean dontHaveWiFiDataInPrefs() {
  String ssid = getPrefsSsidPasswd("ssid");

  if (ssid != "none") {
    return false;
  }

  return true;
}