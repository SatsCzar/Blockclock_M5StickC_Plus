#include <HTTPClient.h>
#include <M5StickCPlus.h>

#include <cmath>
#include <string>

#include "esp32-hal-cpu.h"
#include "esp_wifi.h"

HTTPClient http;

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
  M5.Lcd.setCursor(10, 30);
  M5.Lcd.println("Connecting Wifi");

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWD);

  while (millis() < 3000 && WiFi.status() != WL_CONNECTED) {
    // Await fot WiFi connect
  }

  esp_wifi_set_ps(WIFI_PS_MAX_MODEM);  // Set max power save

  M5.Lcd.setCursor(10, 50);
  if (WiFi.status() == WL_CONNECTED) {
    M5.Lcd.println("Wifi connected");
    M5.Lcd.setCursor(10, 70);
    delay(500);
  } else {
    M5.Lcd.println("Connection failed");
  }
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
  return (int)trunc((voltage - BATTERY_MIN_VOLTAGE) * (100 - 0) / (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE) + 0);
}

void clearScreen() { M5.Lcd.fillRect(0, 0, 240, 135, BLACK); }
                                                                             