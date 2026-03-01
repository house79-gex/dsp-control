# DSP Control – Documentazione Sistema Completa

Sistema di controllo audio PA professionale + controller DMX512 basato su **ESP32-S3**  
Versione: 2.0 | Lingua: Italiano

---

## 1. Architettura Hardware

### 1.1 Componenti Principali

| Componente       | Modello         | Interfaccia | Funzione                              |
|------------------|-----------------|-------------|---------------------------------------|
| Microcontroller  | ESP32-S3        | –           | Elaborazione centrale, WiFi, USB OTG  |
| Codec Audio      | ES8388          | I2S + I2C   | ADC/DAC stereo 24-bit 48kHz           |
| Transceiver RS485| MAX485 / equiv. | UART2       | Comunicazione con DSP CQ260D          |
| Amplificatore DSP| CQ260D          | RS-485      | DSP + amplificazione 260W             |
| Modulo Amplif.   | PDA1001         | RS-485      | Modulo amplificato passivo            |
| Display          | TFT 800×480     | SPI         | Interfaccia locale LVGL               |
| Touch            | GT911 / FT6236  | I2C         | Touch capacitivo                      |
| Relay DPDT       | Optoisolato     | GPIO        | Commutazione TestTone / Passthrough   |
| Transceiver DMX  | MAX485 / equiv. | UART1       | Output DMX512-A (250kbaud)            |

### 1.2 Configurazione Pin ESP32-S3

| Pin     | GPIO | Funzione                          |
|---------|------|-----------------------------------|
| DMX_TX  | 1    | UART1 TX – Output DMX512          |
| DMX_DE  | 2    | Driver Enable DMX (sempre HIGH)   |
| RS485_TX| 43   | UART2 TX – RS-485 DSP             |
| RS485_RX| 44   | UART2 RX – RS-485 DSP             |
| RS485_DE| 21   | Driver Enable RS-485              |
| RS485_RE| 20   | Receiver Enable RS-485            |
| RELAY   | 45   | Relay DPDT (HIGH=TestTone)        |
| LED     | 48   | LED di stato integrato            |
| I2S_SCK | 15   | ES8388 – Bit Clock                |
| I2S_WS  | 16   | ES8388 – Word Select              |
| I2S_OUT | 17   | ES8388 – DAC output               |
| I2S_IN  | 18   | ES8388 – ADC input                |

---

## 2. Architettura Firmware

### 2.1 Moduli

| Modulo               | File                   | Descrizione                                    |
|----------------------|------------------------|------------------------------------------------|
| Protocollo DSP       | `dsp_protocol.h/cpp`   | Frame builder, state machine RX, checksum     |
| Registri DSP         | `dsp_registers.h`      | Struct SYSREG completa CQ260D                  |
| Controllo DSP        | `dsp_control.h/cpp`    | Preset, parametri, modalità esperto/base       |
| Controller DMX       | `dmx512.h/cpp`         | Output DMX512-A, scene, fixture, gruppi        |
| Audio-Reactive       | `audio_reactive.h/cpp` | FFT bande, scenari, blending DMX               |
| AutoTune             | `autotune.h/cpp`       | Calibrazione automatica risposta in frequenza  |
| USB Storage          | `usb_storage.h/cpp`    | Import/export configurazione via USB-C         |
| Web Server           | `web_server.h/cpp`     | WiFi AP + REST API completa                    |
| Storage NVS          | `storage.h/cpp`        | Persistenza assegnazioni su NVS                |
| WLED Client          | `wled_client.h/cpp`    | Controllo strip neon LED via Wi-Fi (HTTP JSON) |
| Audio Mode           | `audio_mode.h/cpp`     | ES8388 I2S, relay, FFT stub                    |
| RS-485               | `rs485.h/cpp`          | Driver UART RS-485, TX/RX                      |
| Config               | `config.h`             | Pin e costanti di sistema                      |
| Main                 | `main.cpp`             | Setup + loop + task FreeRTOS                   |

### 2.2 Task FreeRTOS

| Task               | Core | Priorità | Periodo   | Funzione                            |
|--------------------|------|----------|-----------|-------------------------------------|
| `dmx_task`         | 1    | 10       | 25ms      | Output DMX512 a 40Hz                |
| `audio_fft_task`   | 0    | 5        | 25ms      | Elaborazione FFT audio-reactive     |
| `dsp_heartbeat`    | 1    | 3        | 10ms      | Heartbeat DSP + RX RS-485           |
| `autotune_task`    | 0    | 2        | 100ms     | State machine calibrazione          |
| `wled_health`      | 0    | 2        | 5000ms    | Health-check controller WLED Wi-Fi  |
| Loop Arduino       | 1    | 1        | 5ms       | LVGL + USB + TestTone               |

