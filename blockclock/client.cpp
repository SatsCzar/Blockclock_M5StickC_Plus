#include "client.h"

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <M5StickCPlus.h>

#include "WiFiManager.h"
#include "blockClockTypes.h"
#include "blockClockUtils.h"

const String MEMPOOL_BASEURL = "https://mempool.space/api";
const String COINLIB_BASEURL = "https://coinlib.io/api/v1";
const String COINLIB_APIKEY = "YOUR_API_KEY_HERE";

HTTPClient http;

String getBlockHeight() {
  http.begin(MEMPOOL_BASEURL + "/blocks/tip/height");
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    return http.getString();
  }
  return "ERR " + httpCode;
}

PriceData getBitcoinPrice(CurrencyState currencyState) {
  String currency = currencyStateToString(currencyState);

  DynamicJsonDocument doc(2000);

  http.begin(COINLIB_BASEURL + "/coin?key=" + COINLIB_APIKEY +
             "&pref=" + currency + "&symbol=BTC");
  int httpCode = http.GET();

  PriceData priceData;

  if (httpCode == HTTP_CODE_OK) {
    deserializeJson(doc, http.getString());

    priceData.currency = currencyState;
    priceData.price = intWithThousandSeparator((int)doc["price"]);
    priceData.change1h = (float)doc["delta_1h"];
    priceData.change24h = (float)doc["delta_24h"];
    priceData.change7d = (float)doc["delta_7d"];
    priceData.change30d = (float)doc["delta_30d"];

    return priceData;
  }

  priceData.price = "ERR " + String(httpCode);

  return priceData;
}