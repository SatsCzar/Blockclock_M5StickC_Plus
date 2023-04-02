#include "prefsManager.h"

#include <M5StickCPlus.h>
#include <Preferences.h>

#include "blockClockTypes.h"
#include "blockClockUtils.h"

Preferences preferences;

void wipeWiFiData() {
  preferences.begin("wifi");
  preferences.clear();
  preferences.end();
}

String getPrefsSsidPasswd(String ssidPasswd) {
  ssidPasswd.toUpperCase();
  preferences.begin("wifi");

  String value;

  if (ssidPasswd == "SSID") {
    value = preferences.getString("ssid", "none");
  }
  if (ssidPasswd == "PASS") {
    value = preferences.getString("password", "none");
  }

  preferences.end();

  return value;
}

void saveWiFiDataInStorage(String ssid, String password) {
  preferences.begin("wifi");
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.end();
}

boolean dontHaveWiFiDataInPrefs() {
  String ssid = getPrefsSsidPasswd("ssid");

  if (ssid != "none") {
    return false;
  }

  return true;
}

PriceData getBitcoinDataInPrefs(CurrencyState currency) {
  PriceData priceData;

  String symbol = currencyStateToString(currency);
  String path = "BTC-" + symbol;

  preferences.begin(path.c_str());
  priceData.price = preferences.getString("price", "");
  priceData.change1h = preferences.getFloat("change1h", 0.0);
  priceData.change24h = preferences.getFloat("change_24h", 0.0);
  priceData.change7d = preferences.getFloat("change_7d", 0.0);
  priceData.change30d = preferences.getFloat("change_30d", 0.0);
  priceData.timestamp = preferences.getLong64("timestamp", 0);
  priceData.currency = currency;
  preferences.end();

  return priceData;
}

void saveBitcoinDataInPrefs(PriceData priceData) {
  String symbol = currencyStateToString(priceData.currency);
  String path = "BTC-" + symbol;

  preferences.begin(path.c_str());
  preferences.putString("price", priceData.price);
  preferences.putFloat("change_1h", priceData.change1h);
  preferences.putFloat("change_24h", priceData.change24h);
  preferences.putFloat("change_7d", priceData.change7d);
  preferences.putFloat("change_30d", priceData.change30d);
  preferences.putLong64("timestamp", priceData.timestamp);
  preferences.end();
}