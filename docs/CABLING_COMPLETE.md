# Cablaggio completo – Box Master + Box Slave/Wireless

Documento di riferimento per assemblaggio, **allineato ai PDF in `docs/`** (UEDX, M144, ES8388, Relè, Encoder, Pololu ORing, MAX485, Led Ring).  
Sintesi tecnica centralizzata: **[`DATASHEETS_REFERENCE.md`](DATASHEETS_REFERENCE.md)**.

---

## 0. PDF in repository (fonte ufficiale)

| File | Ruolo |
|------|--------|
| **UEDX80480050E-WB-A-V3.3-SPEC.pdf** | Modulo 5" + ESP32-S3: **5 V ~300 mA** max, J2 GPIO, TP GT911 |
| **M144_sch_moduleaudio_v10.pdf** | Modulo M5 **ES8388**: I2S (SCLK/DSDIN/LRCK/ASDOUT), jack PJ-342B, BUS |
| **ES8388.pdf** | Datasheet codec |
| **Relè Module interface.pdf** | **DC+ DC− IN**, **NO COM NC**, jumper LOW/HIGH trigger |
| **Encoder rotativo.pdf** | **EC11**, 5 V, **7 pin** |
| **pololu-power-oring-ideal-diode-pair.pdf** | **Pololu #5398**, 4–60 V, 6 A, **VIN1/VIN2/VOUT** |
| **MAX485.pdf** | **DI RO DE RE A B** |
| **Led Ring WS2811.pdf** | Anello LED (5 V tip., DIN, GND) |

**UEDX – I2S (SPEC):** GPIO **11** = RTP-DIN (dato verso ESP), **12** = BCLK, **13** = linea I2S verso codec (LRCK/DOUT secondo routing). Coerente con `config.h` (I2S_DIN/BCLK/WS).

**UEDX – conflitto GPIO38:** sulla SPEC, **GPIO38** = **reset touch (TP pin 6)**. Il firmware assegna **GPIO38 = I2S_DOUT**. Sul PCB UEDX il pin è condiviso con il touch: verificare schema **V3.3**; se necessario spostare **I2S_DOUT** su altro GPIO libero e aggiornare `config.h`, oppure usare **solo modulo M144** collegato con fili corti e I2S su header non in conflitto con display.

---

## 1. Box Master – panoramica blocchi

```mermaid
flowchart TB
  subgraph PWR["Alimentazione"]
    AC[Ingresso AC (rete 230V)]
    PSU5[Mean Well IRM-30-5ST (5V, 6A)]
    REG33[Regolatore 3.3V (AMS1117 o equivalente)]
    LC_CODEC[Filtro LC (anti-disturbi) per ES8388]
  end
  subgraph AUDIO["Percorso audio"]
  RCA_IN_L[RCA In L sbilanciato]
  RCA_IN_R[RCA In R sbilanciato]
  ES8388[ES8388 Codec]
  RCA_OUT[RCA Out L/R]
  end
  subgraph MCU["ESP32-S3 Master UEDX80480050E"]
    ENC_V[Encoder Vol A/B + BTN]
    ENC_B[Encoder Bal A/B + BTN]
    RING_V[WS2812 ring Vol]
    RING_B[WS2812 ring Bal]
    TCA[TCA9535 I2C 0x20]
    MAX_DSP[MAX485 DSP]
    RJ45_DSP[RJ45 verso bus DSP]
  end
  AC --> PSU5
  PSU5 --> MCU
  PSU5 --> REG33
  REG33 --> LC_CODEC
  LC_CODEC --> ES8388
  RCA_IN_L --> ES8388
  RCA_IN_R --> ES8388
  ES8388 --> RCA_OUT
  ENC_V --> MCU
  ENC_B --> MCU
  RING_V --> MCU
  RING_B --> MCU
  TCA --> RELAY
  MAX_DSP --> RJ45_DSP
  ES8388 <--> MCU
```

---

## 2. Alimentazione – Alimentatore dedicato (Mean Well IRM) + 3.3V + filtro LC codec

