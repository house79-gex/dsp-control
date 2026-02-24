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
