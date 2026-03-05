# DSP Control – Project Overview

## Obiettivo

Sistema di controllo DSP professionale per impianti audio live basato su ESP32-S3.
Gestisce DSP CQ260D via RS-485, codec audio ES8388, display touch 5" e luci DMX512.

## Architettura Hardware

```
┌─────────────────────────────────────────────────────────┐
│                     ESP32-S3 (240MHz)                   │
│                   8MB PSRAM + 16MB Flash                │
│                                                         │
│  ┌──────────┐   ┌──────────┐   ┌──────────────────────┐│
│  │ ES8388   │   │ Display  │   │     WiFi / BLE       ││
│  │ ADC/DAC  │   │ 800×480  │   │   REST API / OTA     ││
│  │ 44.1kHz  │   │ RGB Touch│   └──────────────────────┘│
│  └────┬─────┘   └────┬─────┘                           │
│  I2S/I2C        RGB+GT911                               │
└───────┼──────────────┼─────────────────────────────────┘
        │              │
    ┌───┴───┐      ┌────┴────┐
    │SC LIVE│      │ Operatore│
    │  4    │      │ touch UI │
    └───────┘      └─────────┘
        │
    ┌───┴────────────────────────────────┐
    │         RS-485 Bus (115200 baud)   │
    │  DSP CQ260D ×N (@ 48kHz fisso)    │
    │  ↓ delay/EQ/crossover/compressor  │
    │  Speaker Array (casse + sub)       │
    └────────────────────────────────────┘
```

## Stack Software

| Layer | Tecnologia |
|-------|-----------|
| RTOS | FreeRTOS (ESP-IDF) |
| Framework | Arduino-ESP32 |
| Display | LovyanGFX + LVGL 8.3 |
| Audio | ESP-DSP + ES8388 driver |
| Storage | NVS (Preferences) |
| Network | AsyncWebServer + REST |
| Light | DMX512 (UART1) |
| Wireless | ESP-NOW (audio RX) |

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

```
firmware/esp32/src/
├── config.h              # Pin ESP32-S3, costanti audio
├── main.cpp              # Entry point, init sequenza
├── audio_config.h/.cpp   # Sistema audio adattivo multi-SR
├── audio_src.h/.cpp      # Sample Rate Converter (44.1k→48k)
├── audio_protection.h/.cpp # DC block, HPF, limiter, soft clip
├── audio_delay_buffer.h/.cpp # Delay buffer PSRAM (max 100ms)
├── audio_mode.h/.cpp     # Gestione modalità audio + FFT
├── dsp_control.h/.cpp    # Controllo DSP CQ260D
├── dsp_protocol.h/.cpp   # Protocollo RS-485 CQ260D
├── dsp_registers.h       # Struttura SYSREG DSP
├── display/
│   └── lvgl_display.h/.cpp # LovyanGFX per UEDX80480050E-WB-A
├── drivers/
│   ├── ES8388.h/.cpp     # Driver codec ES8388
│   └── display_driver.h/.cpp
└── ...
```
