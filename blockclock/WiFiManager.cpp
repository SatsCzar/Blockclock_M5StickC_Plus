#include "WiFiManager.h"

#include "userBoardDefines.h"

#ifdef M5STACK
#include <M5StickCPlus.h>
#endif
#ifdef GENERIC_ESP32
#include <Arduino.h>
#endif

#include "WiFi.h"
#include "WiFiType.h"
#include "blockClockTypes.h"
#include "esp_smartconfig.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "powerManager.h"
#include "prefsManager.h"
#include "screen.h"

const int WIFI_CONNECTION_TIMEOUT = 200;

void initWiFi() {
  WiFi.mode(WIFI_AP_STA);

  if (dontHaveWiFiDataInPrefs()) {
    initWiFiSmartConfig();
    delay(3000);
    ESP.restart();
  }

  String ssid = getPrefsSsidPasswd("ssid");
  String password = getPrefsSsidPasswd("pass");

  drawStringPush("Connecting to: " + truncateString(ssid), 10, 50, 1);
  drawStringPush("Please Stand By", 10, 60, 1);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());

  int count = 0;

  while (waitingWiFiConnection(WiFi.status(), count)) {
    count++;
    delay(100);
  }

  clearHalfScreen();

  if (connectionFailed(WiFi.status())) {
    drawStringPush("Failed to connect to: " + truncateString(ssid), 10, 60, 1);
    drawStringPush("Press main button to wipe WiFi data", 10, 70, 1);
    drawStringPush("Or press other button to restart", 10, 80, 1);

    while (true) {
      M5.update();
      if (M5.BtnA.wasPressed()) {
        drawStringPush("Wiping WiFi data and restarting", 10, 90, 1);
        wipeWiFiData();
        delay(4000);
        ESP.restart();
      }

      if (M5.BtnB.wasPressed() || M5.Axp.GetBtnPress()) {
        ESP.restart();
      }
      delay(1000);
    }
  }

  drawStringPush("Successfuly connected to: " + truncateString(ssid), 10, 60,
                 1);

  setWiFiMaxPowerSave();
}

void initWiFiSmartConfig() {
  WiFi.beginSmartConfig(SC_TYPE_ESPTOUCH);

  drawStringPush("Waiting for SmartConfig", 10, 30, 1);

  while (!WiFi.smartConfigDone()) {
    delay(500);
  }

  drawStringPush("Smartconfig received", 10, 40, 1);
  drawStringPush("Trying to connect", 10, 50, 1);

  int count = 0;
  while (waitingWiFiConnection(WiFi.status(), count)) {
    count++;
    delay(100);
  }

  drawStringPush("Connected to: " + WiFi.SSID(), 10, 60, 1);
  drawStringPush("Saving WiFi data", 10, 70, 1);

  String ssid = getSsidPasswd("SSID");
  String password = getSsidPasswd("PASS");

  saveWiFiDataInStorage(ssid, password);

  drawStringPush("Restarting", 10, 80, 1);

  delay(500);
}

bool waitingWiFiConnection(wl_status_t status, int count) {
  if ((status != WL_CONNECTED && status != WL_CONNECT_FAILED &&
       status != WL_NO_SSID_AVAIL) &&
      count <= WIFI_CONNECTION_TIMEOUT) {
    return true;
  }

  return false;
}

bool connectionFailed(wl_status_t status) {
  if (status == WL_NO_SSID_AVAIL || status == WL_IDLE_STATUS ||
      status == WL_CONNECT_FAILED || status == WL_DISCONNECTED) {
    return true;
  }

  return false;
}

String getSsidPasswd(String ssidPasswd) {
  ssidPasswd.toUpperCase();

  wifi_config_t conf;
  esp_wifi_get_config(WIFI_IF_STA, &conf);

  if (ssidPasswd == "SSID") {
    return String(reinterpret_cast<const char*>(conf.sta.ssid));
  }
  if (ssidPasswd == "PASS") {
    return String(reinterpret_cast<const char*>(conf.sta.password));
  }
}

bool isWiFiConnected() { return WiFi.status() == WL_CONNECTED; }

WiFiData getWiFiData() {
  WiFiData wifiData;
  wifi_ap_record_t wifiApInformation;

  esp_err_t err = esp_wifi_sta_get_ap_info(&wifiApInformation);

  Serial.println(esp_err_to_name(err));

  if (err != ESP_OK) {
    Serial.println(esp_err_to_name(err));
  }

  Serial.println("1");
  wifiData.connected = isWiFiConnected();
  Serial.println("2");
  wifiData.SignalStrength = wifiApInformation.rssi;
  Serial.println("3");
  wifiData.SSID = String(reinterpret_cast<char*>(wifiApInformation.ssid));

  return wifiData;
}
