# DSP Control – Project Overview

## Obiettivo

Sistema di controllo DSP professionale per impianti audio live. **Architettura attuale: Dual-ESP32-S3** (Master + Slave). Gestisce DSP CQ260D via RS-485, codec ES8388, display touch 5" UEDX, luci DMX512, WLED, AutoTune, app Flutter.

Per diagrammi e dettagli vedi **[ARCHITECTURE.md](ARCHITECTURE.md)** e **[README.md](README.md)**.

## Architettura hardware (sintesi)

- **ESP32 #1 (Master)**: Display UEDX 800×480, ES8388 I2S, RS-485 CQ260D, WiFi AP, REST API, IPC verso Slave.
- **ESP32 #2 (Slave)**: I2S RX, FFT 6 bande, DMX512 out, ESP-NOW, NVS storage, encoder/LED ring/relay (periferiche).
- Bus RS-485 115200 baud verso DSP CQ260D ×N (@ 48kHz fisso).

## Stack Software

| Layer | Tecnologia |
|-------|-----------|
| RTOS | FreeRTOS (ESP-IDF) |
| Framework | Arduino-ESP32 |
| Display | LovyanGFX + LVGL 8.3 |
| Audio | ESP-DSP + ES8388 driver |
| Storage | NVS (Preferences) |
| Network | AsyncWebServer + REST |
| Light | DMX512 (Slave), WLED HTTP/UDP |
| Wireless | ESP-NOW (Slave), WiFi AP (Master) |

## Quick Start

```bash
# Prerequisiti: PlatformIO CLI
pip install platformio

# Build firmware
cd firmware/esp32
pio run -e esp32s3

# Upload
pio run -e esp32s3 --target upload

# Monitor seriale
pio device monitor --baud 115200

# Test unitari (ambiente native, no HW)
pio test -e native
```

## Specifiche Tecniche

| Parametro | Valore |
|-----------|--------|
| CPU | ESP32-S3, 240MHz dual-core |
| PSRAM | 8MB (OPI PSRAM) |
| Flash | 16MB |
| Display | 800×480 px, RGB 24-bit, 60fps |
| Touch | GT911, 5 punti, I2C 400kHz |
| Audio ADC/DAC | ES8388, 24-bit, 44.1/48/96kHz |
| DSP | CQ260D, 4in/4out, 48kHz fisso |
| Bus DSP | RS-485, 115200 baud |
| Max DSP delay | 1365 ms (65535 campioni @ 48kHz) |
| Latenza audio | <5ms (senza delay buffer) |
| DMX canali | 512 |

## Struttura Repository

Albero completo in **[README.md](README.md)**. In sintesi: `firmware/esp32/` (Master), `firmware/esp32_slave/` (Slave), `app/flutter/`, `docs/`, `web/`. Master `src/`: config.h, main.cpp, ipc_master, audio_*, storage, rs485, dsp_control, autotune, wled_client, web_server, ui/, display/, drivers/.
