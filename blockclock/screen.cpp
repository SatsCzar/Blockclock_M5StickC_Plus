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

void drawRecommendedFeesScreen(RecommendedFees recommendedFees) {
  M5.Lcd.setCursor(5, 10);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("Recommended Fees:");

  if (recommendedFees.error == true) {
    M5.Lcd.setCursor(5, 50);
    M5.Lcd.print("Erro");
  }

  printFee("High", recommendedFees.high, 5, 50);
  printFee("Medium", recommendedFees.medium, 5, 70);
  printFee("Low", recommendedFees.low, 5, 90);
  printFee("No priority", recommendedFees.noPriority, 5, 110);
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

void printFee(String text, uint16_t fee, int16_t x, int16_t y) {
  String feeString = String(fee);

  M5.Lcd.setCursor(x, y);

  M5.Lcd.print(text + ": ");
  M5.Lcd.print(feeString + "");
}

void drawnDateAndTimeScreen(String hours, String minutes, String ddmmyyyy) {
  M5.Lcd.setTextSize(6);

  M5.Lcd.setCursor(30, 10);
  M5.Lcd.print(hours + ":" + minutes);
  M5.Lcd.setCursor(30, 70);

  M5.Lcd.setTextSize(3);
  M5.Lcd.print(ddmmyyyy);
}

void drawnWiFiDataScreen(WiFiData wifiData) {
  M5.Lcd.setTextSize(2);

  String isConnected;

  if (wifiData.connected == true) {
    isConnected = "YES";
  } else {
    isConnected = "NO";
  }

  M5.Lcd.setTextSize(1);

  M5.Lcd.setCursor(5, 10);
  M5.Lcd.print("WiFi information");
  M5.Lcd.setCursor(5, 20);
  M5.Lcd.print("Is connected: " + isConnected);
  M5.Lcd.setCursor(5, 30);
  M5.Lcd.print("Connected to: " + wifiData.SSID);
  M5.Lcd.setCursor(5, 40);
  M5.Lcd.print("Signal strength: " + String(wifiData.SignalStrength));

  resetTextColor();
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