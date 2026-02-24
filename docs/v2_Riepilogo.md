# Sistema DSP Audio PA – Riepilogo Tecnico v2

## 1. Architettura Generale

Il sistema è composto da un **box di controllo centrale** basato su **ESP32-S3** collegato a una **catena RS-485** che pilota uno o più moduli **PDA1001 + CQ260D** per la gestione DSP professionale dei diffusori.

```
[Mixer/Sorgente XLR] ──► [Box Controllo ESP32-S3]
                                │
                         [Relay DPDT]
                        /             \
               [MixerPassThrough]   [TestTone DAC]
                        │
                    [RS-485 BUS]
                   /      |      \
             [PDA1001] [PDA1001] [PDA1001]
                │          │         │
             [CQ260D]   [CQ260D]  [CQ260D]
                │          │         │
           [Cassa 2-vie] [Cassa 3-vie] [Sub]
```

### Tipologie di diffusori supportati
| Tipo   | Descrizione          | Vie | Driver                        |
|--------|----------------------|-----|-------------------------------|
| 2WAY   | Cassa 2 vie          | 2   | Woofer 8"/10" + Tweeter 1"    |
| 3WAY   | Cassa 3 vie          | 3   | Woofer 15" + Mid 6" + Tweeter |
| SUB    | Subwoofer            | 1   | Woofer 15"/18" LF             |

---

## 2. Hardware

### Box Controllo – ESP32-S3
| Componente               | Modello / Note                        |
|--------------------------|---------------------------------------|
| Microcontrollore         | ESP32-S3-DevKitC-1 (PSRAM inclusa)    |
| Display touch            | 5" 800×480 capacitivo (SPI/I2C)       |
| Codec audio              | M5Stack ES8388 (I2S)                  |
| Driver RS-485            | MAX485 (DE/RE controllati da GPIO)    |
| Relay                    | DPDT optoisolato (MixerPT / TestTone) |
| LED di stato             | LED integrato GPIO 48                 |
| Alimentazione            | 5V DC (da rack o alimentatore esterno)|

### Moduli DSP su casse
| Componente | Modello  | Note                                   |
|------------|----------|----------------------------------------|
| Amplificatore | PDA1001 | Classe D, controllo RS-485           |
| DSP           | CQ260D  | Crossover/EQ digitale, RS-485        |

### Connessioni principali
- **RS-485**: cavo Twisted Pair (2 fili + schermo), terminazione 120 Ω a fine catena
- **Audio IN/OUT**: XLR bilanciato
- **Alimentazione casse**: 230V AC (amplificatori integrati)

---

## 3. Modalità Audio

### MixerPassThrough (default)
- Il relay DPDT è in posizione **NC (Normally Closed)**
- Il segnale XLR IN viene instradato **direttamente** a XLR OUT verso le casse
- L'ES8388 rimane in ascolto (ADC attivo) per analisi FFT futura
- Nessuna elaborazione aggiuntiva del segnale audio

### TestTone
- Il relay DPDT commuta in posizione **NA (Normally Open)**
- Il **DAC ES8388** genera un tono sinusoidale (default 1 kHz, ampiezza configurabile)
- Utile per test di configurazione, identificazione casse, verifica cablaggio
- La frequenza e l'ampiezza sono configurabili via API HTTP

---

## 4. Sistema di Discovery RS-485

### Procedura di scansione
1. L'ESP32-S3 trasmette su RS-485 un comando di **broadcast ping** (indirizzo 0xFF)
2. Ogni modulo PDA1001+CQ260D risponde con il proprio **ID univoco** (1–127)
3. Il sistema raccoglie le risposte entro un timeout configurabile
4. Il display mostra i dispositivi trovati con ID e tipo dichiarato
5. L'utente assegna posizione e ruolo a ciascun dispositivo

### Identificazione sonora (BeepPattern)
Ogni cassa può emettere pattern sonori per identificazione fisica:
| Pattern | Beep | Significato               |
|---------|------|---------------------------|
| Single  | 1×   | Cassa identificata        |
| Double  | 2×   | Cassa selezionata         |
| Triple  | 3×   | Conferma assegnazione     |

