#ifndef CLIENT
#define CLIENT

#include <M5StickCPlus.h>

#include "blockClockTypes.h"
#include "blockClockUtils.h"

class ApiClient {
 public:
  explicit ApiClient(const String& apiKey);

  String getBlockHeight();
  RecommendedFees getRecommendedFees();
  PriceData getBitcoinPrice(CurrencyState currencyState);

 private:
  String coinlibApiKey;
  HTTPClient http;
};

#endif