# App Flutter – DSP Control

App Android/iOS per il controllo del sistema audio PA via WiFi, comunicando con il firmware ESP32-S3.

## Prerequisiti

- [Flutter SDK](https://docs.flutter.dev/get-started/install) ≥ 3.0
- Android SDK (API 21+)
- Dispositivo Android/iOS o emulatore

## Compilazione e Run

```bash
cd app/flutter

# Installa dipendenze
flutter pub get

# Esegui su dispositivo/emulatore collegato
flutter run

# Build APK release
flutter build apk --release
# APK: build/app/outputs/flutter-apk/app-release.apk
```

## Configurazione connessione

1. Connettere il telefono al WiFi `SISTEMA_AUDIO_01` (password: `audio1234`)
2. Aprire l'app e confermare l'IP (default: `http://192.168.4.1`)
3. Premere **CONNETTI**

## Struttura sorgenti

```
lib/
├── main.dart                        – Entry point, navigazione (6 tab)
├── models/
│   ├── speaker.dart                 – Modello cassa (Speaker)
│   ├── dsp_preset.dart              – Preset DSP
│   ├── dmx_fixture.dart             – Fixture DMX
│   ├── dmx_scene.dart               – Scena DMX
│   ├── audio_reactive_config.dart   – Config audio-reactive
│   ├── venue_map.dart               – Mappa venue 2D con elementi
│   ├── wled_controller.dart         – Modello controller WLED
│   └── wled_scene.dart              – Modello scena WLED
├── services/
│   └── api_client.dart              – Client HTTP REST per ESP32
└── screens/
    ├── home_screen.dart             – Volume master + VU meter L/R + lista casse
    ├── discovery_screen.dart        – Scansione RS-485
    ├── speaker_detail_screen.dart   – Dettaglio cassa + DSP
    ├── dmx_screen.dart              – Controllo luci DMX
    ├── dsp_advanced_screen.dart     – EQ, compressor, noise gate
    ├── venue_map_screen.dart        – Mappa 2D venue + calcolo delay
    ├── autotune_screen.dart         – AutoTune locale/smartphone
    ├── wled_screen.dart             – Controllo neon LED WLED
    └── settings_screen.dart         – Impostazioni
```

## Funzionalità principali

### VU Meter (Home Screen)
Il VU meter aggiorna i livelli audio L/R ogni 100ms tramite `/api/audio/levels`.
Visualizza barre con cambio colore: verde → arancio → rosso.

### Mappa Venue
- Canvas 2D con griglia metrica
- Aggiunta elementi (casse, PAR, moving head) via tap
- Calcolo automatico dei delay di allineamento temporale (dist/343*1000ms)
- Salvataggio su ESP32 via `/api/venue/map`

### AutoTune
- **Locale**: usa ADC ES8388 sul modulo ESP32
- **Remoto**: usa il microfono dello smartphone via WiFi
- Polling stato ogni 500ms, applicazione correzioni EQ+delay

## Dipendenze

| Pacchetto         | Versione | Uso                             |
|-------------------|----------|---------------------------------|
| http              | ^1.1.0   | Client HTTP                     |
| provider          | ^6.1.1   | State management                |
| shared_preferences| ^2.2.2   | Salvataggio IP                  |
| record            | ^5.0.0   | Registrazione microfono         |
| path_provider     | ^2.1.0   | Percorsi file                   |

## API REST utilizzate

| Endpoint                        | Metodo | Descrizione                      |
|---------------------------------|--------|----------------------------------|
| `/api/devices`                  | GET    | Lista casse                      |
| `/api/audio/levels`             | GET    | VU meter L/R + peak dBFS         |
| `/api/audio/spectrum`           | GET    | FFT 32 bande                     |
| `/api/autotune/start-remote`    | POST   | AutoTune via smartphone          |
| `/api/autotune/upload-fft`      | POST   | Upload FFT smartphone            |
| `/api/venue/map`                | GET/POST| Mappa venue                    |
| `/api/venue/calculate-delays`   | POST   | Calcola delay da posizioni       |
| `/api/groups`                   | GET    | Gruppi DMX                       |
| `/api/dsp/autotune/start`       | POST   | AutoTune locale                  |
| `/api/dsp/autotune/status`      | GET    | Stato autotune                   |
| `/api/wled/controllers`         | GET    | Lista controller WLED + stato    |
| `/api/wled/color`               | POST   | Imposta colore zona              |
| `/api/wled/effect`              | POST   | Imposta effetto zona             |
| `/api/wled/brightness`          | POST   | Imposta luminosità zona          |
| `/api/wled/scenes`              | GET/POST| Scene WLED                      |
| `/api/wled/scenes/apply`        | POST   | Applica scena WLED               |
| `/api/wled/sync`                | POST   | Sincronizza tutti i controller   |
| `/api/wled/blackout`            | POST   | Blackout tutti i controller      |
| `/api/wled/discover`            | POST   | Scopri controller via UDP        |


App Android per il controllo del sistema audio PA via WiFi, comunicando con il firmware ESP32-S3.

## Prerequisiti

- [Flutter SDK](https://docs.flutter.dev/get-started/install) ≥ 3.0
- Android SDK (API 21+)
- Dispositivo Android o emulatore

## Compilazione e Run

```bash
cd app/flutter

# Installa dipendenze
flutter pub get

# Esegui su dispositivo/emulatore collegato
flutter run

# Build APK release
flutter build apk --release
# APK generato in: build/app/outputs/flutter-apk/app-release.apk
```

## Configurazione connessione

All'avvio l'app mostra una schermata di configurazione dove inserire l'IP dell'ESP32.

**Default**: `http://192.168.4.1`

Per connettersi:
1. Connettere il telefono al WiFi `SISTEMA_AUDIO_01` (password: `audio1234`)
2. Aprire l'app e confermare l'IP (o lasciare il default)
3. Premere **CONNETTI**

## Struttura sorgenti

```
lib/
├── main.dart                        – Entry point, configurazione IP
├── models/
│   └── speaker.dart                 – Modello dati cassa (Speaker)
├── services/
│   └── api_client.dart              – Client HTTP REST per ESP32
└── screens/
    ├── home_screen.dart             – Volume master + lista casse
    ├── discovery_screen.dart        – Scansione RS-485
    └── speaker_detail_screen.dart   – Dettaglio singola cassa
```

## API REST utilizzate

| Endpoint              | Metodo | Descrizione                    |
|-----------------------|--------|--------------------------------|
| `/api/devices`        | GET    | Lista casse assegnate          |
| `/api/audio-mode`     | POST   | Cambia modalità audio          |
| `/api/master-volume`  | POST   | Imposta volume master          |
| `/api/speaker-volume` | POST   | Imposta volume singola cassa   |
| `/api/discovery`      | POST   | Avvia scansione RS-485         |

## Note

- L'app usa `SharedPreferences` per ricordare l'IP dell'ESP32 tra le sessioni
- L'IP viene salvato automaticamente alla connessione
- La discovery RS-485 può richiedere fino a 30 secondi (timeout configurabile in `api_client.dart`)
