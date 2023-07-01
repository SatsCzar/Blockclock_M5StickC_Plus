#include <M5StickCPlus.h>
#include <Preferences.h>

#include "WiFiManager.h"
#include "blockClockClient.h"
#include "blockClockTypes.h"
#include "blockClockUtils.h"
#include "powerManager.h"
#include "prefsManager.h"
#include "screen.h"
#include "timeManager.h"
#include "userBoardDefines.h"


ScreenState stateInScreen;
String blockHeightGlobal;
PriceData priceGlobal;
RecommendedFees recommendedFeesGlobal;
ApiClient apiClient("YOUR_API_KEY_HERE");
Screen screen;
WiFiManager wifiManager(&screen);

uint8_t globalMinute = 61;
int globalBatteryLevel = -1;
unsigned long lastMinuteCheck = 60001;

void setup() {
  M5.begin(true, true, true);
  screen.initScreen();

  setCpuNormalClock();

  M5.Axp.ScreenBreath(9);

  screen.drawStringPush("BLOCKCLOCK", 10, 10, 2);

  wifiManager.initWiFi();

  screen.drawStringPush("Configuring clock", 10, 70, 1);

  timeManagerbegin();

  screen.drawStringPush("Getting current block height", 10, 80, 1);

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
    buttonActionScreen();
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

void buttonActionScreen() {
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
    case WIFIDATA:
      callWiFiDataScreen(true);
    default:
      break;
  }
}

void updateScreen() {
  switch (currentScreenState) {
    case BLOCKHEIGHT:
      callBlockHeightScreen();
      break;
    case RECOMMENDED_FEES:
      callTransactionFeesScreen();
      break;
    case PRICE:
      callPriceScreen();
      break;
    case BTC_CHANGE:
      callChangeScreen();
      break;
    case WIFIDATA:
      callWiFiDataScreen(false);
      break;
    default:
      break;
  }

  if (isIntervalElapsed()) {
    lastMinuteCheck = millis();
  }
}

void firstTimeInit() {
  if (wifiManager.isWiFiConnected()) {
    blockHeightGlobal = apiClient.getBlockHeight();
    screen.drawBlockHeightScreen(blockHeightGlobal);
  }
}

void callBlockHeightScreen() {
  String blockheight;
  if (wifiManager.isWiFiConnected()) {
    if (isIntervalElapsed()) {
      blockheight = apiClient.getBlockHeight();
    } else {
      blockheight = blockHeightGlobal;
    }
    if (blockheight != blockHeightGlobal || stateInScreen != BLOCKHEIGHT) {
      stateInScreen = BLOCKHEIGHT;
      blockHeightGlobal = blockheight;
      screen.drawBlockHeightScreen(blockHeightGlobal);
    }
  }
}

void callTransactionFeesScreen() {
  RecommendedFees recommendedFees;

  if (wifiManager.isWiFiConnected()) {
    if (isIntervalElapsed() || recommendedFeesGlobal.high == 0) {
      recommendedFees = apiClient.getRecommendedFees();
    } else {
      recommendedFees = recommendedFeesGlobal;
    }
    if (recommendedFees.high != recommendedFeesGlobal.high ||
        stateInScreen != RECOMMENDED_FEES) {
      stateInScreen = RECOMMENDED_FEES;
      recommendedFeesGlobal = recommendedFees;
      screen.drawRecommendedFeesScreen(recommendedFees);
    }
  }
}

void callPriceScreen() {
  if (wifiManager.isWiFiConnected()) {
    stateInScreen = PRICE;

    PriceData pricePrefs = getBitcoinDataInPrefs(currentCurrencyState);

    if (pricePrefs.price != "" && pricePrefs.error == false) {
      time_t timestampFromRTC = getTimestampFromRTC();
      int64_t difference = timestampFromRTC - pricePrefs.timestamp;

      if (difference >= 300) {
        priceGlobal = apiClient.getBitcoinPrice(currentCurrencyState);

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
      priceGlobal = apiClient.getBitcoinPrice(currentCurrencyState);
      saveBitcoinDataInPrefs(priceGlobal);
    }

    screen.drawnPriceScreen(priceGlobal);
  }
}

void callChangeScreen() {
  if (wifiManager.isWiFiConnected()) {
    stateInScreen = BTC_CHANGE;

    PriceData pricePrefs = getBitcoinDataInPrefs(currentCurrencyState);

    if (pricePrefs.price != "" && pricePrefs.error == false) {
      time_t timestampFromRTC = getTimestampFromRTC();
      int64_t difference = timestampFromRTC - pricePrefs.timestamp;

      if (difference >= 300) {
        priceGlobal = apiClient.getBitcoinPrice(currentCurrencyState);
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
      priceGlobal = apiClient.getBitcoinPrice(currentCurrencyState);
      saveBitcoinDataInPrefs(priceGlobal);
    }

    screen.drawnChangeScreen(priceGlobal);
  }
}

void callDateTimeScreen() {
  BlockClockDateAndTime currentDateAndTime = getDateAndTime();

  String minutes = String(currentDateAndTime.minutes);
  String hours = String(currentDateAndTime.hour);

  String day;
  String month;

  if (currentDateAndTime.hour < 10) {
    hours = "0" + String(currentDateAndTime.hour);
  }

  if (currentDateAndTime.minutes < 10) {
    minutes = "0" + String(currentDateAndTime.minutes);
  }

  if (currentDateAndTime.day < 10) {
    day = "0" + String(currentDateAndTime.day);
  } else {
    day = String(currentDateAndTime.day);
  }

  if (currentDateAndTime.month < 10) {
    month = "0" + String(currentDateAndTime.month);
  } else {
    month = String(currentDateAndTime.month);
  }

  String ddmmyyyy = day + "/" + month + "/" + String(currentDateAndTime.year);

  if (stateInScreen == DATEANDTIME) {
    if (currentDateAndTime.minutes != globalMinute) {
      screen.drawnDateAndTimeScreen(hours, minutes, ddmmyyyy);
      globalMinute = currentDateAndTime.minutes;
    }

  } else {
    stateInScreen = DATEANDTIME;
    screen.drawnDateAndTimeScreen(hours, minutes, ddmmyyyy);
    globalMinute = currentDateAndTime.minutes;
  }
}

void callWiFiDataScreen(bool forceRender) {
  if (stateInScreen != WIFIDATA || forceRender == true) {
    stateInScreen = WIFIDATA;
    WiFiData wifiData = wifiManager.getWiFiData();

    screen.drawnWiFiDataScreen(wifiData);
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
    screen.clearBatteryScreen();
    screen.printBattery(batteryLevel);
  }
  if (batteryLevel < globalBatteryLevel) {
    globalBatteryLevel = batteryLevel;
    screen.clearBatteryScreen();
    screen.printBattery(batteryLevel);
  }
}