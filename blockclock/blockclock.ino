#include <HTTPClient.h>
#include <M5StickCPlus.h>
#include <Preferences.h>  // WiFi storage
#include <WiFi.h>
#include <esp_wifi.h>
#include <string.h>

#include <cmath>
#include <string>

#include "FS.h"
#include "esp32-hal-cpu.h"
#include "esp_system.h"
#include "esp_wifi.h"

HTTPClient http;

String blockHeightGlobal;
int batteryLevel;
const float BATTERY_MIN_VOLTAGE = 3.7;
const float BATTERY_MAX_VOLTAGE = 4.1;

const char* rssiSSID;  // NO MORE hard coded set AP, all SmartConfig
const char* password;
String PrefSSID, PrefPassword;  // used by preferences storage

int WFstatus;
int UpCount = 0;
int32_t rssi;  // store WiFi signal strength here
String getSsid;
String getPass;
String MAC;

// SSID storage
Preferences preferences;  // declare class object
// END SSID storage

void setup() {
  setCpuFrequencyMhz(80);  // Lower processor clock to save power
  M5.begin(true, true, true);

  M5.Lcd.setTextSize(2);
  M5.Lcd.setRotation(1);
  M5.Axp.ScreenBreath(9);

  M5.Lcd.setCursor(10, 10);
  M5.Lcd.println("BLOCKCLOCK");
  M5.Lcd.setCursor(10, 30);
  M5.Lcd.setTextSize(1);
  M5.Lcd.println("Waiting for Wifi config");
  wifiInit();
}

void loop() {
  String blockheight;
  M5.Lcd.setTextSize(1);

  if (WiFi.status() != WL_CONNECTED) {
    wifiprefsbegin();
  }

  if (WiFi.status() == WL_CONNECTED) {
    blockheight = getBlockHeight();
    if (blockheight != blockHeightGlobal) {
      blockHeightGlobal = blockheight;
      clearScreen();
      printInfo();
    }
    if (!isCharging()) {
      printBattery();
      
    }
  }

  delay(60000);  // 1 minute
}

String getBlockHeight() {
  http.begin("https://mempool.space/api/blocks/tip/height");
  int httpCode = http.GET();
  if (httpCode == 200) {
    return http.getString();
  }
  return "ERR " + httpCode;
}

void printInfo() {
  M5.Lcd.setCursor(5, 10);
  M5.Lcd.setTextSize(3);
  M5.Lcd.print("Block Height:");
  M5.Lcd.setCursor(60, 80);
  M5.Lcd.print(blockHeightGlobal);
}

bool isCharging() {
  float batteryCurrent = M5.Axp.GetBatCurrent();
  return batteryCurrent >= 0;
}

void printBattery() {
  M5.Lcd.setTextSize(2);
  batteryLevel = calculateBatteryPercentage(M5.Axp.GetBatVoltage());
  if (batteryLevel >= 100) {
    M5.Lcd.setCursor(185, 115);
    M5.Lcd.print("100%");
    return;
  }
  M5.Lcd.setCursor(200, 115);
  M5.Lcd.print(String(batteryLevel) + "%");
}

int calculateBatteryPercentage(float voltage) {
  // https://forum.micropython.org/viewtopic.php?f=2&t=7615#p43401
  return (int)trunc((voltage - BATTERY_MIN_VOLTAGE) * (100 - 0) /
                        (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE) +
                    0);
}

void clearScreen() { M5.Lcd.fillRect(0, 0, 240, 135, BLACK); }

void wifiprefsbegin() {
  WFstatus = getWifiStatus(WFstatus);

  WiFi.begin(PrefSSID.c_str(), PrefPassword.c_str());
  int WLcount = 0;
  while (WiFi.status() != WL_CONNECTED && WLcount < 200) {
    ++WLcount;
  }

  if (getWifiStatus(WFstatus) == 3)  // wifi returns
  {
    // stop LED flasher, wifi going up
  }
  delay(100);
}

void wifiInit()  //
{
  WiFi.mode(WIFI_AP_STA);  // required to read NVR before WiFi.begin()

  // load credentials from NVR, a little RTOS code here
  wifi_config_t conf;
  esp_wifi_get_config(WIFI_IF_STA, &conf);  // load wifi settings to struct comf
  rssiSSID = reinterpret_cast<const char*>(conf.sta.ssid);
  password = reinterpret_cast<const char*>(conf.sta.password);

  Serial.printf( "%s\n", rssiSSID );
  Serial.printf( "%s\n", password );

  // Open Preferences with "wifi" namespace. Namespace is limited to 15 chars
  preferences.begin("wifi", false);
  PrefSSID = preferences.getString("ssid", "none");          // NVS key ssid
  PrefPassword = preferences.getString("password", "none");  // NVS key password
  preferences.end();

  // keep from rewriting flash if not needed
  if (!checkPrefsStore())    // see is NV and Prefs are the same
  {                          // not the same, setup with SmartConfig
    if (PrefSSID == "none")  // New...setup wifi
    {
      initSmartConfig();
      delay(3000);
      ESP.restart();  // reboot with wifi configured
    }
  }

  // I flash LEDs while connecting here

  M5.Lcd.setCursor(10, 50);
  M5.Lcd.println("Connecting to: " + String(PrefSSID.c_str()));
  WiFi.begin(PrefSSID.c_str(), PrefPassword.c_str());

  int WLcount = 0;
  while (WiFi.status() != WL_CONNECTED &&
         WLcount < 200)  // can take > 100 loops depending on router settings
  {
    delay(100);
    Serial.printf(".");
    ++WLcount;
  }
  esp_wifi_set_ps(WIFI_PS_MAX_MODEM);  // Set max power save
  M5.Lcd.setCursor(10, 60);
  M5.Lcd.println("Connected");
  delay(1000);

  //  stop the led flasher here
}

