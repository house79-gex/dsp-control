# Firmware ESP32-S3 Master – Display + Audio + RS-485 + WiFi

Firmware PlatformIO per il box di controllo centrale del sistema audio professionale basato su **ESP32-S3** (architettura Dual-ESP32 v2.0+).

Questo è il firmware dell'**ESP32 #1 (Master)**. Per lo Slave, vedere [`firmware/esp32_slave/`](../esp32_slave/).

## Ruolo nel Sistema

| Responsabilità | Stato |
|----------------|-------|
| Display LVGL 800×480 touch GT911 | ✅ |
| ES8388 ADC/DAC (I2S Master, genera BCLK/WS) | ✅ |
| RS-485 → DSP CQ260D | ✅ |
| WiFi AP + REST API | ✅ |
| IPC UART verso ESP32 #2 (Slave) | ✅ Nuovo |
| Storage NVS (con fallback se Slave non disponibile) | ✅ Aggiornato |

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
├── config.h                 – Pin, costanti, GPIO (Master v2.0: I2S/IPC/RS485 aggiornati)
├── ipc_master.h/cpp         – Protocollo UART IPC verso ESP32 #2 Slave (NUOVO v2.0)
├── audio_mode.h/cpp         – I2S Master (genera BCLK GPIO12/WS GPIO13) + FFT reale ESP-DSP + VU meter
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
| RS-485 discovery      | ✅ Scan dsp_connect per GRP/ID                   |
| LVGL + LovyanGFX      | ✅ UEDX80480050E + GT911 (USE_LOVYAN_GFX)       |

## Configurazione Pin

Tutti i pin sono definiti in `src/config.h` (board UEDX80480050E-WB-A V3.3). Riferimento: **`docs/PINOUT_REFERENCE.md`** e **`docs/MASTER_SLAVE_ARCHITECTURE.md`**. Con `USE_SLAVE_PERIPHERALS=1` (default) encoder e LED ring sono sullo Slave via IPC; relay/LED di stato sono su TCA9535 (I2C 0x20). I2S: BCLK=12, WS=13, DIN=11, DOUT=10 (o da Slave se `USE_SLAVE_ES8388_DOUT`). RS-485: TX=43, RX=44, DE/RE=-1 (modulo auto) o GPIO10.

## WiFi Access Point

- **SSID**: `SISTEMA_AUDIO_01`
- **Password**: `audio1234`
- **IP**: `192.168.4.1`

## Note

- Avviare **ESP32 #2 (Slave) prima** del Master per IPC.
- RS-485 discovery è bloccante per breve tempo (intenzionale).
- LVGL: `lv_timer_handler()` ogni ~5 ms nel loop principale.