Dato che l’uscita ausiliaria dei moduli DSP **non è continua** ma va abilitata dal DSP stesso, l’alimentazione del Box Master viene resa **indipendente** tramite alimentatori dedicati.

### 2.1 Scelta consigliata (Master)

- **PSU principale 5 V**: `Mean Well IRM-30-5ST` (**5 V, 6 A, 30 W**) per:
  - UEDX (ESP32 + display)
  - periferiche 5 V (WS2812, MAX485, ecc.)
  - eventuali moduli aggiuntivi
- **Alimentazione modulo ES8388 (M5 M144)**:
  - il **modulo M144** si alimenta a **5 V** (pin `5V` sul BUS); il chip ES8388 lavora internamente a **3.3 V**
- **Filtro (ferrite/LC) anti-disturbi** prima del modulo M144:
  - consigliato soprattutto se alimenti display/backlight e LED dalla stessa 5 V

### 2.2 Collegamento concettuale

```text
  230VAC ──► IRM-30-5ST (5V) ──► +5V_SYS ──► UEDX / MAX485 / WS2812 / ecc.
                           └──► (ferrite/LC) ──► +5V_M144 ──► Modulo ES8388 M144 (interno 3.3V)

  GND: punto stella (analogico) vicino al codec; ritorni “sporchi” (LED/backlight) separati fin dove possibile.
```

### 2.3 Valori pratici filtro (linea 5V verso M144)

Esempio “robusto” (non critico, va bene anche come ferrite+caps):

- **Ferrite bead**: 600Ω @ 100MHz (corrente nominale ≥ 300–500 mA)
  - oppure **L**: 10 µH (corrente nominale ≥ 300–500 mA, bassa DCR)
- **C lato ingresso**: 10 µF + 100 nF verso GND
- **C lato uscita (vicino al modulo M144)**: 10 µF + 100 nF verso GND

Obiettivo: ridurre ripple/commutazione e isolare il codec dai disturbi digitali.

### 2.4 Dimensionamento rapido (correnti e dissipazione)

Questa sezione serve come checklist “prima di cablare”, con stime conservative.

#### 2.4.1 Master (IRM-30-5ST, 5V 6A)

- **UEDX (ESP32 + display)**: fino a ~**300 mA** (picchi/backlight dipendono da luminosità).
- **2× LED ring WS2812**: caso peggiore teorico (bianco pieno) può arrivare a ~**1–2 A** totali a 5 V (dipende dal numero LED reali e brightness).
- **RS-485 / periferiche**: tipicamente **<200 mA** complessivi.

Con `IRM-30-5ST (6 A)` hai margine abbondante anche in condizioni “worst case”.

#### 2.4.2 RX (IRM-05-5, 5V 1A + IRM-20-15, 15V)

- **ESP32-S3 RX (Wi‑Fi/ESP‑NOW)**: tipicamente 200–400 mA, con picchi possibili più alti in TX.
- **ES8388 + I2C/I2S + logica**: tipicamente decine di mA.
- **MAX485 / IO**: tipicamente poche decine di mA.

`IRM-05-5 (5V 1A)` è in genere sufficiente per RX; se aggiungi molte periferiche (LED, relè, ecc.) valuta una versione 5 V più potente.

Per il bilanciato:
- `IRM-20-15 (15V 20W)` fornisce headroom e margine ampio per il modulo DRV134 (assorbimenti tipicamente bassi, ma il margine aiuta su transitori e stabilità).

#### 2.4.3 Dissipazione AMS1117-3.3

L’AMS1117 dissipa:

\[
P \approx (V_{in}-V_{out}) \cdot I \approx (5-3.3)\cdot I = 1.7 \cdot I
\]

Esempi:
- a **100 mA** → \(P \approx 0.17 W\) (ok su PCB con rame decente)
- a **300 mA** → \(P \approx 0.51 W\) (serve area rame/heatsinking, attenzione temperatura)

Se il carico 3.3 V cresce, valuta un regolatore più efficiente o un LDO con migliore termica/rumore.

#### 2.4.4 Note pratiche sui filtri LC

