#include "client.h"

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <M5StickCPlus.h>

#include "WiFiManager.h"
#include "blockClockTypes.h"
#include "blockClockUtils.h"
#include "timeManager.h"

const String MEMPOOL_BASEURL = "https://mempool.space/api";
const String COINLIB_BASEURL = "https://coinlib.io/api/v1";
const String COINLIB_APIKEY = "3602a548384fe25c";

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

  DynamicJsonDocument doc(4096);

  const String url = COINLIB_BASEURL + "/coin?key=" + COINLIB_APIKEY +
                     "&pref=" + currency + "&symbol=BTC";

  http.begin(url);

  int httpCode = http.GET();

  PriceData priceData;
  priceData.currency = currencyState;

  if (httpCode == HTTP_CODE_OK) {
    String httpResponseBody = http.getString();
    deserializeJson(doc, httpResponseBody);

    priceData.price = intWithThousandSeparator((int)doc["price"]);
    priceData.change1h = (float)doc["delta_1h"];
    priceData.change24h = (float)doc["delta_24h"];
    priceData.change7d = (float)doc["delta_7d"];
    priceData.change30d = (float)doc["delta_30d"];
    priceData.timestamp = getTimestampFromRTC();

    return priceData;
  }

  priceData.price = "ERR " + String(httpCode);
  priceData.change1h = (float)0;
  priceData.change24h = (float)0;
  priceData.change7d = (float)0;
  priceData.change30d = (float)0;

  return priceData;
}