### 2.3 Flusso Dati

```
ES8388 ADC
    │
    ▼ (I2S DMA)
audio_fft_task ──→ FFT bande ──→ audio_reactive_process() ──→ dmx_set_fixture_color()
                                                                       │
                                                               dmx_task (UART1)
                                                                       │
                                                               DMX512 Buffer ──→ Fixture luci

RS-485 (UART2)
    │
    ▼
dsp_heartbeat_task ──→ dsp_rx_process_byte() ──→ dsp_uctrl_cmd_processor()
                                                         │
                                                    SysReg aggiornato

REST API ──→ dsp_set_*() ──→ dsp_command_send() ──→ RS-485 TX
         └──→ dmx_*()    ──→ dmx_buffer           ──→ UART1 TX
```

---

## 3. Architettura App Flutter

### 3.1 Struttura Directory

```
lib/
├── main.dart                        – Entry point + navigazione responsive
├── models/
│   ├── speaker.dart                 – Modello cassa DSP
│   ├── dmx_fixture.dart             – Modello fixture DMX
│   ├── dmx_scene.dart               – Modello scena DMX
│   ├── dsp_preset.dart              – Modello preset DSP
│   └── audio_reactive_config.dart   – Modello configurazione audio-reactive
├── services/
│   └── api_client.dart              – Client REST completo
└── screens/
    ├── home_screen.dart             – Tab Audio (casse, volume, DSP base)
    ├── speaker_detail_screen.dart   – Dettaglio cassa singola
    ├── discovery_screen.dart        – Discovery RS-485
    ├── dmx_screen.dart              – Tab Luci (fixture, scene, audio-reactive)
    ├── dsp_advanced_screen.dart     – DSP avanzato (EQ, compressore, filtri)
    └── settings_screen.dart         – Impostazioni (WiFi, modalità, USB)
```

### 3.2 Layout Responsive

| Breakpoint | Layout                    | Navigazione           |
|------------|---------------------------|------------------------|
| < 600dp    | Smartphone portrait/landscape | BottomNavigationBar |
| ≥ 600dp    | Tablet / landscape        | NavigationRail (lateral)|

---

## 4. API REST – Riferimento Completo

### Base URL: `http://192.168.4.1`

### 4.1 Endpoint Originali

| Metodo | Endpoint             | Body / Params                    | Risposta              |
|--------|---------------------|----------------------------------|-----------------------|
| GET    | `/api/devices`       | –                                | Array casse           |
| POST   | `/api/audio-mode`    | `{mode: "mixer"\|"test"}`        | `{ok:true}`           |
| POST   | `/api/master-volume` | `{value: 0-100}`                 | `{ok:true}`           |
| POST   | `/api/speaker-volume`| `{id, value: 0-100}`             | `{ok:true}`           |
| POST   | `/api/discovery`     | –                                | `{devices: [ids]}`    |

### 4.2 DSP Avanzato

| Metodo | Endpoint                    | Body                                   |
|--------|-----------------------------|----------------------------------------|
| GET    | `/api/dsp/status`           | –                                      |
| GET    | `/api/dsp/levels`           | –                                      |
| GET    | `/api/dsp/presets`          | –                                      |
| POST   | `/api/dsp/preset/apply`     | `{name, deviceId?}`                   |
| POST   | `/api/dsp/preset/save`      | `{name, description}`                  |
| DELETE | `/api/dsp/preset`           | `{name}`                               |
| POST   | `/api/dsp/gain`             | `{id, value_db}`                       |
| POST   | `/api/dsp/hpf`              | `{id, freq, slope}`                    |
| POST   | `/api/dsp/lpf`              | `{id, freq, slope}`                    |
| POST   | `/api/dsp/delay`            | `{id, delay_ms}`                       |
| POST   | `/api/dsp/mute`             | `{id, muted}`                          |
| POST   | `/api/dsp/phase`            | `{id, inverted}`                       |
| POST   | `/api/dsp/eq`               | `{id, band, freq, gain, q, type}`      |
| POST   | `/api/dsp/compressor`       | `{id, threshold, ratio, attack, release, makeup}` |
| POST   | `/api/dsp/noise-gate`       | `{id, threshold, attack, decay}`       |
| POST   | `/api/dsp/group-control`    | `{groupType: "SX"\|"DX"\|"SUB"\|"ALL", param, value}` |