- Scegli induttanze con **corrente nominale** superiore al carico (codec/driver) e **bassa DCR**.
- Metti i condensatori **vicino ai pin di alimentazione** del carico (ES8388 / DRV134).
- Se noti risonanze (whine/instabilità), una soluzione semplice è usare **ferrite bead** al posto dell’induttore o aggiungere un po’ di ESR (es. elettrolitico) sul ramo.

#### 2.4.5 Sicurezza lato AC (importante)

- Inserire **fusibile** sul conduttore di fase (L) e, se possibile, **interruttore** generale.
- Se usi contenitori metallici, prevedi **PE (terra)** e fissaggio meccanico adeguato.
- Mantieni separazione fisica tra AC e bassa tensione (distanze/isolamenti) e passacavi.

---

## 2b. Modulo audio M5 (M144) + ES8388 – da M144_sch_moduleaudio_v10.pdf

| Segnale | ES8388 (pin) | Verso ESP32-S3 (firmware) |
|---------|--------------|---------------------------|
| BCLK | SCLK (5) | GPIO **12** |
| DAC data | DSDIN (6) | GPIO **38** (⚠ conflitto touch UEDX) |
| LRCK | (7) | GPIO **13** |
| ADC data | ASDOUT (8) | GPIO **11** |
| I2C codec | CE/CDATA/CCLK → BUS SDA/SCL | Stesso bus **GPIO19/20** + pull-up 2k2 |

Uscite cuffia / linea: **LOUT1/ROUT1** → jack **PJ-342B** (J1/J2) con **22 Ω** serie. Ingressi linea: **CODEC_LIN1/RIN1**, **LIN2/RIN2**.

---

## 3. RS-485 DSP – MAX485 (datasheet) + RJ45

**MAX485:** **DI**←TX MCU, **RO**→RX MCU, **DE**+**RE** insieme, **A/B** bus. **DMX** = stesso tipo di transceiver ma **altro modulo** e **250 kbaud** sullo Slave.

### 3.1 Collegamento Master → MAX485 → RJ45

| ESP32 Master | MAX485 (DSP) | RJ45 (suggerito – adattare al tuo cavo) |
|--------------|--------------|----------------------------------------|
| GPIO43 TX | DI | — |
| GPIO44 RX | RO | — |
| GPIO10 | DE + /RE (insieme) | — |
| GND | GND | Pin **1** (schermo/GND) |
| — | **A** | Pin **4** (D+) – *da allineare ai moduli* |
| — | **B** | Pin **5** (D−) |

**Standard de facto spesso usato (non IEEE Ethernet):**

| Pin RJ45 (T568B) | Uso tipico bus RS-485 custom |
|------------------|------------------------------|
| 1 | GND / schermo |
| 2 | GND |
| 3 | riservato |
| 4 | RS-485 **A** |
| 5 | RS-485 **B** |
| 6 | riservato |
| 7 | riservato (non usato per alimentazione) |
| 8 | riservato |

**Verifica obbligatoria** su primo modulo DSP: continuità A/B e (se presenti) eventuali funzioni extra su RJ45.  
In questa revisione **non si usa** alimentazione ausiliaria via RJ45.

Terminazione **120 Ω** tra A e B sull’**ultima** presa della catena (o solo sul box se punto unico).

---

## 4. Ingressi audio sbilanciati (RCA) → ES8388

L’**ES8388** viene ora alimentato direttamente da ingressi **linea sbilanciati RCA** (tipicamente **LIN1/RIN1** verso massa analogica).  
La sorgente (es. **Denon** / pre-out) entra con ODG di livello linea compatibile con il range dell’ES8388, senza stadio di conversione bilanciato→sbilanciato.

### Collegamento concettuale (per canale)

```text
  RCA centrale L ──► AC couple (10µF) ──► eventuale attenuatore ──► ES8388 LINx
  RCA centrale R ──► AC couple (10µF) ──► eventuale attenuatore ──► ES8388 RINx
  Schermo RCA ─────► GND analogico (punto stella)
```

- **Gain**: il guadagno si regola via **PGA ES8388** (firmware già usa `setADCGain`).
- **Protezione**: opzionale clamp con diodi verso rail su ingressi del codec se ambiente “live”.

