# App Flutter – DSP Control

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
