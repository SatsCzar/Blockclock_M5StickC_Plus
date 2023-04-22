#ifndef TIMEMANAGER
#define TIMEMANAGER

#include "userBoardDefines.h"

#ifdef M5STACK
#include <M5StickCPlus.h>
#endif

#ifdef GENERIC_ESP32
#include <Arduino.h>
#endif

#include <time.h>

struct BlockClockDateAndTime {
  uint8_t day;
  uint8_t month;
  uint16_t year;
  uint8_t hour;
  uint8_t minutes;
  uint8_t seconds;
};

void timeManagerbegin();
time_t getTimestampFromRTC();
BlockClockDateAndTime getDateAndTime();
#ifdef M5STACK
void syncRTCToNTP();
RTC_TimeTypeDef getTime();
RTC_DateTypeDef getDate();
#endif

#endif