# DSP Control – Sistema Audio PA Professionale

Sistema di controllo DSP per impianti audio professionale basato su **ESP32-S3** con catena **RS-485** verso moduli amplificati **PDA1001 + CQ260D**.

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
| Modulo PDA1001 + CQ260D       | N (uno per cassa) |

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
