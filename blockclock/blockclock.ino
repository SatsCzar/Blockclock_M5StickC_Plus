#include <M5StickCPlus.h>
#include <Preferences.h>

#include <string>

#include "WiFiManager.h"
#include "client.h"
#include "powerManager.h"
#include "screen.h"

enum ScreenState {
  BLOCKHEIGHT,
  PRICE,
};

ScreenState currentScreenState = BLOCKHEIGHT;
String blockHeightGlobal;
const int ONE_MINUTE = 60000;
const int NUM_SCREEN_STATES = 2;

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
  if (getWiFiStatus() == WL_CONNECTED) {
    String blockheight = getBlockHeight();
    if (blockheight != blockHeightGlobal) {
      blockHeightGlobal = blockheight;
      clearScreen();
      drawBlockHeightScreen(blockHeightGlobal);
    }
  }
  if (!isCharging()) {
    printBattery(getBatteryLevel());
  }
  delay(ONE_MINUTE);
}