### Mappatura da smartphone
- L'ESP32-S3 espone un **WiFi Access Point** (`SISTEMA_AUDIO_01`)
- L'app Flutter si connette e mostra la lista dei dispositivi trovati
- L'operatore cammina nella sala, ascolta i beep e assegna le posizioni
- Le assegnazioni vengono salvate su NVS (flash persistente)

---

## 5. Modello Dati Casse

### SpeakerDevice
```
id       : uint8_t   – indirizzo RS-485 univoco (1–127)
type     : string    – "2WAY" | "3WAY" | "SUB"
online   : bool      – raggiungibile sulla catena RS-485
```

### SpeakerAssignment
```
deviceId : uint8_t   – riferimento a SpeakerDevice.id
position : string    – "SX1" | "SX2" | "SX3" | "DX1" | ... | "SUB1" | "SUB2" | "P1" | "P2"
role     : enum      – SX | DX | SUB | MONO
type     : string    – "2WAY" | "3WAY" | "SUB"
name     : string    – nome personalizzato (es. "Palco Sinistra")
```

### Posizioni disponibili
```
SX1, SX2, SX3   – lato sinistro
DX1, DX2, DX3   – lato destro
SUB1, SUB2       – subwoofer
P1, P2           – palco / monitor
```

---

## 6. Display Adattivo (LVGL)

Il display 5" touch mostra **solo le casse effettivamente connesse**, adattandosi dinamicamente alla configurazione del sistema.

### Schermate LVGL
| Schermata     | Contenuto                                                      |
|---------------|----------------------------------------------------------------|
| **Home**      | Slider volume master, slider balance SX/DX, lista casse attive |
| **Discovery** | Pulsante scansione RS-485, lista ID trovati                    |
| **Assignment**| Bottoni posizione/tipo, campo nome, SALVA/AVANTI               |

### Funzionalità display
- Aggiornamento dinamico: le casse compaiono/scompaiono in base alla connessione
- Possibilità di aggiungere nuovi diffusori senza riconfigurare il firmware
- Touch capacitivo per navigazione intuitiva

---

## 7. Server HTTP/WebSocket (ESP32-S3)

### WiFi Access Point
- **SSID**: `SISTEMA_AUDIO_01`
- **Password**: `audio1234`
- **IP ESP32**: `192.168.4.1`

### REST API
| Metodo | Endpoint              | Body / Risposta                           |
|--------|-----------------------|-------------------------------------------|
| GET    | `/api/devices`        | JSON array di SpeakerAssignment           |
| POST   | `/api/audio-mode`     | `{"mode": "mixer" \| "test"}`             |
| POST   | `/api/master-volume`  | `{"value": 0–100}`                        |
| POST   | `/api/speaker-volume` | `{"id": N, "value": 0–100}`               |
| POST   | `/api/discovery`      | trigger scan → `{"devices": [id, ...]}`   |

### Interfaccia Web integrata
- Pagina HTML servita su `/` con slider e pulsanti
- Compatibile con browser mobile (responsive)
- Nessuna dipendenza esterna (tutto inline)

---

## 8. App Flutter Android

### Funzionalità
- Connessione al WiFi AP dell'ESP32 (`192.168.4.1`)
- Visualizzazione e controllo volume per ogni cassa
- Avvio procedura di discovery RS-485
- Assegnazione posizione e tipo ai dispositivi trovati
- Configurazione nome personalizzato per ogni cassa
- Persistenza locale delle impostazioni (SharedPreferences)

### Schermate
| Schermata               | Descrizione                                      |
|-------------------------|--------------------------------------------------|
| HomeScreen              | Volume master + lista casse con slider           |
| DiscoveryScreen         | Scansione RS-485 + lista ID trovati              |
| SpeakerDetailScreen     | Dettaglio singola cassa, volume, tipo, ruolo     |

---

## 9. Preset DSP

I preset DSP configurano il CQ260D per ogni tipo di cassa:

