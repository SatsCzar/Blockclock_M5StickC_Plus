#include <HTTPClient.h>
#include <M5StickCPlus.h>
#include <Preferences.h>

#include <cmath>
#include <string>

#include "WiFiManager.h"
#include "powerManager.h"

HTTPClient http;

String blockHeightGlobal;
int batteryLevel;
const int ONE_MINUTE = 60000;
const String MEMPOOL_BASEURL = "https://mempool.space/api";


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
  http.begin(MEMPOOL_BASEURL + "/blocks/tip/height");
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

void clearScreen() { M5.Lcd.fillRect(0, 0, 240, 135, BLACK); }
