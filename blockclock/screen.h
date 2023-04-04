#ifndef SCREEN
#define SCREEN

#include <M5StickCPlus.h>

#include "blockClockTypes.h"
#include "blockClockUtils.h"

void drawBlockHeightScreen(String blockheight);
void drawnPriceScreen(PriceData priceData);
void drawnChangeScreen(PriceData priceData);
void drawnDateAndTimeScreen(String hours, String minutes, String ddmmyyyy);
void printChange(String time, float change, int16_t x, int16_t y);
void drawnWiFiDataScreen(WiFiData wifiData);
void printBattery(int batteryLevel);
void clearBatteryScreen();
void clearScreen();
void clearScreenExceptBattery();
void setBitcoinTextColor(float delta);
void resetTextColor();

#endif