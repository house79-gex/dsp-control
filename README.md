# DSP Control – Sistema Audio PA Professionale

[![Versione](https://img.shields.io/badge/versione-2.1.0-blue)](CHANGELOG.md)
[![Architettura](https://img.shields.io/badge/architettura-Dual--ESP32-green)](docs/DUAL_ESP32_INTEGRATION.md)
[![Firmware](https://img.shields.io/badge/firmware-PlatformIO-orange)](firmware/esp32/platformio.ini)

Sistema di controllo DSP per impianti audio professionale basato su **architettura Dual-ESP32-S3** con catena **RS-485** verso **moduli amplificatori attivi PDA1001 con DSP CQ260D integrato**, controller DMX512 per luci, audio-reactive e autotune automatico.

## Architettura Dual-ESP32 (v2.0+)

Il sistema utilizza **due ESP32-S3** con ruoli distinti per superare i limiti GPIO e CPU di un singolo MCU:

```
┌─────────────────────────┐   I2S Condiviso   ┌──────────────────────────┐
│   ESP32 #1 (MASTER)     │◄─────────────────►│   ESP32 #2 (SLAVE)       │
│   UEDX80480050E-WB-A    │   UART IPC        │   ESP32-S3-WROOM1-N16R8  │
│   Display 5" 800×480    │◄─────────────────►│   Antenna IPEX 3dBi      │
│   ES8388 ADC/DAC        │   115200 baud      │   DMX512 @ 40Hz          │
│   RS-485 → CQ260D DSP   │   CRC8 frames      │   ESP-NOW wireless TX    │
│   WiFi AP + REST API    │                    │   NVS Storage            │
│   CPU ~60%              │                    │   CPU ~40%               │
└─────────────────────────┘                    └──────────────────────────┘
```

Vedere [`docs/DUAL_ESP32_INTEGRATION.md`](docs/DUAL_ESP32_INTEGRATION.md) per la guida completa.

## Funzionalità

- **Controllo DSP avanzato** per casse 2-vie, 3-vie e subwoofer (CQ260D)
- **Controller DMX512** per fixture PAR RGB, moving head, wash, strobe
- **Audio-reactive DMX**: sincronizzazione luci al segnale audio (6 bande FFT)
- **AutoTune automatico**: sweep + analisi FFT → correzione EQ e delay
- **AutoTune remoto**: microfono smartphone via WiFi come sorgente
- **FFT real-time** 32 bande logaritmiche 20Hz-20kHz (ESP-DSP library)
- **VU meter** L/R con picco dBFS
- **LED ring WS2812B** attorno agli encoder rotativi volume/balance (su Slave)
- **Mappa venue 2D** con calcolo automatico delay per allineamento temporale
- **WLED Neon LED**: strip WS2811 12V sulle casse via controller GLEDOPTO Wi-Fi
- **Display touch 5"** LVGL 800×480 con tab Home/Discovery/Assegnazione/Luci/DSP/Neon
- **App Flutter Android** per controllo da smartphone + tablet
- **Anteprima HTML** (`web/preview.html`) interfaccia completa standalone

## Struttura del Progetto

```
dsp-control/
├── README.md
├── ARCHITECTURE.md
├── CHANGELOG.md
├── PROJECT.md
├── web/
│   ├── preview.html
│   └── wled_preview.html
├── docs/
│   ├── DUAL_ESP32_INTEGRATION.md
│   ├── MASTER_SLAVE_ARCHITECTURE.md
│   ├── WIRING_GUIDE.md
│   ├── SCHEMATICS.md
│   ├── SCHEMATIC_DSP_CONNECTIONS.html
│   ├── CABLING_COMPLETE.md
│   ├── PINOUT_REFERENCE.md
│   ├── DATASHEETS_REFERENCE.md
│   ├── TESTING.md
│   ├── PROTOCOL_RS485_CQ260D.md
│   ├── hardware/README.md
│   └── ...
├── firmware/
│   ├── esp32/                    # ESP32 #1 Master
│   │   ├── platformio.ini
│   │   ├── README.md
│   │   └── src/
│   │       ├── main.cpp, config.h
│   │       ├── ipc_master, audio_mode, storage
│   │       ├── rs485, dsp_control, autotune
│   │       ├── wled_client, web_server
│   │       ├── drivers/, display/
│   │       └── ui/ (home, discovery, assignment, dmx, wled, dsp_advanced)
│   └── esp32_slave/              # ESP32 #2 Slave
│       ├── platformio.ini
│       ├── README.md
│       └── src/
│           ├── main.cpp, config.h
│           ├── ipc_slave, audio_slave, dmx512_slave
│           ├── wireless_slave, storage_slave
│           ├── relay_control, slave_peripherals
│           └── ...
└── app/flutter/
    ├── pubspec.yaml, README.md
    ├── lib/
    │   ├── main.dart
    │   ├── models/ (speaker, dsp_preset, dmx_*, venue_map, wled_*, ...)
    │   ├── services/ (api_client, audio_capture_service)
    │   └── screens/ (home, discovery, speaker_detail, dmx, dsp_advanced,
    │                  venue_map, autotune, wled, settings)
    └── android/
```

Riferimento pin: [`docs/PINOUT_REFERENCE.md`](docs/PINOUT_REFERENCE.md) e [`docs/MASTER_SLAVE_ARCHITECTURE.md`](docs/MASTER_SLAVE_ARCHITECTURE.md).

## Bill of Materials (BOM)

### Componenti Core Dual-ESP32

| Componente | Qt | Note |
|------------|----|------|
| ESP32-S3 Display Module UEDX80480050E-WB-A | 1 | Board Master (5" 800×480 RGB, 16MB Flash, 8MB PSRAM) |
| ESP32-S3-WROOM1-N16R8 Dev Board | 1 | Board Slave (16MB Flash, 8MB PSRAM, connettore IPEX) |
| Antenna 2.4GHz 3dBi IPEX (U.FL) | 1 | Per ESP32 #2 – range 150-300m |
| Cavo ribbon 7-pin 2.54mm 30cm | 1 | I2S (3) + UART IPC (2) + GND (1) + +5V opz (1) |
| Alimentatore AC/DC 5V (Mean Well IRM-30-5ST) | 1 | 5V 6A, alimentazione principale Master (indipendente da Vaux DSP) |
| Regolatore 3.3V (AMS1117-3.3) | 1 | Per alimentare ES8388 dal +5V (Master) |
| Filtro LC per codec | 1 | L=10µH + C=10µF//100n (pre/post) per ridurre disturbi su ES8388 |

### Componenti Audio/Controllo

| Componente | Qt | Note |
|------------|----|------|
| Codec ES8388 (I2S ADC/DAC) | 1 | I2C addr 0x10, 24bit, 48kHz |
| MAX485 o SN65HVD485 (RS-485) | 1 | Per DSP CQ260D su Master |
| MAX485 o SN65HVD485 (DMX) | 1 | Per DMX512 su Slave (3.3V) |
| GPIO Expander TCA9535 | 1 | I2C addr 0x20, relay/LED/TOUCH_RST su Master |
| Relay DPDT optoisolato | 1 | Via TCA9535 P0_0 (o su Slave GPIO10) |
| LED ring WS2812B 16 LED | 2 | Encoder volume/balance (su Slave) |
| Encoder rotativo | 2 | Volume e balance (su Slave) |
| Modulo PDA1001 con DSP CQ260D integrato | N | Uno per cassa amplificata |

Vedere [`docs/WIRING_GUIDE.md`](docs/WIRING_GUIDE.md) e [`docs/SCHEMATIC_DSP_CONNECTIONS.html`](docs/SCHEMATIC_DSP_CONNECTIONS.html) per collegamenti.

## Compilazione Firmware

### ESP32 #1 (Master)

```bash
cd firmware/esp32
pio run -e esp32s3
pio run -e esp32s3 --target upload
pio device monitor --baud 115200
```

### ESP32 #2 (Slave)

```bash
cd firmware/esp32_slave
pio run -e esp32s3
pio run -e esp32s3 --target upload
pio device monitor --baud 115200
```

> **Nota**: avviare ESP32 #2 (Slave) **prima** di ESP32 #1 (Master) per la sincronizzazione IPC.

Dopo il boot l'ESP32 #1 crea il WiFi AP `SISTEMA_AUDIO_01` (password: `audio1234`). REST API su `http://192.168.4.1`.

## Compilazione App Flutter

```bash
cd app/flutter
flutter pub get
flutter run
flutter build apk
```

## API REST principali

| Metodo | Endpoint | Descrizione |
|--------|----------|-------------|
| GET | `/api/audio/levels` | VU meter L/R + peak dBFS |
| GET | `/api/audio/spectrum` | FFT 32 bande |
| POST | `/api/autotune/start-remote` | AutoTune via microfono smartphone |
| POST | `/api/autotune/upload-fft` | Upload FFT dallo smartphone |
| GET | `/api/venue/map` | Mappa venue |
| POST | `/api/venue/calculate-delays` | Calcola delay da posizioni |
| GET | `/api/groups` | Gruppi DMX |
| GET | `/api/wled/controllers` | Lista controller WLED + stato |
| POST | `/api/wled/color` | Imposta colore zona WLED |
| POST | `/api/wled/effect` | Imposta effetto WLED |
| POST | `/api/wled/blackout` | Blackout tutti i controller WLED |
| POST | `/api/wled/discover` | Scopri controller WLED via UDP |

## Stato Sviluppo

| Componente              | Stato        |
|-------------------------|--------------|
| WiFi AP + REST API      | ✅ Completo  |
| Storage NVS (con IPC)   | ✅ Completo  |
| Audio relay DPDT        | ✅ Completo  |
| FFT real-time (ESP-DSP)| ✅ Completo  |
| VU meter L/R            | ✅ Completo  |
| DMX512 controller       | ✅ Completo  |
| Audio-reactive DMX     | ✅ Completo  |
| AutoTune locale/remoto  | ✅ Completo  |
| LED ring WS2812B        | ✅ Completo  |
| Encoder rotativi        | ✅ Completo (Slave + IPC) |
| Mappa venue 2D          | ✅ Completo  |
| App Flutter             | ✅ Completo  |
| WLED Neon LED client    | ✅ Completo  |
| RS-485 discovery        | ✅ Scan reale (dsp_connect) |
| DSP CQ260D protocollo   | ✅ Frame + SET_PARAM + gain/delay/preset |
| LVGL + LovyanGFX        | ✅ Driver display UEDX + GT911 |

## Documentazione

- **Architettura**: [`ARCHITECTURE.md`](ARCHITECTURE.md), [`docs/DUAL_ESP32_INTEGRATION.md`](docs/DUAL_ESP32_INTEGRATION.md), [`docs/MASTER_SLAVE_ARCHITECTURE.md`](docs/MASTER_SLAVE_ARCHITECTURE.md)
- **Cablaggio**: [`docs/WIRING_GUIDE.md`](docs/WIRING_GUIDE.md), [`docs/CABLING_COMPLETE.md`](docs/CABLING_COMPLETE.md), [`docs/SCHEMATICS.md`](docs/SCHEMATICS.md)
- **Pin e datasheet**: [`docs/PINOUT_REFERENCE.md`](docs/PINOUT_REFERENCE.md), [`docs/DATASHEETS_REFERENCE.md`](docs/DATASHEETS_REFERENCE.md)
- **Protocollo RS-485**: [`docs/PROTOCOL_RS485_CQ260D.md`](docs/PROTOCOL_RS485_CQ260D.md)
- **Test**: [`docs/TESTING.md`](docs/TESTING.md)
- **Riepilogo tecnico (italiano)**: [`docs/v2_Riepilogo.md`](docs/v2_Riepilogo.md)
