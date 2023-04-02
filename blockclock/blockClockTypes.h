#ifndef BLOCKCLOCKTYPES
#define BLOCKCLOCKTYPES

#include <M5StickCPlus.h>

#include "blockClockUtils.h"
#include <time.h>

struct PriceData {
  String price;
  CurrencyState currency;
  float change1h;
  float change24h;
  float change7d;
  float change30d;
  time_t timestamp;
};

#endif