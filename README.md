# DSP Control – Sistema Audio PA Professionale

Sistema di controllo DSP per impianti audio professionale basato su **ESP32-S3** con catena **RS-485** verso **moduli amplificatori attivi PDA1001 con DSP CQ260D integrato**, controller DMX512 per luci, audio-reactive e autotune automatico.

## Panoramica

Il sistema gestisce un impianto audio PA multi-cassa + illuminazione scenica da un box di controllo centrale o da smartphone Android:

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
├── web/
│   ├── preview.html         ← Anteprima HTML interfaccia completa (standalone)
│   └── wled_preview.html    ← Anteprima HTML interfaccia WLED Neon LED (4CH)
├── docs/
│   └── v2_Riepilogo.md
├── firmware/
│   └── esp32/
│       ├── platformio.ini
│       ├── README.md
│       └── src/
│           ├── main.cpp
│           ├── config.h             ← pin, costanti, GPIO encoder/LED ring
│           ├── audio_mode.h/cpp     ← I2S ES8388 + FFT reale + VU meter
│           ├── audio_reactive.h/cpp ← audio-reactive DMX (6 bande)
│           ├── rs485.h/cpp          ← RS-485 discovery/controllo
│           ├── storage.h/cpp        ← persistenza NVS (preset, fixture, scene, WLED)
│           ├── dsp_control.h/cpp    ← controllo DSP CQ260D
│           ├── dmx512.h/cpp         ← controller DMX512
│           ├── autotune.h/cpp       ← autotune locale + remoto smartphone
│           ├── led_ring.h/cpp       ← LED ring WS2812B + encoder rotativi
│           ├── wled_client.h/cpp    ← WLED neon LED client Wi-Fi
│           ├── web_server.h/cpp     ← WiFi AP + REST API
│           └── ui/
│               ├── ui_home.h/cpp
│               ├── ui_discovery.h/cpp
│               ├── ui_assignment.h/cpp
│               ├── ui_dmx.h/cpp         ← tab DMX LVGL
│               ├── ui_wled.h/cpp        ← tab Neon WLED LVGL
│               └── ui_dsp_advanced.h/cpp ← tab DSP avanzato + VU meter LVGL
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

## Hardware Richiesto

| Componente                    | Quantità |
|-------------------------------|----------|
| ESP32-S3-DevKitC-1            | 1        |
| Display 5" 800×480 touch SPI  | 1        |
| Codec M5Stack ES8388 (I2S)    | 1        |
| MAX485 breakout (RS-485)      | 1        |
| Relay DPDT optoisolato        | 1        |
| LED ring WS2812B 16 LED       | 2        |
| Encoder rotativo              | 2        |
| Modulo PDA1001 con DSP CQ260D integrato | N (uno per cassa) |

## Compilazione Firmware

```bash
cd firmware/esp32
pio run -e esp32s3
pio run -e esp32s3 --target upload
pio device monitor --baud 115200
```

Dopo il boot l'ESP32 crea il WiFi AP `SISTEMA_AUDIO_01` (password: `audio1234`).
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
| RS-485 discovery        | 🔶 Stub      |
| DSP CQ260D protocollo   | 🔶 Parziale  |
| LVGL driver display     | 🔶 Stub      |


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