Non esistono più **XLR passanti** né relay DPDT per selezionare pass-through: il segnale RCA dalla sorgente arriva **solo** al controller (ES8388) e non viene più fatto proseguire in analogico verso un’uscita “thru”.

---

## 5. Ingressi/uscite ES8388 – RCA + attenuatore

### 5.1 Ingressi RCA → ES8388 (ADC)

Per ogni canale:

```text
RCA centrale ── R1 = 10kΩ ──► Nodo IN_CODEC ──► condensatore ingresso ES8388
                         │
                         └─ R2 = 4.7kΩ ──► GND analogico
Schermo RCA ───────────────► GND analogico (punto stella)
```

- **Attenuazione** ≈ ‑9.5 dB (fattore ~0.33), adatta a sorgenti “calde” tipo Denon.
- **Impedenza vista dalla sorgente** ≈ 15kΩ (range tipico linea).

### 5.2 Uscite ES8388 → RCA (DAC)

| ES8388 (tipico breakout M5Stack / Atom) | RCA |
|----------------------------------------|-----|
| LOUT1 / ROUT1 (o HP out se usato) | RCA centrale |
| AGND | RCA schermo |

Usare cavi corti; massa RCA collegata al **punto stella** analogico, separata dai ritorni di potenza.

---

## 6. Encoder EC11 + LED ring (Encoder.pdf + Led Ring.pdf)

**Encoder:** modello **EC11**, **5 V**, 7 pin (A, B, COM, SW, …). **LED ring:** **5 V**, **DIN** + GND, 100 nF vicino al primo LED.

Da **`config.h`** / firmware:

| Funzione | GPIO Master | Collegamento fisico |
|----------|-------------|---------------------|
| Encoder **Volume** A (CLK) | **GPIO35** | Terminale A encoder |
| Encoder **Volume** B (DT) | **GPIO36** | Terminale B encoder |
| Encoder **Volume** pulsante | **GPIO37** | Un pin a BTN, altro a GND (pull-up interno) |
| **WS2812 ring Volume** DATA | **GPIO27** | DIN primo LED anello (5 V + condensatore 100 µF vicino anello) |
| Encoder **Balance** A | **GPIO33** | |
| Encoder **Balance** B | **GPIO34** | |
| Encoder **Balance** pulsante | **GPIO26** | |
| **WS2812 ring Balance** DATA | **GPIO28** | |

**LED WS2812B:** alimentazione **5 V**; livello dati 3.3 V spesso sufficiente; se instabile, **level shifter** 74HCT245 o 1 MOS per linea DATA. **Resistore serie 330 Ω** su ogni DATA.

**Pulsanti:** firmware assume **LOW = premuto** (pull-up interno).

---

## 7. Relay (modulo Relè interface.pdf) + TCA9535

Modulo relè tipico: **DC+ DC−** bobina, **IN** logica, **NO / COM / NC** contatti. Jumper **LOW** = attivo basso su **IN**.

| Da TCA9535 P0_0 (o GPIO 3.3 V) | Verso modulo relè |
|---------------------------------|-------------------|
| Open collector / NPN collettore | **IN** (se jumper LOW: GND = attiva bobina via transistor) |
| Modulo **DC+** | **5 V o 12 V** bobina (secondo modulo) |
| **COM / NO / NC** | Percorso audio **DPDT** (Mixer pass-through vs test tone) |

Il firmware oggi usa indice **RELAY_PIN = 0** (TCA9535): implementare **scrittura I2C** su expander per pilotare **IN** del modulo relè (non `pinMode` su GPIO0).

---

## 8. I2S ESP32 Master ↔ ES8388

| Segnale | GPIO Master | ES8388 |
|---------|-------------|--------|
| BCLK | **12** | BCLK |
| WS (LRCK) | **13** | LRCK |
| DIN (ESP ← ADC) | **11** | DOUT |
| DOUT (ESP → DAC) | **38** | DIN |
| I2C SDA/SCL | (stesso bus codec + TCA9535 + GT911) | SDA/SCL |
| Indirizzo I2C | — | **0x10** |