| Preset     | Applicazione                        | HPF    | LPF    | Note                       |
|------------|-------------------------------------|--------|--------|----------------------------|
| `2WAY_STD` | Cassa 2 vie standard                | 80 Hz  | 20 kHz | Crossover interno a 3 kHz  |
| `3WAY_STD` | Cassa 3 vie full-range              | 60 Hz  | 20 kHz | Crossover a 500 Hz / 4 kHz |
| `SUB_STD`  | Subwoofer standard                  | 30 Hz  | 120 Hz | Solo basse frequenze       |
| `MONITOR`  | Monitor da palco                    | 100 Hz | 16 kHz | Risposta piatta             |
| `OUTDOOR`  | Utilizzo outdoor (vento, distanza)  | 100 Hz | 18 kHz | Boost medio-alti +2 dB     |

---

## 10. Driver Acustici e Volumi

### Cassa 2 Vie (2WAY)
- Woofer: 8" o 10", 100–200 W, 8 Ω
- Tweeter: 1" compression driver, 30–50 W, 8 Ω
- Crossover DSP: ~3 kHz
- Volume cassa: ~50–80 L (trapezoidale)

### Cassa 3 Vie (3WAY)
- Woofer: 15", 400–600 W, 8 Ω
- Mid-range: 6"–8", 100–200 W, 8 Ω
- Tweeter: 1.5" compression driver, 50–80 W, 8 Ω
- Crossover DSP: ~500 Hz / 4 kHz
- Volume cassa: ~120–200 L

### Subwoofer (SUB)
- Woofer: 15" o 18", 600–1200 W, 4 Ω o 8 Ω
- Filtro LPF DSP: 80–120 Hz (configurabile)
- Volume cassa: ~150–300 L (vented o bass-reflex)

---

## 11. Costi Approssimativi e Materiali

### Box Controllo
| Componente                    | Costo stimato (€) |
|-------------------------------|-------------------|
| ESP32-S3-DevKitC-1            | 8–12              |
| Display 5" 800×480 touch SPI  | 25–40             |
| M5Stack ES8388 codec board    | 15–25             |
| MAX485 breakout board         | 2–4               |
| Relay DPDT optoisolato        | 3–6               |
| PCB / connettori / cavi       | 10–20             |
| Box rack 1U alluminio         | 20–35             |
| **Totale box controllo**      | **~83–142**       |

### Per ogni modulo cassa (PDA1001 + CQ260D)
| Componente              | Costo stimato (€) |
|-------------------------|-------------------|
| Amplificatore PDA1001   | 80–120            |
| DSP CQ260D              | 60–100            |
| Cablaggio RS-485        | 3–8               |
| **Totale per modulo**   | **~143–228**      |

### Esempio configurazione completa (4 casse + 2 sub)
| Voce                      | Quantità | Costo (€)    |
|---------------------------|----------|--------------|
| Box controllo ESP32-S3    | 1        | 83–142       |
| Moduli PDA1001+CQ260D     | 6        | 858–1368     |
| Casse 2-vie / 3-vie       | 4        | 400–1200     |
| Subwoofer                 | 2        | 400–800      |
| Cablaggio XLR + RS-485    | –        | 50–100       |
| **Totale indicativo**     |          | **~1791–3610**|

> ⚠️ I costi sono puramente indicativi e soggetti a variazioni di mercato.

---

## 12. Note di Sviluppo e TODO

- [ ] Implementare protocollo RS-485 reale PDA1001/CQ260D (attualmente stub simulato)
- [ ] Integrare libreria ESP-DSP per analisi FFT real-time
- [ ] Aggiungere autenticazione all'API HTTP (almeno token statico)
- [ ] Sviluppare WebSocket per aggiornamenti push verso l'app Flutter
- [ ] Testare LVGL con display fisico e calibrare touch
- [ ] Implementare OTA (Over-The-Air update) per il firmware
- [ ] Aggiungere logging su SD card o SPIFFS per diagnostica
- [ ] Certificazione EMC per uso professionale
