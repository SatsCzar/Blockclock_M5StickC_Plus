#include <M5StickCPlus.h>

#include <string>

void drawBlockHeightScreen(String blockHeight) {
  M5.Lcd.setCursor(5, 10);
  M5.Lcd.setTextSize(3);
  M5.Lcd.print("Block Height:");
  M5.Lcd.setCursor(60, 80);
  M5.Lcd.print(blockHeight);
}

void printBattery(int batteryLevel) {
  M5.Lcd.setTextSize(2);
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