#include <M5StickCPlus.h>
#include <Preferences.h>

#include "WiFiManager.h"
#include "blockClockTypes.h"
#include "blockClockUtils.h"
#include "client.h"
#include "powerManager.h"
#include "screen.h"

ScreenState stateInScreen;
String blockHeightGlobal;
PriceData priceGlobal;

int globalBatteryLevel = -1;
const unsigned long CHECK_INTERVAL = 60000;
unsigned long lastMinuteCheck = 60001;

void setup() {
  M5.begin(true, true, true);
  M5.Rtc.begin();

  setCpuMaxPowerSave();

  M5.Lcd.setTextSize(2);
  M5.Lcd.setRotation(1);
  M5.Axp.ScreenBreath(9);

  M5.Lcd.setCursor(10, 10);
  M5.Lcd.println("BLOCKCLOCK");

  initWiFi();

  firstTimeInit();
  updateScreen();
}

void loop() {
  if (isIntervalElapsed()) {
    updateScreen();
  }

  if (!isCharging()) {
    callPrintbattery();
  }

  M5.update();

  if (checkButtonALongPressed()) {
    if (stateInScreen == PRICE) {
      priceGlobal.price = "";
      changeCurrencyState();
      callPriceScreen();
    }
  }

  if (checkButtonBPressed()) {
    changeScreenState();
    updateScreen();
  }

  delay(100);
}

void updateScreen() {
  switch (currentScreenState) {
    case BLOCKHEIGHT:
      callBlockHeightScreen();
      break;
    case PRICE:
      callPriceScreen();
      break;
    default:
      break;
  }

  if (isIntervalElapsed()) {
    lastMinuteCheck = millis();
  }
}

void firstTimeInit() {
  if (isWiFiConnected()) {
    blockHeightGlobal = getBlockHeight();
    clearScreen();
    drawBlockHeightScreen(blockHeightGlobal);
  }
}

void callBlockHeightScreen() {
  String blockheight;
  if (isWiFiConnected()) {
    if (isIntervalElapsed()) {
      blockheight = getBlockHeight();
    } else {
      blockheight = blockHeightGlobal;
    }
    if (blockheight != blockHeightGlobal || stateInScreen != BLOCKHEIGHT) {
      stateInScreen = BLOCKHEIGHT;
      blockHeightGlobal = blockheight;
      clearScreenExceptBattery();
      drawBlockHeightScreen(blockHeightGlobal);
    }
  }
}

void callPriceScreen() {
  if (isWiFiConnected()) {
    stateInScreen = PRICE;

    if (isIntervalElapsed() || priceGlobal.price == "") {
      priceGlobal = getBitcoinPrice(currentCurrencyState);
    }

    clearScreenExceptBattery();
    drawnPriceScreen(priceGlobal, currentCurrencyState);
  }
}

boolean checkButtonBPressed() {
  if (M5.BtnB.wasPressed()) {
    return true;
  }

  return false;
}

boolean checkButtonALongPressed() {
  if (M5.BtnA.wasReleasefor(700)) {
    return true;
  }

  return false;
}

boolean isIntervalElapsed() {
  unsigned long currentTime = millis();
  if (currentTime - lastMinuteCheck >= CHECK_INTERVAL) {
    return true;
  }

  return false;
}

void callPrintbattery() {
  int batteryLevel = getBatteryLevel();
  if (globalBatteryLevel == -1) {
    globalBatteryLevel = batteryLevel;
    clearBatteryScreen();
    printBattery(batteryLevel);
  }
  if (batteryLevel < globalBatteryLevel) {
    globalBatteryLevel = batteryLevel;
    clearBatteryScreen();
    printBattery(batteryLevel);
  }
}