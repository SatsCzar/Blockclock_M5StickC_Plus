#include "timeManager.h"

#include <time.h>

#include "userBoardDefines.h"

#ifdef M5STACK
#include <M5StickCPlus.h>
#endif

#ifdef GENERIC_ESP32
#include <Arduino.h>
#endif

const long gmtOffset_sec = 0;
const int daylightOffset_sec = -3600 * 3;
const char* ntpServer = "pool.ntp.org";

void timeManagerbegin() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

#ifdef M5STACK
  M5.Rtc.begin();
  syncRTCToNTP();
#endif
}

#ifdef M5STACK
void syncRTCToNTP() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return;
  }

  RTC_TimeTypeDef currentTime;
  currentTime.Hours = timeinfo.tm_hour;
  currentTime.Minutes = timeinfo.tm_min;
  currentTime.Seconds = timeinfo.tm_sec;

  RTC_DateTypeDef currentDate;
  currentDate.Year = timeinfo.tm_year + 1900;
  currentDate.Month = timeinfo.tm_mon + 1;
  currentDate.Date = timeinfo.tm_mday;

  M5.Rtc.SetTime(&currentTime);
  M5.Rtc.SetData(&currentDate);
}
#endif

time_t getTimestampFromRTC() {
#ifdef M5STACK
  RTC_TimeTypeDef currentTime = getTime();
  RTC_DateTypeDef currentDate = getDate();

  struct tm timeinfo;
  timeinfo.tm_year = currentDate.Year + 1900;
  timeinfo.tm_mon = currentDate.Month - 1;
  timeinfo.tm_mday = currentDate.Date;
  timeinfo.tm_hour = currentTime.Hours;
  timeinfo.tm_min = currentTime.Minutes;
  timeinfo.tm_sec = currentTime.Seconds;

  return mktime(&timeinfo);
#endif

#ifdef GENERIC_ESP32
  struct tm timeinfo;
  getLocalTime(&timeinfo);

  time_t timestamp = mktime(&timeinfo);

  return timestamp;
#endif
}

BlockClockDateAndTime getDateAndTime() {
#ifdef M5STACK
  RTC_TimeTypeDef currentTime = getTime();
  RTC_DateTypeDef currentDate = getDate();

  BlockClockDateAndTime currentDateAndTime;
  currentDateAndTime.year = currentDate.Year;
  currentDateAndTime.month = currentDate.Month;
  currentDateAndTime.day = currentDate.Date;
  currentDateAndTime.hour = currentTime.Hours;
  currentDateAndTime.minutes = currentTime.Minutes;
  currentDateAndTime.seconds = currentTime.Seconds;

  return currentDateAndTime;
#endif

#ifdef GENERIC_ESP32
  struct tm timeinfo;
  getLocalTime(&timeinfo);

  BlockClockDateAndTime currentDateAndTime;

  currentDateAndTime.year = timeinfo.tm_year + 1900;
  currentDateAndTime.month = timeinfo.tm_mon + 1;
  currentDateAndTime.day = timeinfo.tm_mday;
  currentDateAndTime.hour = timeinfo.tm_hour;
  currentDateAndTime.minutes = timeinfo.tm_min;
  currentDateAndTime.seconds = timeinfo.tm_sec;

  return currentDateAndTime;
#endif
}

#ifdef M5STACK
RTC_TimeTypeDef getTime() {
  RTC_TimeTypeDef timeNow;

  M5.Rtc.GetTime(&timeNow);

  return timeNow;
}

RTC_DateTypeDef getDate() {
  RTC_DateTypeDef dateNow;

  M5.Rtc.GetData(&dateNow);

  return dateNow;
}
#endif
