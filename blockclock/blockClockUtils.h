#ifndef BLOCKCLOCKUTILS
#define BLOCKCLOCKUTILS

#include <M5StickCPlus.h>

enum ScreenState {
  BLOCKHEIGHT,
  DATEANDTIME,
  PRICE,
  BTC_CHANGE,
  NUM_SCREEN_STATES
};

enum CurrencyState {
  BRL,
  USD,
  NUM_CURRENCIES_STATES
};

extern ScreenState currentScreenState;
extern CurrencyState currentCurrencyState;

void changeScreenState();
void changeCurrencyState();
String currencyStateToString(CurrencyState currency);
String currencyStateToSymbol(CurrencyState currency);
String intWithThousandSeparator(int number);

#endif