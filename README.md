# DSP Control – Sistema Audio PA Professionale

[![Versione](https://img.shields.io/badge/versione-2.0.0-blue)](CHANGELOG.md)
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
- **LED ring WS2812B** attorno agli encoder rotativi volume/balance
- **Mappa venue 2D** con calcolo automatico delay per allineamento temporale
- **WLED Neon LED**: strip WS2811 12V sulle casse via controller GLEDOPTO Wi-Fi
- **Display touch 5"** LVGL 800×480 con tab Home/Discovery/Assegnazione/Luci/DSP/Neon
- **App Flutter Android** per controllo da smartphone + tablet
- **Anteprima HTML** (`web/preview.html`) interfaccia completa standalone

## Struttura del Progetto

```
dsp-control/
├── README.md
├── ARCHITECTURE.md          ← Architettura sistema (diagrammi dual-ESP32)
├── CHANGELOG.md
├── web/
│   ├── preview.html         ← Anteprima HTML interfaccia completa (standalone)
│   └── wled_preview.html    ← Anteprima HTML interfaccia WLED Neon LED (4CH)
├── docs/
│   ├── DUAL_ESP32_INTEGRATION.md  ← Guida integrazione dual-ESP32 (NUOVO)
│   ├── WIRING_GUIDE.md            ← Cablaggio fisico + BOM
│   ├── SCHEMATICS.md              ← Indice schemi + Mermaid
│   ├── CABLING_COMPLETE.md        ← Cablaggio completo (aggiornato ai PDF)
│   ├── DATASHEETS_REFERENCE.md   ← Sintesi UEDX, M144, Pololu, relè, encoder…
│   ├── hardware/README.md         ← Elenco PDF
│   ├── schematics_print.html      ← Stampa PDF schemi base
│   ├── schematics_print_full.html ← Stampa PDF completo UEDX+Slave+RS485+DMX
│   ├── MASTER_SLAVE_ARCHITECTURE.md ← Slave: encoder, LED, relay, IPC
│   ├── schematics_electric.html     ← Schema elettrico stile CAD (MAX485, ES8388, …)
│   ├── schematics_graphic.html      ← Schemi a blocchi SVG
│   ├── schematics_print_system.html ← Stampa sistema testuale
│   ├── UEDX_V33_FREE_PINS.md
│   ├── UEDX_V33_PINOUT_PLAN.md    ← Piano + Slave
│   ├── PINOUT_REFERENCE.md        ← Tabelle pinout complete
│   ├── TESTING.md                 ← Procedure test IPC/I2S/DMX (NUOVO)
│   └── ...altri docs...
├── firmware/
│   ├── esp32/               ← ESP32 #1 (Master: Display + Audio + RS-485 + WiFi)
│   │   ├── platformio.ini
│   │   ├── README.md
│   │   └── src/
│   │       ├── main.cpp
│   │       ├── config.h             ← pin GPIO Master + IPC + I2S + RS-485
│   │       ├── ipc_master.h/cpp     ← Protocollo IPC verso Slave (NUOVO)
│   │       ├── audio_mode.h/cpp     ← I2S Master (genera BCLK/WS) + FFT 32 bande
│   │       ├── storage.h/cpp        ← Storage con IPC delegation + fallback NVS
│   │       └── ...altri moduli...
│   └── esp32_slave/         ← ESP32 #2 (Slave: DMX + Wireless + Storage) (NUOVO)
│       ├── platformio.ini
│       ├── README.md
│       └── src/
│           ├── main.cpp
│           ├── config.h             ← pin GPIO Slave
│           ├── ipc_slave.h/cpp      ← Ricevitore comandi IPC
│           ├── audio_slave.h/cpp    ← I2S Slave RX + FFT 6 bande
│           ├── dmx512_slave.h/cpp   ← DMX512 output 40Hz
│           ├── wireless_slave.h/cpp ← ESP-NOW TX broadcast
│           ├── storage_slave.h/cpp  ← NVS storage completo
│           └── relay_control.h/cpp  ← Relay + strobo audio-reactive
└── app/
    └── flutter/
        └── ...
```


└── app/
    └── flutter/
        ├── pubspec.yaml
        ├── README.md
        └── lib/
            ├── main.dart
            ├── models/
            │   ├── speaker.dart
            │   ├── dsp_preset.dart
            │   ├── dmx_fixture.dart
            │   ├── dmx_scene.dart
            │   ├── audio_reactive_config.dart
            │   ├── venue_map.dart        ← modello mappa venue 2D
            │   ├── wled_controller.dart  ← modello controller WLED
            │   └── wled_scene.dart       ← modello scena WLED
            ├── services/api_client.dart
            └── screens/
                ├── home_screen.dart      ← VU meter L/R real-time
                ├── discovery_screen.dart
                ├── speaker_detail_screen.dart
                ├── dmx_screen.dart
                ├── dsp_advanced_screen.dart
                ├── venue_map_screen.dart ← mappa 2D con calcolo delay
                ├── autotune_screen.dart  ← autotune locale/smartphone
                ├── wled_screen.dart      ← controllo neon LED WLED
                └── settings_screen.dart
```

## Bill of Materials (BOM)

### Componenti Core Dual-ESP32

| Componente | Qt | Note |
|------------|----|------|
| ESP32-S3 Display Module UEDX80480050E-WB-A | 1 | Board Master (5" 800×480 RGB, 16MB Flash, 8MB PSRAM) |
| ESP32-S3-WROOM1-N16R8 Dev Board | 1 | Board Slave (16MB Flash, 8MB PSRAM, connettore IPEX) |
| Antenna 2.4GHz 3dBi IPEX (U.FL) | 1 | Per ESP32 #2 – range 150-300m |
| Cavo ribbon 7-pin 2.54mm 30cm | 1 | I2S (3) + UART IPC (2) + GND (1) + +5V opz (1) |
| Alimentatore 5V 2A | 1 | O 2× alimentatori 5V 1A con GND comune |

### Componenti Audio/Controllo

| Componente | Qt | Note |
|------------|----|------|
| Codec ES8388 (I2S ADC/DAC) | 1 | I2C addr 0x10, 24bit, 48kHz |
| MAX485 o SN65HVD485 (RS-485) | 1 | Per DSP CQ260D su Master |
| MAX485 o SN65HVD485 (DMX) | 1 | Per DMX512 su Slave (usare versione 3.3V) |
| GPIO Expander TCA9535 | 1 | I2C addr 0x20, per relay/LED/TOUCH_RST su Master |
| Relay DPDT optoisolato | 1 | Collegato a TCA9535 P0_0 |
| LED ring WS2812B 16 LED | 2 | Encoder volume/balance |
| Encoder rotativo | 2 | Volume e balance |
| Modulo PDA1001 con DSP CQ260D integrato | N | Uno per cassa amplificata |

Vedere [`docs/WIRING_GUIDE.md`](docs/WIRING_GUIDE.md) per schema completo e collegamenti.

## Compilazione Firmware

### ESP32 #1 (Master)

```bash
cd firmware/esp32
pio run -e esp32s3
pio run -e esp32s3 --target upload
pio device monitor --baud 115200
```

### ESP32 #2 (Slave) – NUOVO

```bash
cd firmware/esp32_slave
pio run -e esp32s3
pio run -e esp32s3 --target upload
pio device monitor --baud 115200
```

> **Nota**: avviare ESP32 #2 (Slave) **prima** di ESP32 #1 (Master) per la sincronizzazione IPC.

Dopo il boot l'ESP32 #1 crea il WiFi AP `SISTEMA_AUDIO_01` (password: `audio1234`).
REST API disponibile su `http://192.168.4.1`.

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
| Storage NVS persistenza | ✅ Completo  |
| Audio relay DPDT        | ✅ Completo  |
| FFT real-time (ESP-DSP) | ✅ Completo  |
| VU meter L/R            | ✅ Completo  |
| DMX512 controller       | ✅ Completo  |
| Audio-reactive DMX      | ✅ Completo  |
| AutoTune locale         | ✅ Completo  |
| AutoTune remoto         | ✅ Completo  |
| LED ring WS2812B        | ✅ Completo  |
| Encoder rotativi        | ✅ Completo  |
| Mappa venue 2D          | ✅ Completo  |
| App Flutter             | ✅ Completo  |
| WLED Neon LED client    | ✅ Completo  |
| RS-485 discovery        | ✅ Scan reale (dsp_connect) |
| DSP CQ260D protocollo   | ✅ Frame + SET_PARAM + identif. gain |
| LVGL + LovyanGFX        | ✅ Driver display (vedi firmware) |


Sistema di controllo DSP per impianti audio professionale basato su **ESP32-S3** con catena **RS-485** verso **moduli amplificatori attivi PDA1001 con DSP CQ260D integrato**.

## Panoramica

Il sistema permette di gestire da un box di controllo centrale (o da smartphone Android) un impianto audio PA multi-cassa con:
- **Controllo volume** master e per singola cassa
- **Discovery automatica** dei moduli collegati via RS-485
- **Identificazione sonora** delle casse (beep pattern)
- **Due modalità audio**: MixerPassThrough (relay diretto) e TestTone (DAC ES8388)
- **Preset DSP** per casse 2-vie, 3-vie e subwoofer
- **Display touch 5"** LVGL adattivo (mostra solo le casse connesse)
- **App Flutter Android** per controllo da smartphone

## Struttura del Progetto

```
dsp-control/
├── README.md                        ← questo file
├── docs/
│   └── v2_Riepilogo.md              ← documentazione tecnica completa (italiano)
├── firmware/
│   └── esp32/                       ← progetto PlatformIO per ESP32-S3
│       ├── platformio.ini
│       ├── README.md
│       └── src/
│           ├── main.cpp
│           ├── config.h             ← pin e costanti
│           ├── audio_mode.h/cpp     ← I2S ES8388 + relay DPDT
│           ├── rs485.h/cpp          ← comunicazione RS-485 + discovery
│           ├── storage.h/cpp        ← persistenza NVS (Preferences)
│           ├── dsp_control.h/cpp    ← controllo DSP CQ260D (stub)
│           ├── web_server.h/cpp     ← WiFi AP + REST API
│           └── ui/
│               ├── ui_home.h/cpp        ← schermata home LVGL
│               ├── ui_discovery.h/cpp   ← schermata discovery LVGL
│               └── ui_assignment.h/cpp  ← schermata assegnazione LVGL
└── app/
    └── flutter/                     ← app Android Flutter
        ├── pubspec.yaml
        ├── README.md
        ├── lib/
        │   ├── main.dart
        │   ├── models/speaker.dart
        │   ├── services/api_client.dart
        │   └── screens/
        │       ├── home_screen.dart
        │       ├── discovery_screen.dart
        │       └── speaker_detail_screen.dart
        └── android/
            └── app/
                ├── build.gradle
                └── src/main/AndroidManifest.xml
```

## Hardware Richiesto

| Componente                    | Quantità |
|-------------------------------|----------|
| ESP32-S3-DevKitC-1            | 1        |
| Display 5" 800×480 touch SPI  | 1        |
| Codec M5Stack ES8388 (I2S)    | 1        |
| MAX485 breakout (RS-485)      | 1        |
| Relay DPDT optoisolato        | 1        |
| Modulo PDA1001 con DSP CQ260D integrato | N (uno per cassa) |

## Come Compilare il Firmware

```bash
cd firmware/esp32

# Compilazione
pio run -e esp32s3

# Flash (USB collegato)
pio run -e esp32s3 --target upload

# Monitor seriale
pio device monitor --baud 115200
```

Dopo il boot l'ESP32 crea il WiFi AP `SISTEMA_AUDIO_01` (password: `audio1234`).  
Interfaccia web disponibile su `http://192.168.4.1`.

## Come Compilare l'App Flutter

```bash
cd app/flutter

flutter pub get
flutter run          # su dispositivo Android collegato
flutter build apk    # genera APK release
```

## Documentazione Tecnica

Vedere [`docs/v2_Riepilogo.md`](docs/v2_Riepilogo.md) per:
- Architettura completa del sistema
- Dettaglio driver acustici e volumi casse
- Preset DSP disponibili
- Protocollo RS-485 e modello dati
- Costi approssimativi e materiali
- Note di sviluppo e TODO

## Stato Sviluppo

| Componente          | Stato        |
|---------------------|--------------|
| WiFi AP + REST API  | ✅ Completo  |
| Storage NVS         | ✅ Completo  |
| Audio relay DPDT    | ✅ Completo  |
| I2S / Test tone     | ⚠️ Parziale  |
| RS-485 discovery    | 🔶 Stub      |
| DSP CQ260D          | 🔶 Stub      |
| LVGL display        | 🔶 Stub      |
| App Flutter         | ✅ Completo  |
Modulo segnale audio dsp volume remote control
