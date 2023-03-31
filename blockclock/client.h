#ifndef CLIENT
#define CLIENT

#include <M5StickCPlus.h>

#include "blockClockTypes.h"
#include "blockClockUtils.h"

String getBlockHeight();
PriceData getBitcoinPrice(CurrencyState currencyState);

#endif