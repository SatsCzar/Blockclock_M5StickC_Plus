#include "WiFiManager.h"

#include <M5StickCPlus.h>

#include "WiFi.h"
#include "WiFiType.h"
#include "blockClockTypes.h"
#include "esp_smartconfig.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "powerManager.h"
#include "prefsManager.h"

const int WIFI_CONNECTION_TIMEOUT = 200;

void initWiFi() {
  M5.Lcd.setTextSize(1);

  WiFi.mode(WIFI_AP_STA);

  if (dontHaveWiFiDataInPrefs()) {
    initWiFiSmartConfig();
    delay(3000);
    ESP.restart();
  }

  String ssid = getPrefsSsidPasswd("ssid");
  String password = getPrefsSsidPasswd("pass");

  M5.Lcd.setCursor(10, 50);
  M5.Lcd.println("Connecting to: " + truncateString(ssid));
  M5.Lcd.setCursor(10, 60);
  M5.Lcd.println("Please Stand By");
  M5.Lcd.println("");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());

  int count = 0;

  while (waitingWiFiConnection(WiFi.status(), count)) {
    M5.Lcd.print(".");
    count++;
    delay(100);
  }

  M5.Lcd.fillRect(0, 60, 240, 135, BLACK);

  if (connectionFailed(WiFi.status())) {
    M5.Lcd.setCursor(10, 60);
    M5.Lcd.println("Failed to connect to: " + truncateString(ssid));
    M5.Lcd.setCursor(10, 70);
    M5.Lcd.println("Press main button to wipe WiFi data");
    M5.Lcd.setCursor(10, 80);
    M5.Lcd.println("Or press other button to restart");

    while (true) {
      M5.update();
      if (M5.BtnA.wasPressed()) {
        M5.Lcd.setCursor(10, 90);
        M5.Lcd.println("Wiping WiFi data and restarting");
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

  M5.Lcd.setCursor(10, 60);
  M5.Lcd.println("Successfuly connected to: " +
                 truncateString(ssid));

  setWiFiMaxPowerSave();
}

void initWiFiSmartConfig() {
  WiFi.beginSmartConfig(SC_TYPE_ESPTOUCH);

  M5.Lcd.setCursor(10, 30);
  M5.Lcd.println("Waiting for SmartConfig");

  while (!WiFi.smartConfigDone()) {
    delay(500);
  }

  M5.Lcd.setCursor(10, 40);
  M5.Lcd.println("Smartconfig received");
  M5.Lcd.setCursor(10, 50);
  M5.Lcd.println("Trying to connect");

  int count = 0;
  while (waitingWiFiConnection(WiFi.status(), count)) {
    count++;
    delay(100);
  }

  M5.Lcd.setCursor(10, 60);
  M5.Lcd.println("Connected to: " + WiFi.SSID());
  M5.Lcd.setCursor(10, 70);
  M5.Lcd.println("Saving WiFi data");

  String ssid = getSsidPasswd("SSID");
  String password = getSsidPasswd("PASS");

  saveWiFiDataInStorage(ssid, password);

  M5.Lcd.setCursor(10, 80);
  M5.Lcd.println("Restarting");

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
