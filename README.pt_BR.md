# Blockclock para M5Stick CPlus

Este é um versátil blockclock de Bitcoin para o M5StickC Plus (ESP32) que se conecta ao Wi-Fi e busca dados de mempool.space a cada minuto. A versão atualizada agora suporta várias telas, exibindo informações adicionais, como preço, taxas de transação, data e hora e muito mais. Esta versão requer a biblioteca ESP32-Bitcoin-BlockClock-Library, que pode ser encontrada no [GitHub](https://github.com/SatsCzar/ESP32-Bitcoin-BlockClock-Library).

![M5StickC executando blockclock](https://raw.githubusercontent.com/bolsonitro/Blockclock_M5StickC_Plus/master/m5stickblockclock.jpg)

## Instalação

1. Baixe a versão mais recente do Arduino IDE
2. Siga este [tutorial](https://docs.m5stack.com/en/quick_start/m5stickc_plus/arduino) para instalar as bibliotecas necessárias, que incluem M5StickC Plus, ArduinoJson e [ESP32-Bitcoin-BlockClock-Library](https://github.com/SatsCzar/ESP32-Bitcoin-BlockClock-Library)
3. Clone o repositório e abra o arquivo blockclock.ino com o Arduino IDE
4. Certifique-se de editar o arquivo `userBoardDefines.h` na ESP32-Bitcoin-BlockClock-Library para definir o tipo de dispositivo. Por exemplo:
```
#define GENERIC_ESP32 1
/* #define M5STACK 1 */
#ifdef GENERIC_ESP32
#define BUTTON1PIN 35
#define BUTTON2PIN 0
#endif
```
5. Selecione a porta COM correspondente ao seu M5Stick e o modelo
6. Compile e faça o upload do código usando estes comandos:
```
# Comando para compilar
arduino-cli compile --fqbn m5stack:esp32:m5stick-c-plus:PartitionScheme=default,CPUFreq=240,UploadSpeed=1500000,DebugLevel=none,EraseFlash=none --build-path ./build --port COM5 blockclock/blockclock.ino

# Comando para fazer upload
arduino-cli upload --fqbn m5stack:esp32:m5stick-c-plus:PartitionScheme=default,CPUFreq=240,UploadSpeed=1500000,DebugLevel=none,EraseFlash=none --programmer esptool --input-dir ./build --port COM5
```

## Telas

| #   | Tela            | Função                             |
| --- | --------------- | ---------------------------------- |
| 1   | BlockHeight     | Mostra a altura atual do bloco     |
| 2   | TransactionFees | Mostra as taxas recomendadas em Sats/Byte |
| 3   | DateTime        | Mostra a data e hora atuais        |
| 4   | Price           | Mostra o preço atual do BTC        |
| 5   | Change          | Mostra a variação do BTC em 1h, 24h, 7d, 30d |
| 6   | WiFiData        | Mostra informações sobre o Wi-Fi   |

## Recursos

- Usa o relógio em tempo real do M5StickC para gerenciar o tempo e controlar as atualizações de informações na tela
- Busca a data e hora atuais do servidor NTP e define essas informações no relógio em tempo real
- Armazena

 o preço e as mudanças de preço do Bitcoin em armazenamento persistente, válido por 5 minutos, reduzindo as buscas na API
- Suporta várias moedas para exibição de preço, incluindo BRL e USD
- Melhora a exibição da porcentagem da bateria
- Armazena as credenciais do Wi-Fi (SSID e senha) de forma persistente após a conclusão bem-sucedida do smartconfig

## Conectando ao Wi-Fi

O código usa smartconfig do ESP32 para se conectar ao Wi-Fi, portanto, você precisa do aplicativo [ESP-TOUCH](https://www.espressif.com/en/products/software/esp-touch/resources), que pode ser encontrado no site da EspressIF.

![SmartConfig](https://www.espressif.com/sites/default/files/faq/screen_shot_2016-04-27_at_1.30.27_pm_0.png)