### 4.3 AutoTune

| Metodo | Endpoint                        | Body / Risposta                        |
|--------|---------------------------------|----------------------------------------|
| POST   | `/api/dsp/autotune/start`       | `{mode: "single"\|"all", targetId?}`  |
| GET    | `/api/dsp/autotune/status`      | `{state, progress, message}`          |
| POST   | `/api/dsp/autotune/apply`       | –                                      |
| POST   | `/api/dsp/autotune/cancel`      | –                                      |

### 4.4 DMX

| Metodo | Endpoint                              | Body                                  |
|--------|---------------------------------------|---------------------------------------|
| GET    | `/api/dmx/fixtures`                   | –                                     |
| POST   | `/api/dmx/fixture`                    | Oggetto DmxFixture                    |
| DELETE | `/api/dmx/fixture`                    | `{id}`                                |
| POST   | `/api/dmx/fixture/color`              | `{fixtureId, r, g, b, w?}`           |
| POST   | `/api/dmx/fixture/dimmer`             | `{fixtureId, value}`                  |
| POST   | `/api/dmx/fixture/channel`            | `{fixtureId, channel, value}`         |
| POST   | `/api/dmx/blackout`                   | –                                     |
| POST   | `/api/dmx/master-dimmer`              | `{value: 0-255}`                      |
| GET    | `/api/dmx/groups`                     | –                                     |
| POST   | `/api/dmx/group/control`              | `{groupId, r, g, b, w?, dimmer?}`    |
| GET    | `/api/dmx/scenes`                     | –                                     |
| POST   | `/api/dmx/scene/play`                 | `{sceneId}`                           |
| POST   | `/api/dmx/scene/stop`                 | –                                     |
| POST   | `/api/dmx/scene/save`                 | Oggetto DmxScene                      |
| DELETE | `/api/dmx/scene`                      | `{sceneId}`                           |

### 4.5 Audio-Reactive

| Metodo | Endpoint                                   | Body / Risposta                     |
|--------|--------------------------------------------|-------------------------------------|
| GET    | `/api/dmx/audio-reactive/status`           | `{enabled, scenarioId, bandLevels}` |
| POST   | `/api/dmx/audio-reactive/enable`           | `{enabled, scenarioId?}`            |
| GET    | `/api/dmx/audio-reactive/scenarios`        | Array scenari                       |
| GET    | `/api/dmx/audio-reactive/spectrum`         | `{bands: [0.0-1.0 × 6]}`           |

### 4.6 USB Import/Export

| Metodo | Endpoint              | Descrizione                        |
|--------|-----------------------|------------------------------------|
| GET    | `/api/export/config`  | Esporta configurazione completa    |
| POST   | `/api/import/config`  | Importa configurazione             |
| GET    | `/api/export/presets` | Esporta scenari audio-reactive     |
| GET    | `/api/export/scenes`  | Esporta fixture DMX                |

### 4.7 Sistema

| Metodo | Endpoint              | Body / Risposta                                      |
|--------|-----------------------|------------------------------------------------------|
| GET    | `/api/system/info`    | `{firmware_version, uptime_ms, free_heap, chip_model, dsp_connected}` |
| POST   | `/api/system/mode`    | `{mode: "base"\|"expert", password?}`               |
| GET    | `/api/system/usb-status` | `{connected}`                                    |

---

## 5. Guida Utente – Modalità Base

La modalità Base è pensata per l'utilizzo quotidiano senza configurazioni avanzate.

### 5.1 Controllo Volume
1. Apri l'app e connettiti all'ESP32 (rete WiFi `SISTEMA_AUDIO_01`, password `audio1234`)
2. Lo slider **Volume Master** controlla il volume globale dell'impianto
3. Ogni cassa ha il suo slider di volume individuale

### 5.2 Applicare un Preset Audio
1. Vai su **Tab Audio → Preset**
2. Seleziona il preset desiderato dalla lista (es. `2WAY_STD`, `CONCERT`)
3. Premi **Applica** – il preset viene inviato a tutte le casse

