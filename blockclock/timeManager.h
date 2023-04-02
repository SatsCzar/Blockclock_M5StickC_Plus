#ifndef TIMEMANAGER
#define TIMEMANAGER

#include <M5StickCPlus.h>
#include <time.h>

void timeManagerbegin();
void syncRTCToNTP();
time_t getTimestampFromRTC();
RTC_TimeTypeDef getTime();
RTC_DateTypeDef getDate();

#endif