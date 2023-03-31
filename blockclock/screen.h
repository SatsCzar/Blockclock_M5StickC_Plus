#ifndef SCREEN
#define SCREEN

#include <M5StickCPlus.h>
#include "blockClockTypes.h"

void drawBlockHeightScreen(String blockheight);
void drawnPriceScreen(PriceData priceData, CurrencyState currencyState);
void printBattery(int batteryLevel);
void clearBatteryScreen();
void clearScreen();
void clearScreenExceptBattery();

#endif