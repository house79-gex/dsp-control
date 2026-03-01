# Firmware ESP32-S3 – Sistema Audio PA

Firmware PlatformIO per il box di controllo centrale del sistema audio professionale basato su **ESP32-S3**.

## Requisiti

- [PlatformIO](https://platformio.org/) (estensione VS Code o CLI)
- Board: ESP32-S3-DevKitC-1
- Python ≥ 3.8 (richiesto da PlatformIO)

## Compilazione e Flash

```bash
# Compilazione
pio run -e esp32s3

# Flash (con ESP32-S3 collegato via USB)
pio run -e esp32s3 --target upload

# Monitor seriale
pio device monitor --baud 115200
```

## Struttura sorgenti

```
src/
├── main.cpp                 – Setup e loop principale
├── config.h                 – Pin, costanti, GPIO encoder e LED ring
├── audio_mode.h/cpp         – I2S ES8388 + FFT reale ESP-DSP + VU meter
├── audio_reactive.h/cpp     – Audio-reactive DMX (6 bande frequenza)
├── rs485.h/cpp              – RS-485, discovery, beep pattern
├── storage.h/cpp            – Persistenza NVS (preset, fixture, scene, WLED)
├── dsp_control.h/cpp        – Controllo DSP CQ260D
├── dmx512.h/cpp             – Controller DMX512 con task FreeRTOS
├── autotune.h/cpp           – AutoTune locale + remoto via smartphone
├── led_ring.h/cpp           – LED ring WS2812B + encoder rotativi
├── wled_client.h/cpp        – Client WLED per strip neon LED (Wi-Fi)
├── web_server.h/cpp         – WiFi AP + server HTTP REST
└── ui/
    ├── ui_home.h/cpp            – Tab home LVGL
    ├── ui_discovery.h/cpp       – Tab discovery RS-485 LVGL
    ├── ui_assignment.h/cpp      – Tab assegnazione casse LVGL
    ├── ui_dmx.h/cpp             – Tab controllo luci DMX LVGL
    ├── ui_wled.h/cpp            – Tab controllo neon WLED LVGL
    └── ui_dsp_advanced.h/cpp    – Tab DSP avanzato + VU meter LVGL
```

## Dipendenze (platformio.ini)

| Libreria                      | Versione | Uso                          |
|-------------------------------|----------|------------------------------|
| lvgl/lvgl                     | ^8.3     | Display touch UI             |
| ESP Async WebServer           | ^1.2.4   | Server HTTP asincrono        |
| bblanchon/ArduinoJson         | ^6.21    | JSON serializzazione         |
| idf-extra-components/esp_dmx  | ^4.1.0   | Output DMX512                |
| adafruit/Adafruit NeoPixel    | ^1.12.0  | LED ring WS2812B             |
| espressif/esp-dsp             | ^1.3.2   | FFT real-time                |

## Stato moduli

| Modulo                | Stato        | Note                                          |
|-----------------------|--------------|-----------------------------------------------|
| WiFi AP + REST API    | ✅ Reale     | ESPAsyncWebServer                             |
| Storage NVS           | ✅ Completo  | Preset, fixture, scene, gruppi, config        |
| FFT real-time         | ✅ Reale     | ESP-DSP, 32 bande log, finestra Hann          |
| VU meter L/R          | ✅ Reale     | RMS + peak dBFS thread-safe                   |
| DMX512 controller     | ✅ Reale     | UART1 + task FreeRTOS Core 1                  |
| Audio-reactive DMX    | ✅ Reale     | 6 bande, attack/decay configurabile           |
| AutoTune locale       | ✅ Reale     | Sweep + analisi FFT + correzione EQ/delay     |
| AutoTune remoto       | ✅ Nuovo     | Microfono smartphone via WiFi                 |
| LED ring WS2812B      | ✅ Nuovo     | Volume (verde→rosso) + balance (centro→sx/dx) |
| Encoder rotativi      | ✅ Nuovo     | Decodifica quadraturale 4 passi/click         |
| Mappa venue REST      | ✅ Nuovo     | Calcolo delay da posizione (dist/343*1000)    |
| WLED neon LED client  | ✅ Nuovo     | HTTP JSON API + UDP discovery, task FreeRTOS  |
| Relay DPDT            | ✅ Reale     | GPIO semplice                                 |
| I2S ES8388            | ⚠️ Parziale | Init I2C ES8388 da completare                 |
| RS-485 discovery      | 🔶 Stub      | Da collegare al protocollo CQ260D             |
| LVGL display driver   | 🔶 Stub      | flush/touch da collegare all'hardware         |

## Configurazione Pin

Tutti i pin sono in `src/config.h`. Sono **placeholder** – verificare con lo schema elettrico:

```
LCD_MOSI/MISO/SCLK/CS/DC/RST/BL     – Display SPI
TOUCH_SDA/SCL                         – Touch I2C
I2S_SCK/WS/SD_OUT/SD_IN              – Codec ES8388
RS485_TX/RX/DE/RE                     – MAX485
RELAY_PIN (GPIO 45)                   – Relay DPDT
LED_STATUS (GPIO 48)                  – LED integrato
LED_RING_VOL_PIN (GPIO 39)            – LED ring volume
LED_RING_BAL_PIN (GPIO 40)            – LED ring balance
ENCODER_VOL_A/B (GPIO 41/42)          – Encoder volume
ENCODER_BAL_A/B (GPIO 47/46)          – Encoder balance
```

## WiFi Access Point

- **SSID**: `SISTEMA_AUDIO_01`
- **Password**: `audio1234`
- **IP**: `192.168.4.1`


Firmware PlatformIO per il box di controllo centrale del sistema audio professionale basato su **ESP32-S3**.

## Requisiti

- [PlatformIO](https://platformio.org/) (estensione VS Code o CLI)
- Board: ESP32-S3-DevKitC-1
- Python ≥ 3.8 (richiesto da PlatformIO)

## Compilazione e Flash

```bash
# Compilazione
pio run -e esp32s3

# Flash (con ESP32-S3 collegato via USB)
pio run -e esp32s3 --target upload

# Monitor seriale
pio device monitor --baud 115200
```

## Struttura sorgenti

```
src/
├── main.cpp              – Setup e loop principale
├── config.h              – Pin e costanti di configurazione
├── audio_mode.h/cpp      – Gestione I2S ES8388 e relay DPDT
├── rs485.h/cpp           – Comunicazione RS-485, discovery, beep
├── storage.h/cpp         – Persistenza NVS (Preferences)
├── dsp_control.h/cpp     – Controllo DSP CQ260D (stub)
├── web_server.h/cpp      – WiFi AP + server HTTP REST
└── ui/
    ├── ui_home.h/cpp      – Schermata LVGL: home (volume, lista casse)
    ├── ui_discovery.h/cpp – Schermata LVGL: discovery RS-485
    └── ui_assignment.h/cpp– Schermata LVGL: assegnazione casse
```

## Parti reali vs stub/simulate

| Modulo            | Stato         | Note                                                       |
|-------------------|---------------|------------------------------------------------------------|
| WiFi AP           | ✅ Reale      | ESP32 WiFi nativo                                          |
| Server HTTP REST  | ✅ Reale      | ESPAsyncWebServer                                          |
| Pagina web HTML   | ✅ Reale      | Servita inline da flash                                    |
| Storage NVS       | ✅ Reale      | Preferences library (flash persistente)                    |
| Relay DPDT        | ✅ Reale      | GPIO semplice                                              |
| I2S ES8388 init   | ⚠️ Parziale  | Driver I2S configurato; init I2C ES8388 da completare      |
| Test tone DAC     | ⚠️ Parziale  | Generazione sine wave OK; verifica con hardware fisico     |
| RS-485 discovery  | 🔶 Stub       | Restituisce 3 dispositivi simulati; protocollo da definire |
| DSP CQ260D        | 🔶 Stub       | Solo log; protocollo reale non documentato                 |
| LVGL display      | 🔶 Stub       | Driver flush/touch da collegare all'hardware fisico        |
| FFT ADC           | 🔶 Stub       | Da implementare con ESP-DSP library                        |

## Configurazione Pin

Tutti i pin sono definiti in `src/config.h`. I valori default sono **placeholder** – verificare
con lo schema elettrico del modulo fisico prima di flashare:

```
LCD_MOSI/MISO/SCLK/CS/DC/RST/BL  – Display SPI
TOUCH_SDA/SCL                      – Touch I2C
I2S_SCK/WS/SD_OUT/SD_IN           – Codec ES8388
RS485_TX/RX/DE/RE                  – MAX485
RELAY_PIN                          – Relay DPDT
LED_STATUS                         – LED integrato (GPIO 48)
```

## WiFi Access Point

Dopo l'avvio l'ESP32 crea un Access Point:
- **SSID**: `SISTEMA_AUDIO_01`
- **Password**: `audio1234`
- **IP**: `192.168.4.1`
- **Interfaccia web**: `http://192.168.4.1`

## Note di sviluppo

- La scansione RS-485 nel loop di discovery è **bloccante** per breve tempo (intenzionale)
- ESPAsyncWebServer gestisce le richieste HTTP in modo completamente asincrono
- LVGL richiede chiamata a `lv_timer_handler()` ogni ~5 ms nel loop principale
- La libreria ESP-DSP (per FFT) può essere aggiunta come dipendenza futura in `platformio.ini`
