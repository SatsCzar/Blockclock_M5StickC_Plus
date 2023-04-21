#include "powerManager.h"

#include <M5StickCPlus.h>

#include <cmath>

#include "WiFiManager.h"
#include "esp32-hal-cpu.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"

const float BATTERY_MIN_VOLTAGE = 3.06;
const float BATTERY_MAX_VOLTAGE = 4.1;

void setWiFiMaxPowerSave() { esp_wifi_set_ps(WIFI_PS_MAX_MODEM); }

void setCpuMaxPowerSave() { setCpuFrequencyMhz(80); }

bool isCharging() {
  float batteryCurrent = M5.Axp.GetBatCurrent();
  return batteryCurrent >= 0;
}

int getBatteryLevel() {
  return calculateBatteryPercentage(M5.Axp.GetBatVoltage());
}

int calculateBatteryPercentage(float voltage) {
  // https://forum.micropython.org/viewtopic.php?f=2&t=7615#p43401
  int batteryPercentage =
      (int)trunc((voltage - BATTERY_MIN_VOLTAGE) * (100 - 0) /
                        (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE) +
                    0);
  if (batteryPercentage > 100) return 100;
  if (batteryPercentage < 0) return 0;
  return batteryPercentage;
}
