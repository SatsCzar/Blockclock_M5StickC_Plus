#include <HTTPClient.h>
#include <M5StickCPlus.h>
#include <Preferences.h>

#include <cmath>
#include <string>

#include "esp32-hal-cpu.h"
#include "esp_system.h"
#include "esp_wifi.h"

HTTPClient http;
Preferences preferences;

String blockHeightGlobal;
int batteryLevel;
const float BATTERY_MIN_VOLTAGE = 3.06;
const float BATTERY_MAX_VOLTAGE = 4.1;
const int ONE_MINUTE = 60000;
const int WIFI_CONNECTION_TIMEOUT = 200;

void setup() {
  M5.begin(true, true, false);
  setCpuMaxPowerSave();

  M5.Lcd.setTextSize(2);
  M5.Lcd.setRotation(1);
  M5.Axp.ScreenBreath(9);

  M5.Lcd.setCursor(10, 10);
  M5.Lcd.println("BLOCKCLOCK");

  initWiFi();
}

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
  M5.Lcd.println("Connecting to: " + ssid);
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
    M5.Lcd.println("Failed to connect to: " + ssid);
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

boolean waitingWiFiConnection(wl_status_t status, int count) {
  if ((status != WL_CONNECTED && status != WL_CONNECT_FAILED &&
       status != WL_NO_SSID_AVAIL) &&
      count <= WIFI_CONNECTION_TIMEOUT) {
    return true;
  }

  return false;
}

boolean connectionFailed(wl_status_t status) {
  if (status == WL_NO_SSID_AVAIL || status == WL_IDLE_STATUS ||
      status == WL_CONNECT_FAILED || status == WL_DISCONNECTED) {
    return true;
  }

  return false;
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

void setWiFiMaxPowerSave() { esp_wifi_set_ps(WIFI_PS_MAX_MODEM); }

void setCpuMaxPowerSave() { setCpuFrequencyMhz(80); }

void loop() {
  String blockheight;

  if (WiFi.status() == WL_CONNECTED) {
    blockheight = getBlockHeight();
    if (blockheight != blockHeightGlobal) {
      blockHeightGlobal = blockheight;
      clearScreen();
      printInfo();
    }
  }
  if (!isCharging()) {
    printBattery();
  }
  delay(ONE_MINUTE);
}

String getBlockHeight() {
  http.begin("https://mempool.space/api/blocks/tip/height");
  int httpCode = http.GET();
  if (httpCode == 200) {
    return http.getString();
  }
  return "ERR " + httpCode;
}

void printInfo() {
  M5.Lcd.setCursor(5, 10);
  M5.Lcd.setTextSize(3);
  M5.Lcd.print("Block Height:");
  M5.Lcd.setCursor(60, 80);
  M5.Lcd.print(blockHeightGlobal);
}

bool isCharging() {
  float batteryCurrent = M5.Axp.GetBatCurrent();
  return batteryCurrent >= 0;
}

void printBattery() {
  M5.Lcd.setTextSize(2);
  batteryLevel = calculateBatteryPercentage(M5.Axp.GetBatVoltage());
  if (batteryLevel >= 100) {
    M5.Lcd.setCursor(185, 115);
    M5.Lcd.print("100%");
    return;
  }
  M5.Lcd.setCursor(200, 115);
  if (batteryLevel <= 0) {
    M5.Lcd.print("0%");
    return;
  }
  M5.Lcd.print(String(batteryLevel) + "%");
}

int calculateBatteryPercentage(float voltage) {
  // https://forum.micropython.org/viewtopic.php?f=2&t=7615#p43401
  return (int)trunc((voltage - BATTERY_MIN_VOLTAGE) * (100 - 0) /
                        (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE) +
                    0);
}

void clearScreen() { M5.Lcd.fillRect(0, 0, 240, 135, BLACK); }
