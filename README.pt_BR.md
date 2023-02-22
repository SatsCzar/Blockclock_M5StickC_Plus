# Blockclock for M5Stick CPlus

Este é um relógio de blocos que funciona conectado à wifi, puxando dados da mempool.space à cada 1 minuto.

![M5StickC running blockclock](https://raw.githubusercontent.com/bolsonitro/Blockclock_M5StickC_Plus/master/m5stickblockclock.jpg)

## Instalando

1. Baixe a ultima versão do Arduino IDE
2. Siga o [tutorial da M5Stack](https://docs.m5stack.com/en/quick_start/m5stickc_plus/arduino)
3. Clone o repósitório e abra o arquivo blockclock.ino com a IDE do arduino
4. Selecione a porta COM correspondente à sua M5Stick e o modelo
5. Grave o código (Seta para a direita no canto superior esquerdo)

## Conectando ao WiFi

O código usa smartconfig do ESP32 para realizar a conexão com o WiFi, portanto, precisa-se do aplicativo [ESP-TOUCH](https://www.espressif.com/en/products/software/esp-touch/resources) que encontra-se no website da EspressIF.
![Conexão ao WiFi](https://www.espressif.com/sites/default/files/faq/screen_shot_2016-04-27_at_1.30.27_pm_0.png)