### 5.3 Controllo Luci (Base)
1. Vai su **Tab Luci**
2. Usa lo slider **Master Dimmer** per la luminosità globale
3. Tappa su una fixture per cambiarne il colore
4. Seleziona una scena dalla lista e premi ▶ per attivarla
5. Premi **BLACKOUT** (pulsante rosso) per spegnere tutto istantaneamente

---

## 6. Guida Utente – Modalità Esperto

La modalità Esperto sblocca tutti i parametri avanzati DSP e DMX.

### 6.1 Attivazione
1. Vai su **Impostazioni → Modalità Utente**
2. Attiva lo switch **Esperto**
3. Inserisci la password (lascia vuoto se non configurata)

### 6.2 EQ Parametrico
1. In **Tab Audio**, tappa su una cassa per aprire il **Dettaglio Cassa**
2. Vai su **DSP Avanzato**
3. Regola le bande EQ con gli slider (gain in dB per banda)
4. Le modifiche vengono inviate al DSP in tempo reale

### 6.3 Filtri HPF/LPF
- **HPF** (High-Pass Filter): elimina le frequenze sotto la soglia (evita distorsione nei bassi)
- **LPF** (Low-Pass Filter): elimina le frequenze sopra la soglia (per subwoofer)
- Pendenza: 12 / 24 / 48 dB/ottava

### 6.4 Compressore
Parametri disponibili:
- **Soglia** (Threshold): livello sopra cui agisce il compressore
- **Ratio**: rapporto di compressione (2:1 = soft, 20:1 = hard limiting)
- **Attack**: velocità di risposta al segnale (ms)
- **Release**: velocità di ritorno al normale (ms)
- **Makeup**: recupero del gain perso dalla compressione (dB)

### 6.5 Delay di Allineamento
Il delay compensa la differenza di distanza tra le casse e il punto di ascolto.
Formula: **delay (ms) = distanza (m) / 0.343** (velocità del suono in aria)

---

## 7. Guida AutoTune

L'AutoTune calibra automaticamente la risposta in frequenza di ogni cassa.

### 7.1 Prerequisiti
- Posizionare un microfono di riferimento al centro dell'area di ascolto
- Collegare il microfono all'ingresso XLR del sistema (ES8388 ADC)
- Assicurarsi che l'ambiente sia il più silenzioso possibile

### 7.2 Calibrazione Singola Cassa
1. In **DSP Avanzato**, premi **Calibra questa cassa**
2. Il sistema muta tutte le altre casse
3. Genera un sweep 20Hz–20kHz (visible: LED di stato lampeggia)
4. Cattura la risposta dal microfono (2 secondi)
5. Calcola la correzione EQ ottimale
6. Premi **Applica correzioni** per inviare l'EQ correttivo al DSP

### 7.3 Calibrazione Impianto Completo
Dalla schermata web o app, seleziona **AutoTune impianto completo**:
il sistema esegue automaticamente la calibrazione di ogni cassa in sequenza.

---

## 8. Guida Configurazione DMX Fixture

### 8.1 Aggiungere una Fixture
1. Vai su **Tab Luci → Fixture**
2. Premi **+** (aggiungi fixture)
3. Inserisci:
   - **Nome**: identificativo (es. "PAR SX 1")
   - **Tipo**: PAR_RGB, PAR_RGBW, MOVING_HEAD, WASH, STROBE...
   - **Indirizzo DMX**: canale di partenza (1-512)
   - Il numero di canali viene impostato automaticamente in base al tipo
4. Premi **Aggiungi**

### 8.2 Standard Indirizzi DMX Consigliati
```
Canali 1-50:    PAR palco sinistro (10 fixture × 5ch)
Canali 51-100:  PAR palco destro
Canali 101-150: Moving heads centrali
Canali 151-200: Wash truss frontale
Canali 201-250: Strobe
Canali 251-512: Liberi
```

---

## 9. Guida Audio-Reactive

Il sistema analizza il segnale audio in tempo reale e controlla le luci di conseguenza.

### 9.1 Attivazione
1. Vai su **Tab Luci → Audio-Reactive**
2. Attiva il toggle **ON/OFF**
3. Seleziona uno scenario (es. `BASS_PULSE`, `DJ_PARTY`)

### 9.2 Scenari Disponibili