// match WiFi IDs in NVS to Pref store,  assumes WiFi.mode(WIFI_AP_STA);  was
// executed
bool checkPrefsStore() {
  bool val = false;
  String NVssid, NVpass, prefssid, prefpass;

  NVssid = getSsidPass("ssid");
  NVpass = getSsidPass("pass");

  // Open Preferences with my-app namespace. Namespace name is limited to 15
  // chars
  preferences.begin("wifi", false);
  prefssid = preferences.getString("ssid", "none");      // NVS key ssid
  prefpass = preferences.getString("password", "none");  // NVS key password
  preferences.end();

  if (NVssid.equals(prefssid) && NVpass.equals(prefpass)) {
    val = true;
  }

  return val;
}

// optionally call this function any way you want in your own code
// to remap WiFi to another AP using SmartConfig mode.   Button, condition etc..
void initSmartConfig() {
  // start LED flasher
  int loopCounter = 0;

  WiFi.mode(WIFI_AP_STA);  // Init WiFi, start SmartConfig
  Serial.printf("Entering SmartConfig\n");

  WiFi.beginSmartConfig();

  while (!WiFi.smartConfigDone()) {
    // flash led to indicate not configured
    Serial.printf(".");
    if (loopCounter >= 40)  // keep from scrolling sideways forever
    {
      loopCounter = 0;
      Serial.printf("\n");
    }
    delay(600);
    ++loopCounter;
  }
  loopCounter = 0;

  // stopped flasher here

  M5.Lcd.setCursor(10, 50);
  M5.Lcd.println("Smartconfig received");
  M5.Lcd.println("Restarting");
  Serial.printf("\nSmartConfig received.\n Waiting for WiFi\n\n");
  delay(2000);

  while (WiFi.status() != WL_CONNECTED)  // check till connected
  {
    delay(500);
  }
  IP_info();  // connected lets see IP info

  preferences.begin("wifi", false);  // put it in storage
  preferences.putString("ssid", getSsid);
  preferences.putString("password", getPass);
  preferences.end();

  delay(300);
}  // END SmartConfig()

void IP_info() {
  getSsid = WiFi.SSID();
  getPass = WiFi.psk();
  rssi = getRSSI(rssiSSID);
  WFstatus = getWifiStatus(WFstatus);
  MAC = getMacAddress();

  M5.Lcd.println(getSsid.c_str());
  M5.Lcd.println(WiFi.localIP());
}

int getWifiStatus(int WiFiStatus) {
  WiFiStatus = WiFi.status();
  Serial.printf("\tStatus %d", WiFiStatus);
  switch (WiFiStatus) {
    case WL_IDLE_STATUS:  // WL_IDLE_STATUS     = 0,
      Serial.printf(", WiFi IDLE \n");
      break;
    case WL_NO_SSID_AVAIL:  // WL_NO_SSID_AVAIL   = 1,
      Serial.printf(", NO SSID AVAIL \n");
      break;
    case WL_SCAN_COMPLETED:  // WL_SCAN_COMPLETED  = 2,
      Serial.printf(", WiFi SCAN_COMPLETED \n");
      break;
    case WL_CONNECTED:  // WL_CONNECTED       = 3,
      Serial.printf(", WiFi CONNECTED \n");
      break;
    case WL_CONNECT_FAILED:  // WL_CONNECT_FAILED  = 4,
      Serial.printf(", WiFi WL_CONNECT FAILED\n");
      break;
    case WL_CONNECTION_LOST:  // WL_CONNECTION_LOST = 5,
      Serial.printf(", WiFi CONNECTION LOST\n");
      WiFi.persistent(false);  // don't write FLASH
      break;
    case WL_DISCONNECTED:  // WL_DISCONNECTED    = 6
      Serial.printf(", WiFi DISCONNECTED ==\n");
      WiFi.persistent(false);  // don't write FLASH when reconnecting
      break;
  }
  return WiFiStatus;
}
// END getWifiStatus()

// Get the station interface MAC address.
// @return String MAC
String getMacAddress(void) {
  WiFi.mode(WIFI_AP_STA);  // required to read NVR before WiFi.begin()
  uint8_t baseMac[6];
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);  // Get MAC address for WiFi station
  char macStr[18] = {0};
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1],
          baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
  return String(macStr);
}
// END getMacAddress()

// Return RSSI or 0 if target SSID not found
// const char* SSID = "YOUR_SSID";  // declare in GLOBAL space
// call:  int32_t rssi = getRSSI( SSID );
int32_t getRSSI(const char* target_ssid) {
  byte available_networks = WiFi.scanNetworks();

  for (int network = 0; network < available_networks; network++) {
    if (strcmp(WiFi.SSID(network).c_str(), target_ssid) == 0) {
      return WiFi.RSSI(network);
    }
  }
  return 0;
}  //  END  getRSSI()

// Requires; #include <esp_wifi.h>
// Returns String NONE, ssid or pass arcording to request
// ie String var = getSsidPass( "pass" );
String getSsidPass(String s) {
  String val = "NONE";  // return "NONE" if wrong key sent
  s.toUpperCase();
  if (s.compareTo("SSID") == 0) {
    wifi_config_t conf;
    esp_wifi_get_config(WIFI_IF_STA, &conf);
    val = String(reinterpret_cast<const char*>(conf.sta.ssid));
  }
  if (s.compareTo("PASS") == 0) {
    wifi_config_t conf;
    esp_wifi_get_config(WIFI_IF_STA, &conf);
    val = String(reinterpret_cast<const char*>(conf.sta.password));
  }
  return val;
}