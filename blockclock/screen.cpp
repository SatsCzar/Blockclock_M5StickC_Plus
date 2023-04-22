#include "project_defines.h"

#ifdef M5STACK
#include <M5StickCPlus.h>
#endif
#ifdef GENERIC_ESP32
#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#endif

#include "blockClockTypes.h"
#include "blockClockUtils.h"
#include "screen.h"

TFT_eSPI *lcd;
TFT_eSprite *sprite;

void initScreen() {
#ifdef M5STACK
  lcd = &M5.Lcd;

  lcd->setTextSize(2);
  lcd->setRotation(1);

  static TFT_eSprite spr(&M5.Lcd);

  sprite = &spr;
  sprite->createSprite(240, 135);
#endif

#ifdef GENERIC_ESP32
  static TFT_eSPI tft = TFT_eSPI();
  lcd = &tft;
  lcd->init();
  lcd->setRotation(1);

  static TFT_eSprite spr(&tft);

  sprite = &spr;
  sprite->createSprite(240, 135);

  sprite->fillSprite(TFT_BLACK);
  sprite->setTextSize(2);
  sprite->pushSprite(0, 0);

#endif
}

void drawStringPush(String value, int32_t x, int32_t y, uint8_t size) {
  drawString(value, x, y, size);
  sprite->pushSprite(0, 0);
}

void drawString(String value, int32_t x, int32_t y, uint8_t size) {
  sprite->setTextSize(size);
  sprite->drawString(value, x, y, 1);
}

void drawBlockHeightScreen(String blockHeight) {
  clearSpriteExceptBattery();
  drawString("Block Height:", 5, 10, 3);
  drawString(blockHeight, 60, 80, 3);
  sprite->pushSprite(0, 0);
}

void drawRecommendedFeesScreen(RecommendedFees recommendedFees) {
  clearSpriteExceptBattery();
  drawString("Recommended Fees:", 5, 10, 2);

  if (recommendedFees.error == true) {
    drawString("Erro", 5, 50, 2);
  }

  printFee("High", recommendedFees.high, 5, 50);
  printFee("Medium", recommendedFees.medium, 5, 70);
  printFee("Low", recommendedFees.low, 5, 90);
  sprite->pushSprite(0, 0);
}

void drawnPriceScreen(PriceData priceData) {
  clearSpriteExceptBattery();
  String symbol = currencyStateToSymbol(priceData.currency);

  String textToPrint(symbol + " " + priceData.price);

  drawString("Bitcoin Price:", 40, 10, 2);
  setBitcoinTextColor(priceData.change24h);
  drawString(textToPrint, 29, 70, 3);

  resetTextColor();
  sprite->pushSprite(0, 0);
}

void drawnChangeScreen(PriceData priceData) {
  clearSpriteExceptBattery();
  String symbol = currencyStateToString(priceData.currency);
  String change1h = String(priceData.change1h);

  drawString("Change in " + symbol, 5, 10, 2);

  printChange("1h", priceData.change1h, 5, 40);
  printChange("24h", priceData.change24h, 5, 60);
  printChange("7d", priceData.change7d, 5, 80);
  printChange("30d", priceData.change30d, 5, 100);

  sprite->pushSprite(0, 0);
}

void printChange(String time, float change, int16_t x, int16_t y) {
  String changeString = String(change);

  drawString(time + ": ", x, y, 2);
  setBitcoinTextColor(change);
  drawString(changeString + "%", x + 60, y, 2);
  resetTextColor();
}

void printFee(String text, uint16_t fee, int16_t x, int16_t y) {
  String feeString = String(fee);

  drawString(text + ": ", x, y, 2);
  drawString(feeString + " sat/vB", x + 90, y, 2);
}

void drawnDateAndTimeScreen(String hours, String minutes, String ddmmyyyy) {
  clearSpriteExceptBattery();
  String hoursAndMinutes = hours + ":" + minutes;

  drawString(hoursAndMinutes, 30, 10, 6);
  drawString(ddmmyyyy, 30, 70, 3);
  sprite->pushSprite(0, 0);
}

void drawnWiFiDataScreen(WiFiData wifiData) {
  clearSpriteExceptBattery();
  String isConnected;

  if (wifiData.connected == true) {
    isConnected = "YES";
  } else {
    isConnected = "NO";
  }

  drawString("WiFi information", 5, 10, 1);
  drawString("Is connected: " + isConnected, 5, 20, 1);
  drawString("Connected to: " + truncateString(wifiData.SSID), 5, 30, 1);
  drawString("Signal strength: " + String(wifiData.SignalStrength), 5, 40, 1);
  sprite->pushSprite(0, 0);
}

void printBattery(int batteryLevel) {
  void clearBatterySprite();
  if (batteryLevel == 100) {
    drawString("100%", 185, 115, 2);
    return;
  }

  drawString(String(batteryLevel) + "%", 200, 115, 2);
  sprite->pushSprite(0, 0);
}

void clearBatteryScreen() { lcd->fillRect(185, 115, 240, 135, TFT_BLACK); }

void clearBatterySprite() { sprite->fillRect(185, 115, 240, 135, TFT_BLACK); }

void clearScreen() { lcd->fillRect(0, 0, 240, 135, TFT_BLACK); }

void setBitcoinTextColor(float delta) {
  if (delta > 0) {
    sprite->setTextColor(TFT_GREEN);
    return;
  }
  if (delta < 0) {
    sprite->setTextColor(TFT_RED);
    return;
  }
  sprite->setTextColor(TFT_WHITE);
}

void resetTextColor() { sprite->setTextColor(TFT_WHITE); }

void clearScreenExceptBattery() {
  lcd->fillRect(0, 0, 240, 112, TFT_BLACK);
  lcd->fillRect(0, 0, 183, 135, TFT_BLACK);
}

void clearSpriteExceptBattery() {
  sprite->fillRect(0, 0, 240, 112, TFT_BLACK);
  sprite->fillRect(0, 0, 183, 135, TFT_BLACK);
}

void clearHalfScreen() { sprite->fillRect(0, 60, 240, 135, TFT_BLACK); }