#include "screen.h"

#include <M5StickCPlus.h>

#include "blockClockTypes.h"
#include "blockClockUtils.h"

void drawBlockHeightScreen(String blockHeight) {
  M5.Lcd.setCursor(5, 10);
  M5.Lcd.setTextSize(3);
  M5.Lcd.print("Block Height:");
  M5.Lcd.setCursor(60, 80);
  M5.Lcd.print(blockHeight);
}

void drawnPriceScreen(PriceData priceData) {
  String symbol = currencyStateToSymbol(priceData.currency);

  M5.Lcd.setCursor(40, 10);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("Bitcoin Price:");
  M5.Lcd.setCursor(29, 70);
  M5.Lcd.setTextSize(3);
  if (priceData.change24h > 0) {
    M5.Lcd.setTextColor(GREEN);
  } else {
    M5.Lcd.setTextColor(RED);
  }
  M5.Lcd.print(symbol + " " + priceData.price);
  M5.Lcd.setTextColor(WHITE);
}

void drawnChangeScreen(PriceData priceData) {
  String symbol = currencyStateToString(priceData.currency);
  String change1h = String(priceData.change1h);

  M5.Lcd.setTextSize(2);

  M5.Lcd.setCursor(5, 10);
  M5.Lcd.print("Change in " + symbol);

  printChange("1h", priceData.change1h, 5, 40);
  printChange("24h", priceData.change24h, 5, 60);
  printChange("7d", priceData.change7d, 5, 80);
  printChange("30d", priceData.change30d, 5, 100);

  resetTextColor();
}

void printChange(String time, float change, int16_t x, int16_t y) {
  String changeString = String(change);

  M5.Lcd.setCursor(x, y);
  resetTextColor();

  M5.Lcd.print(time + ": ");
  setBitcoinTextColor(change);
  M5.Lcd.print(changeString + "%");
}

void printBattery(int batteryLevel) {
  M5.Lcd.setTextSize(2);
  if (batteryLevel == 100) {
    M5.Lcd.setCursor(185, 115);
    M5.Lcd.print("100%");
    return;
  }
  M5.Lcd.setCursor(200, 115);
  M5.Lcd.print(String(batteryLevel) + "%");
}

void clearBatteryScreen() { M5.Lcd.fillRect(185, 115, 240, 135, BLACK); }

void clearScreen() { M5.Lcd.fillRect(0, 0, 240, 135, BLACK); }

void setBitcoinTextColor(float delta) {
  if (delta > 0) {
    M5.Lcd.setTextColor(GREEN);
    return;
  }
  if (delta < 0) {
    M5.Lcd.setTextColor(RED);
    return;
  }
  M5.Lcd.setTextColor(WHITE);
}

void resetTextColor() { M5.Lcd.setTextColor(WHITE); }

void clearScreenExceptBattery() {
  M5.Lcd.fillRect(0, 0, 240, 112, BLACK);
  M5.Lcd.fillRect(0, 0, 183, 135, BLACK);
}