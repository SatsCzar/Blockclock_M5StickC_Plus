#include <M5StickCPlus.h>
#include <Preferences.h>

#include "WiFiManager.h"
#include "blockClockTypes.h"
#include "blockClockUtils.h"
#include "client.h"
#include "powerManager.h"
#include "prefsManager.h"
#include "screen.h"
#include "timeManager.h"

ScreenState stateInScreen;
String blockHeightGlobal;
PriceData priceGlobal;

uint8_t globalMinute = 61;
int globalBatteryLevel = -1;
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

  M5.Lcd.setCursor(10, 70);
  M5.Lcd.println("Configuring clock");
  timeManagerbegin();

  M5.Lcd.setCursor(10, 80);
  M5.Lcd.println("Getting current block height");
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

  if (checkButtonAPressed()) {
    switch (stateInScreen) {
      case PRICE:
        priceGlobal.price = "";
        changeCurrencyState();
        callPriceScreen();
        break;
      case BTC_CHANGE:
        priceGlobal.price = "";
        changeCurrencyState();
        callChangeScreen();
        break;
      default:
        break;
    }
  }

  if (checkButtonBPressed()) {
    changeScreenState();
    updateScreen();
  }

  if (currentScreenState == DATEANDTIME) {
    callDateTimeScreen();
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
    case BTC_CHANGE:
      callChangeScreen();
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

    PriceData pricePrefs = getBitcoinDataInPrefs(currentCurrencyState);

    if (pricePrefs.price != "" && pricePrefs.error == false) {
      time_t timestampFromRTC = getTimestampFromRTC();
      int64_t difference = timestampFromRTC - pricePrefs.timestamp;

      if (difference >= 300) {
        priceGlobal = getBitcoinPrice(currentCurrencyState);

        if (priceGlobal.price != "") {
          saveBitcoinDataInPrefs(priceGlobal);
        }
      } else {
        priceGlobal.price = pricePrefs.price;
        priceGlobal.change1h = pricePrefs.change1h;
        priceGlobal.change24h = pricePrefs.change24h;
        priceGlobal.change30d = pricePrefs.change30d;
        priceGlobal.change7d = pricePrefs.change7d;
        priceGlobal.currency = pricePrefs.currency;
      }

      lastMinuteCheck = millis();
    }

    if (isIntervalElapsed() || priceGlobal.price == "" ||
        priceGlobal.error == true) {
      priceGlobal = getBitcoinPrice(currentCurrencyState);
      saveBitcoinDataInPrefs(priceGlobal);
    }

    clearScreenExceptBattery();
    drawnPriceScreen(priceGlobal);
  }
}

void callChangeScreen() {
  if (isWiFiConnected()) {
    stateInScreen = BTC_CHANGE;

    PriceData pricePrefs = getBitcoinDataInPrefs(currentCurrencyState);

    if (pricePrefs.price != "" && pricePrefs.error == false) {
      time_t timestampFromRTC = getTimestampFromRTC();
      int64_t difference = timestampFromRTC - pricePrefs.timestamp;

      if (difference >= 300) {
        priceGlobal = getBitcoinPrice(currentCurrencyState);
        if (priceGlobal.price != "") {
          saveBitcoinDataInPrefs(priceGlobal);
        }
      } else {
        priceGlobal = pricePrefs;
      }

      lastMinuteCheck = millis();
    }

    if (isIntervalElapsed() || priceGlobal.price == "" ||
        priceGlobal.error == true) {
      priceGlobal = getBitcoinPrice(currentCurrencyState);
      saveBitcoinDataInPrefs(priceGlobal);
    }

    clearScreenExceptBattery();
    drawnChangeScreen(priceGlobal);
  }
}

void callDateTimeScreen() {
  RTC_TimeTypeDef timeNow = getTime();
  RTC_DateTypeDef dateNow = getDate();

  String minutes;
  String hours = String(timeNow.Hours);

  String day;
  String month;

  if (timeNow.Minutes < 10) {
    minutes = "0" + String(timeNow.Minutes);
  }

  if (timeNow.Minutes >= 10) {
    minutes = timeNow.Minutes;
  }

  if (dateNow.Date < 10) {
    day = "0" + String(dateNow.Date);
  } else {
    day = String(dateNow.Date);
  }

  if (dateNow.Month < 10) {
    month = "0" + String(dateNow.Month);
  } else {
    month = String(dateNow.Month);
  }

  String ddmmyyyy = day + "/" + month + "/" + String(dateNow.Year);

  if (stateInScreen == DATEANDTIME) {
    if (timeNow.Minutes != globalMinute) {
      clearScreenExceptBattery();
      drawnDateAndTimeScreen(hours, minutes, ddmmyyyy);
      globalMinute = timeNow.Minutes;
    }

  } else {
    stateInScreen = DATEANDTIME;
    clearScreenExceptBattery();
    drawnDateAndTimeScreen(hours, minutes, ddmmyyyy);
    globalMinute = timeNow.Minutes;
  }
}

bool checkButtonBPressed() {
  if (M5.BtnB.wasPressed()) {
    return true;
  }

  return false;
}

bool checkButtonAPressed() {
  if (M5.BtnA.isPressed()) {
    return true;
  }

  return false;
}

bool isIntervalElapsed() {
  const unsigned long BLOCK_HEIGHT_CHECK_INTERVAL = 60000;
  const unsigned long PRICE_CHANGE_CHECK_INTERVAL = 300000;
  unsigned long checkInterval;

  if (currentScreenState == BLOCKHEIGHT) {
    checkInterval = BLOCK_HEIGHT_CHECK_INTERVAL;
  } else {
    checkInterval = PRICE_CHANGE_CHECK_INTERVAL;
  }

  unsigned long currentTime = millis();
  if (currentTime - lastMinuteCheck >= checkInterval) {
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