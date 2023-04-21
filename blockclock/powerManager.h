#ifndef POWERMANAGER
#define POWERMANAGER

#include <M5StickCPlus.h>

void setWiFiMaxPowerSave();
void setCpuMaxPowerSave();
int getBatteryLevel();
int calculateBatteryPercentage(float voltage);
bool isCharging();

#endif