| Scenario         | Descrizione                                          |
|------------------|------------------------------------------------------|
| `BASS_PULSE`     | I bassi fanno pulsare tutte le luci in rosso         |
| `FULL_SPECTRUM`  | Ogni banda controlla un colore diverso               |
| `KICK_FLASH`     | Bassi intensi → flash bianco su tutto l'impianto     |
| `DJ_PARTY`       | Bassi rossi, medi blu, alti bianchi                  |
| `CONCERT_SYNC`   | Bassi rosso, medi viola, alti ciano                  |
| `AMBIENT_FLOW`   | Reazione lenta e fluida a tutto lo spettro           |
| `VOCAL_WASH`     | Frequenze vocali (mid) → wash caldo                  |
| `BASS_COLOR_CHASE`| Bassi → chase sequenziale di colore                 |

---

## 10. USB-C Import/Export

### 10.1 Export
1. Collegare una chiavetta USB alla porta USB-C dell'ESP32
2. Dall'app: **Impostazioni → USB-C → Esporta Config**
3. Il file `sistema-audio.dsp-config` viene scritto sulla chiavetta

### 10.2 Import
1. Inserire la chiavetta con il file di configurazione
2. Dall'app: **Impostazioni → USB-C → Importa Config**
3. La configurazione viene caricata e applicata

### 10.3 Formati File

| Estensione       | Contenuto                          |
|------------------|------------------------------------|
| `.dsp-config`    | Configurazione completa sistema    |
| `.dsp-preset`    | Singolo preset DSP                 |
| `.dmx-scene`     | Singola scena DMX                  |
| `.dmx-fixtures`  | Configurazione fixture DMX         |
| `.audio-reactive`| Scenari audio-reactive             |

---

## 11. Troubleshooting

| Problema                        | Causa probabile              | Soluzione                              |
|---------------------------------|------------------------------|----------------------------------------|
| DSP non risponde                | Cablaggio RS-485             | Verificare A/B, GND, DE/RE             |
| App non si connette             | WiFi non connesso            | Connettiti a `SISTEMA_AUDIO_01`        |
| Luci non reagiscono alla musica | Audio-reactive disabilitato  | Abilitare in Tab Luci → Audio-Reactive |
| AutoTune fallisce               | Nessun segnale microfono     | Verificare connessione ADC ES8388      |
| Preset non si applica           | DSP disconnesso              | Riavviare ESP32 e verificare RS-485    |
| USB non rilevata                | Driver MSC non inizializzato | Verificare connessione USB-C e firmware|

---

## 12. Changelog

### v2.0.0 (2026)
- ✅ Implementato protocollo RS-485 CQ260D reale (da reverse engineering)
- ✅ Controller DMX512-A completo con 14 scene predefinite
- ✅ Sistema audio-reactive con FFT e 8 scenari
- ✅ AutoTune automatico (singola cassa + impianto completo)
- ✅ USB-C import/export configurazione completa
- ✅ 40+ endpoint REST API (incluso WLED)
- ✅ App Flutter responsive (smartphone + tablet)
- ✅ Modalità Base / Esperto con password opzionale
- ✅ 12 preset DSP di fabbrica + slot personalizzabili
- ✅ Task FreeRTOS dedicati per DMX (Core 1) e FFT (Core 0)
- ✅ Controllo WLED Neon LED via Wi-Fi (HTTP JSON API + UDP sync)

---

## 13. WLED – Controllo Neon LED

### 13.1 Architettura

```
ESP32-S3 (Access Point 192.168.4.1)
    │
    ├── Wi-Fi AP (SISTEMA_AUDIO_01)
    │       │
    │       ├── GLEDOPTO GL-C-015WL-D (192.168.4.10) — Cabinet_SX
    │       │       ├── CH1 → Logo SX  (WS2811 neon ~80px)
    │       │       ├── CH2 → Logo DX  (WS2811 neon ~80px)
    │       │       ├── CH3 → Front Frame (WS2811 neon ~96px)
    │       │       └── CH4 → Spare
    │       │
    │       └── GLEDOPTO GL-C-015WL-D (192.168.4.11) — Cabinet_DX
    │               ├── CH1 → Logo SX
    │               ├── CH2 → Logo DX
    │               ├── CH3 → Front Frame
    │               └── CH4 → Spare
    │
    └── HTTP JSON API POST /json/state → WLED controller
        UDP port 21324               → WLED real-time sync
```

### 13.2 Hardware per Cabinet

