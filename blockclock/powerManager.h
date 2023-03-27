#ifndef POWERMANAGER
#define POWERMANAGER

#include <M5StickCPlus.h>

void setWiFiMaxPowerSave();
void setCpuMaxPowerSave();
int calculateBatteryPercentage(float voltage);
bool isCharging();

#endif