---

## 9. Box Slave / Wireless (secondo ESP32 + DMX + opzionale ES8388)

### 9.1 Come da firmware attuale (`firmware/esp32_slave`)

| Funzione | GPIO Slave | Connettore / nota |
|----------|------------|-------------------|
| I2S BCLK in | 5 | Da Master GPIO12 |
| I2S WS in | 6 | Da Master GPIO13 |
| I2S DIN | 7 | Fan-out da ES8388 DOUT Master |
| IPC RX / TX | 2 / 1 | UART verso Master |
| **DMX TX** | **8** | → **MAX485 #2** DI |
| **DMX DE** | **9** | MAX485 DE+/RE- HIGH |
| MAX485 A/B | — | **XLR 3 pin** (pin 3+, 2−, 1 gnd) |
| Relay | 10 | Opzionale |
| Strobe | 11 | Opzionale |

**Secondo MAX485:** dedicato al **DMX 250 kbaud** (firmware); **non** condividere lo stesso transceiver del bus DSP 115200 senza multiplexing complesso.

### 9.2 Estensione “Slave con ES8388” (Atomstack / schematico tuo)

Se aggiungi uno **Slave con ES8388** (monitor locale o seconda coppia RCA/XLR):

- **I2S Slave** resta per **FFT / ESP-NOW** (clock dal Master).
- **ES8388 sullo Slave** richiede **altri GPIO** I2S + I2C e **non è nel firmware Slave attuale** – da progettare (es. I2S master locale solo per quel codec, o secondo bus).

Schema logico proposto per **box Slave autonomo**:

```text
  ESP32-S3 Slave
    ├── MAX485 #2 ──► XLR DMX out
    ├── RJ45 ──► stesso bus RS-485 DSP (opzionale, se vuoi slave sul bus)
    ├── XLR audio out (opz.) ──► da DAC esterno o futuro ES8388
    ├── Alimentazione 5 V (da ORing condiviso o buck locale)
    └── Antenna IPEX → ESP-NOW
```

Inserisci lo **schematico Atomstack** nella cartella `docs/hardware/` (PDF) e aggiorna questa tabella con i pin reali.

---

## 10. Riepilogo connettori pannello – Box Master

| Connettore | Funzione |
|------------|----------|
| Ingresso AC | Alimentazione → IRM-30-5ST (5V) |
| RJ45 | Bus RS-485 DSP (solo dati A/B + GND/schermo) |
| RCA In L/R | Ingressi linea sbilanciati da sorgente (es. Denon) → ES8388 |
| RCA Out L/R | Uscita monitor / registrazione da ES8388 DAC |
| USB-C | Programmazione ESP (se presente sulla UEDX) |

---

## 11. Riepilogo connettori – Box Slave

| Connettore | Funzione |
|------------|----------|
| XLR 3 pin | **DMX OUT** (da MAX485 #2) |
| XLR (opz.) | Audio out (futuro ES8388) |
| RJ45 | Bus DSP (se slave sul bus) |
| Header 7 fili | Verso Master (I2S + UART + GND) |

---

## 12. File correlati nel repo

| File | Contenuto |
|------|-----------|
| **`docs/DATASHEETS_REFERENCE.md`** | Sintesi da **tutti i PDF** |
| `docs/WIRING_GUIDE.md` | Dual-ESP32 7 fili |
| `docs/PINOUT_REFERENCE.md` | GPIO Master/Slave |
| `docs/DISPLAY_SETUP.md` | Display + touch + UEDX SPEC |
| `docs/SCHEMATICS.md` | Indice + link schema HTML |
| `docs/SCHEMATIC_DSP_CONNECTIONS.html` | Schemi completi stampabili |
| `firmware/esp32/src/config.h` | Pin firmware Master |
| PDF in `docs/*.pdf` | Fonte originale |

---

*Ultimo aggiornamento: cablaggio esteso encoder, LED, relay, alimentazione con PSU IRM dedicati, XLR bilanciato, RJ45 DSP, secondo MAX485 DMX, Slave esteso.*
