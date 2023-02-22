# Blockclock for M5Stick CPlus

This is a bitcoin blockclock that works connected to wifi, pulling data from mempool.space every 1 minute.

![M5StickC running blockclock](https://raw.githubusercontent.com/bolsonitro/Blockclock_M5StickC_Plus/master/m5stickblockclock.jpg)

## Instalando

1. Download the latest version of Arduino IDE
2. Follow the tutorial [tutorial](https://docs.m5stack.com/en/quick_start/m5stickc_plus/arduino) to install the libraries
3. Clone the repository and open the blockclock.ino file with the arduino IDE
4. Select the COM port corresponding to your M5Stick and the model
5. Upload the code

## Connecting to WiFi

The code uses smartconfig from ESP32 to connect to WiFi, so you need the app [ESP-TOUCH](https://www.espressif.com/en/products/software/esp-touch/resources) which can be found on the EspressIF website.
![SmartConfig](https://www.espressif.com/sites/default/files/faq/screen_shot_2016-04-27_at_1.30.27_pm_0.png)

