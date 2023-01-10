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
const char* SSID = "WIFISSID";
const char* PASSWD = "WIFIPASSWORD";
const float BATTERY_MIN_VOLTAGE = 3.7;
const float BATTERY_MAX_VOLTAGE = 4.1;

void setup() {
  setCpuFrequencyMhz(80);  // Lower processor clock to save power
  M5.begin(true, true, false);

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

  if (!haveDataInPrefs()) {
    /* Dont have wifi saved
        Init smarttconfig mode */
    initWifiSmartConfig();
    delay(3000);
    ESP.restart();
  }

  String ssid = getPrefsSsidPasswd("ssid");
  String password = getPrefsSsidPasswd("pass");

  M5.Lcd.setCursor(10, 50);
  M5.Lcd.println("Connecting to: " + ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());

  while (waitingWiFiConnection(WiFi.status())) {
    delay(500);
  }

  if (WiFi.status() == WL_CONNECT_FAILED) {
    M5.Lcd.setCursor(10, 60);
    M5.Lcd.println("Failed to connect to: " + ssid);
  }

  esp_wifi_set_ps(WIFI_PS_MAX_MODEM);  // Set max power save
}

void initWifiSmartConfig() {
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

  while (waitingWiFiConnection(WiFi.status())) {
    delay(500);
  }

  M5.Lcd.setCursor(10, 60);
  M5.Lcd.println("Connected to: " + WiFi.SSID());
  M5.Lcd.setCursor(10, 70);
  M5.Lcd.println("Saving WiFi data");

  String ssid = getSsidPasswd("SSID");
  String password = getSsidPasswd("PASS");

  saveWifiDataInStorage(ssid, password);

  M5.Lcd.setCursor(10, 80);
  M5.Lcd.println("Restarting");

  delay(500);
}

boolean waitingWiFiConnection(wl_status_t status) {
  if (status != WL_CONNECTED && status != WL_CONNECT_FAILED) {
    return true;
  }

  return false;
}

void saveWifiDataInStorage(String ssid, String password) {
  preferences.begin("wifi");
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.end();
}

boolean haveDataInPrefs() {
  String ssid = getSsidPasswd("ssid");

  if(ssid != "none") {
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
  delay(60000);  // 1 minute
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
  M5.Lcd.print(String(batteryLevel) + "%");
}

int calculateBatteryPercentage(float voltage) {
  // https://forum.micropython.org/viewtopic.php?f=2&t=7615#p43401
  return (int)trunc((voltage - BATTERY_MIN_VOLTAGE) * (100 - 0) /
                        (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE) +
                    0);
}

void clearScreen() { M5.Lcd.fillRect(0, 0, 240, 135, BLACK); }