| Componente              | Modello/Specifiche                              |
|-------------------------|-------------------------------------------------|
| Controller WLED         | GLEDOPTO GL-C-015WL-D (ESP32, 2CH o 4CH)       |
| Strip LED               | WS2811 12V RGBIC neon flex, 96 LED/m            |
| Passo pixel             | 1 pixel ogni 3 LED (taglio ogni 3 LED)          |
| Alimentatore            | Mean Well LRS-150-12 (12V, 12.5A, 150W)        |

**Zone per cabinet:**
- `logo_left` — profilo SX del logo sulla cassa
- `logo_right` — profilo DX del logo sulla cassa
- `front_frame` — cornice frontale inferiore/superiore
- `spare` — canale aggiuntivo (facoltativo)

### 13.3 Calcolo Alimentazione

```
Striscia neon WS2811 12V: ~9.6W/m
Lunghezza totale stimata per cabinet: ~8m

P_totale = 8m × 9.6W/m = 76.8W
I_totale = 76.8W / 12V = 6.4A

Alimentatore consigliato: Mean Well LRS-150-12 (12V, 12.5A, 150W) ✓
Margine di sicurezza: 12.5A / 6.4A ≈ 195%
```

### 13.4 Schema Cablaggio

```
Mean Well LRS-150-12 (12V, 12.5A)
│
├── +12V ──→ Strip VCC (rosso)
├── GND  ──→ Strip GND (nero/bianco)
│
└── GLEDOPTO GL-C-015WL-D
        │
        ├── +5V/USB → alimentazione logica controller (via USB-C o 5V separato)
        ├── CH1 DATA ──[220Ω]──→ Strip DATA zona logo_left
        ├── CH2 DATA ──[220Ω]──→ Strip DATA zona logo_right
        ├── CH3 DATA ──[220Ω]──→ Strip DATA zona front_frame
        └── CH4 DATA ──[220Ω]──→ Strip DATA zona spare

Resistore sui DATA:  220Ω–470Ω sul segnale DATA
Condensatore:        470µF–1000µF/25V vicino al connettore alimentazione strip
Nota:                Il terminatore DMX 120Ω si applica solo all'ultimo device RS-485,
                     NON alle strip WS2811 (protocollo unidirezionale)
```

### 13.5 Layout Fisico nel Cabinet

```
  [ TOP ]
  ┌─────────────────────────────────────────┐
  │  Controller WLED (soffitto, scomparto    │
  │  tweeter)                               │
  ├────────────────┬────────────────────────┤
  │  Horn/tweeter  │  Crossover (divisore)  │
  ├────────────────┴────────────────────────┤
  │         Woofer / midrange               │
  ├─────────────────────────────────────────┤
  │  Cavi audio    │    Cavi 12V            │
  │  (sx)          │    (dx, separati)      │
  ├─────────────────────────────────────────┤
  │  PSU LRS-150-12 (box inferiore,         │
  │  con ventilazione)                      │
  └─────────────────────────────────────────┘
  [ BOTTOM ]

  Neon logo:
    ← [logo_left]  [ LOGO ]  [logo_right] →
    ─────────── [front_frame] ───────────
```

### 13.6 API REST WLED (14 endpoint)

| Metodo | Endpoint | Descrizione |
|--------|----------|-------------|
| GET    | `/api/wled/controllers`       | Lista controller + stato |
| POST   | `/api/wled/controllers`       | Aggiungi controller |
| DELETE | `/api/wled/controllers`       | Rimuovi controller |
| POST   | `/api/wled/color`             | Imposta colore zona |
| POST   | `/api/wled/brightness`        | Imposta luminosità zona |
| POST   | `/api/wled/effect`            | Imposta effetto zona |
| GET    | `/api/wled/scenes`            | Lista scene WLED |
| POST   | `/api/wled/scenes`            | Crea/aggiorna scena |
| POST   | `/api/wled/scenes/apply`      | Applica scena |
| POST   | `/api/wled/sync`              | Sincronizza tutti |
| POST   | `/api/wled/blackout`          | Blackout tutti |
| POST   | `/api/wled/mirror`            | Mirror due zone |
| POST   | `/api/wled/discover`          | Scopri controller via UDP |
| GET    | `/api/wled/status`            | Stato sistema WLED |

### v1.0.0 (baseline)
- Stub RS-485 con 3 dispositivi simulati
- Web server base con endpoint volume/modalità/discovery
- App Flutter base con home screen e discovery
