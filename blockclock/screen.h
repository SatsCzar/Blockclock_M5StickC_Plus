#ifndef SCREEN
#define SCREEN

#include <M5StickCPlus.h>

#include "blockClockTypes.h"
#include "blockClockUtils.h"

void initScreen();
void drawStringPush(String value, int32_t x, int32_t y, uint8_t size);
void drawString(String value, int32_t x, int32_t y, uint8_t size);
void clearScreen();
void drawBlockHeightScreen(String blockheight);
void drawRecommendedFeesScreen(RecommendedFees recommendedFees);
void drawnPriceScreen(PriceData priceData);
void drawnChangeScreen(PriceData priceData);
void drawnDateAndTimeScreen(String hours, String minutes, String ddmmyyyy);
void printChange(String time, float change, int16_t x, int16_t y);
void printFee(String text, uint16_t fee, int16_t x, int16_t y);
void drawnWiFiDataScreen(WiFiData wifiData);
void printBattery(int batteryLevel);
void clearBatteryScreen();
void clearScreenExceptBattery();
void setBitcoinTextColor(float delta);
void resetTextColor();
void clearHalfScreen();
void clearSpriteExceptBattery();

#endif