#include "client.h"

#include <HTTPClient.h>
#include <M5StickCPlus.h>

#include <string>

#include "WiFiManager.h"

const String MEMPOOL_BASEURL = "https://mempool.space/api";

HTTPClient http;

String getBlockHeight() {
  http.begin(MEMPOOL_BASEURL + "/blocks/tip/height");
  int httpCode = http.GET();
  if (httpCode == 200) {
    return http.getString();
  }
  return "ERR " + httpCode;
}
