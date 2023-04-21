#include "timeManager.h"

#include <M5StickCPlus.h>
#include <time.h>

const long gmtOffset_sec = 0;
const int daylightOffset_sec = -3600 * 3;
const char* ntpServer = "pool.ntp.org";

void timeManagerbegin() {
  M5.Rtc.begin();
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  syncRTCToNTP();
}

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

time_t getTimestampFromRTC() {
  RTC_TimeTypeDef currentTime;
  RTC_DateTypeDef currentDate;
  M5.Rtc.GetTime(&currentTime);
  M5.Rtc.GetData(&currentDate);

  struct tm timeinfo;
  timeinfo.tm_year = currentDate.Year + 1900;
  timeinfo.tm_mon = currentDate.Month - 1;
  timeinfo.tm_mday = currentDate.Date;
  timeinfo.tm_hour = currentTime.Hours;
  timeinfo.tm_min = currentTime.Minutes;
  timeinfo.tm_sec = currentTime.Seconds;

  return mktime(&timeinfo);
}

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
