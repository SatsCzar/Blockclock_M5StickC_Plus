# Blockclock for M5Stick CPlus

This is a versatile Bitcoin blockclock for the M5StickC Plus (ESP32) that connects to Wi-Fi and fetches data from mempool.space every minute. The updated version now supports multiple screens, displaying additional information such as price, transaction fees, date and time, and more.

![M5StickC running blockclock](https://raw.githubusercontent.com/SatsCzar/Blockclock_M5StickC_Plus/master/m5stickblockclock.jpg)

## Installing

1. Download the latest version of Arduino IDE
2. Follow this [tutorial](https://docs.m5stack.com/en/quick_start/m5stickc_plus/arduino) to install the required libraries, which include M5StickC Plus and ArduinoJson
3. Clone the repository and open the blockclock.ino file with the Arduino IDE
4. Select the COM port corresponding to your M5Stick and the model
5. Upload the code

## Screens

| #   | Screen          | Function                           |
| --- | --------------- | ---------------------------------- |
| 1   | BlockHeight     | Show current blockheight           |
| 2   | TransactionFees | Show recommended fees in Sats/Byte |
| 3   | DateTime        | Show current date and time         |
| 4   | Price           | Show current price of BTC          |
| 5   | Change          | Show BTC change in 1h, 24h, 7d, 30d |
| 6   | WiFiData        | Show information about Wi-Fi       |

## Features

- Uses the M5StickC's real-time clock to manage time and control on-screen information updates
- Fetches current time and date from NTP server and sets this information in the real-time clock
- Stores Bitcoin price and price changes in persistent storage, valid for 5 minutes, reducing API fetches
- Supports multiple currencies for price display, including BRL and USD
- Improved battery percentage display
- Stores Wi-Fi credentials (SSID and password) persistently after successful smartconfig

## Connecting to Wi-Fi

The code uses smartconfig from ESP32 to connect to Wi-Fi, so you need the app [ESP-TOUCH](https://www.espressif.com/en/products/software/esp-touch/resources), which can be found on the EspressIF website.

![SmartConfig](https://www.espressif.com/sites/default/files/faq/screen_shot_2016-04-27_at_1.30.27_pm_0.png)
