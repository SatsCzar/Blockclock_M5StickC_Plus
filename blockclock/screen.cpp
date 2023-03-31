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

void drawnPriceScreen(PriceData priceData, CurrencyState currencyState) {
  String symbol = currencyStateToSymbol(currencyState);

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

void printBattery(int batteryLevel) {
  M5.Lcd.setTextSize(2);
  if (batteryLevel == 100) {
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

void clearBatteryScreen() { M5.Lcd.fillRect(200, 115, 240, 135, BLACK); }

void clearScreen() { M5.Lcd.fillRect(0, 0, 240, 135, BLACK); }

void clearScreenExceptBattery() {
  M5.Lcd.fillRect(0, 0, 240, 112, BLACK);
  M5.Lcd.fillRect(0, 0, 183, 135, BLACK);
}