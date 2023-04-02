#ifndef PREFSMANAGER
#define PREFSMANAGER

#include <M5StickCPlus.h>

#include "blockClockTypes.h"
#include "blockClockUtils.h"

void wipeWiFiData();
String getPrefsSsidPasswd(String ssidPasswd);
void saveWiFiDataInStorage(String ssid, String password);
boolean dontHaveWiFiDataInPrefs();
PriceData getBitcoinDataInPrefs(CurrencyState currency);
void saveBitcoinDataInPrefs(PriceData priceData);

#endif