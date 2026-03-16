## Guida passo‑passo KiCad – Tavole schema DSP Control

Questa guida riassume, **foglio per foglio**, cosa piazzare e come collegare i simboli in KiCad per ricostruire gli schemi elettrici del sistema DSP Control (Master + Slave + RX wireless).

Usa questa guida insieme alle tavole concettuali `schematics_electric.html` e `schematics_graphic.html`.

---

## Foglio 1 – Master → MAX485 → RJ45 (Bus DSP CQ260D)

**Obiettivo**: collegare l’ESP32 Master al bus RS‑485 dei moduli CQ260D tramite MAX485 e RJ45.

### Simboli da piazzare

- Blocco `ESP32 Master` (anche come connettore generico, es. `Connector_Generic:Conn_01x04`), con pin logici:
  - `GPIO43 (RS485_TX)`
  - `GPIO44 (RS485_RX)`
  - `GPIO10 (RS485_DE_RE)`
  - `GND`
- `U1` = `Interface_UART:MAX485E` (o simbolo equivalente MAX485).
- `J_RJ45` = `Connector_RJ:RJ45_8`.
- `C_RS485` = `Device:C`, valore `100n`.
- `R_TERM` = `Device:R`, valore `120R` (terminazione di linea, opzionale sulla tavola se vuoi mostrarla).

### Collegamenti

1. **ESP32 → MAX485**
   - `GPIO43` → pin **4 DI** (`RS485_TX`).
   - `GPIO44` ← pin **1 RO** (`RS485_RX`).
   - `GPIO10` → pin **2 /RE** e **3 DE** (collegati insieme).
   - GND Master → pin **5 GND**.

2. **MAX485 → RJ45**
   - Pin **6 A** → net `RS485_A` → pin **4** di `J_RJ45`.
   - Pin **7 B** → net `RS485_B` → pin **5** di `J_RJ45`.
   - Pin **5 GND** → GND → pin **1–2** di `J_RJ45` (massa e schermo).

3. **Alimentazione e terminazione**
   - Pin **8 VCC** → net `+5V`.
   - `C_RS485` fra `+5V` e `GND` vicino al MAX485.
   - `R_TERM` tra `RS485_A` e `RS485_B` (solo sull’ultimo nodo del bus o sul box Master se vuoi terminare lì).

---

## Foglio 2 – Ingressi RCA → Pad universale → ES8388 + Uscite RCA

**Obiettivo**: pad universale 15k/4.7k sugli ingressi RCA, collegamento a ES8388, uscite RCA per monitor/delay.

### Simboli da piazzare

- Ingressi RCA:
  - `J_IN_L` = `Connector_Generic:Conn_01x02`
  - `J_IN_R` = `Connector_Generic:Conn_01x02`
- Rete pad L/R:
  - `R1L`, `R1R` = `Device:R` valore `15k` (serie).
  - `R2L`, `R2R` = `Device:R` valore `4k7` (verso massa).
  - `C1L`, `C1R` = `Device:C` valore `4u7`–`10u` (AC ingresso ES8388).
- Codec:
  - `U7` = `Audio:ES8388`.
- Uscite RCA:
  - `J_OUT_L`, `J_OUT_R` = `Connector_Generic:Conn_01x02`.

### Collegamenti ingresso L (identico per R con nodo diverso)

1. **RCA L**
   - `J_IN_L` pin 1 (centrale) → `R1L` pin 1.
   - `J_IN_L` pin 2 → net `AGND` / `GND`.

2. **Pad universale L**
   - `R1L` pin 2 → nodo `IN_CODEC_L`.
   - `R2L` pin 1 → nodo `IN_CODEC_L`.
   - `R2L` pin 2 → `AGND`.
   - `C1L` pin 1 → nodo `IN_CODEC_L`.
   - `C1L` pin 2 → pin `LIN1` di `U7 (ES8388)`.

3. **Rete R**
   - Stessa struttura con:
     - nodo `IN_CODEC_R`,
     - condensatore `C1R` verso pin `RIN1` del codec.

### Collegamenti I2S tra ESP32 Master e ES8388

- Pin `SCLK` di ES8388 → net `I2S_BCLK` (Master GPIO12).
- Pin `LRCK` di ES8388 → net `I2S_WS` (Master GPIO13).
- Pin `ASDOUT` di ES8388 → net `I2S_DIN_MCU` (Master GPIO11).
- Pin `DSDIN` di ES8388 ← net `I2S_DOUT_MCU` (Master GPIO10/38 secondo la tua scelta HW finale).

### Uscite RCA da ES8388

- `LOUT1` di ES8388 → `J_OUT_L` pin 1 (centrale).
- `ROUT1` di ES8388 → `J_OUT_R` pin 1.
- `J_OUT_L` pin 2 e `J_OUT_R` pin 2 → `AGND` (punto stella analogico).

---

## Foglio 3 – Slave DMX → MAX485 #2 → XLR 3‑pin

**Obiettivo**: driver DMX dal secondo ESP32 (Slave) verso XLR 3‑pin.

### Simboli da piazzare

- Blocco `ESP32 Slave` o connettore con pin logici:
  - `GPIO8 (DMX_TX)`
  - `GPIO9 (DMX_DE)`
  - `GND`, `+5V_DMX`
- `U_DMXX` = `Interface_UART:MAX485E` (secondo MAX485).
- `J_DMX_OUT` = `Connector_Generic:Conn_01x03` oppure simbolo XLR 3‑pin.
- `C_DMX` = `Device:C`, valore `100n`.

### Collegamenti

1. **Slave → MAX485**
   - GPIO8 → pin **4 DI** (`DMX_TX`).
   - GPIO9 → pin **2 /RE** e **3 DE** (uniti).
   - GND Slave → pin **5 GND**.

2. **MAX485 → XLR**
   - Pin **7 B** → pin 2 XLR (DMX−).
   - Pin **6 A** → pin 3 XLR (DMX+).
   - GND (5) → pin 1 XLR (schermo).

3. **Alimentazione**
   - Pin **8 VCC** → net `+5V_DMX`.
   - `C_DMX` tra `+5V_DMX` e `GND` vicino a U_DMXX.

---

## Foglio 4 – Alimentazione: Pololu ORing + Buck 5 V

**Obiettivo**: sommare due sorgenti (Jack DC + Vaux RJ45) e generare 5 V per Master/Slave/codec.

### Simboli da piazzare

- `J_DC` = `Connector_Generic:Conn_01x02` (ingresso DC principale).
- `J_RJ45_VAUX` = connettore RJ45 (puoi riusare il simbolo di Foglio 1, usando solo i pin di potenza).
- `U_ORING` = simbolo custom o generico (puoi usare `Conn_02x03` annotando pin):
  - `VIN1`, `VIN1R`, `VIN2`, `VIN2R`, `VOUT`, `GND`.
- `U_BUCK5` = simbolo per step-down 5 V (o connettore verso modulo LM2596/altro).

### Collegamenti

1. **Ingressi verso Pololu**
   - `J_DC` pin + → `VIN1` (o `VIN1R`) di `U_ORING`.
   - `J_DC` pin − → `GND`.
   - Da RJ45 Vaux:
     - pin scelto come V+ (es. 7) → `VIN2` (o `VIN2R`) di `U_ORING`.
     - pin GND (es. 8) → `GND` comune.

2. **Uscita Pololu → Buck**
   - `VOUT` di `U_ORING` → ingresso positivo di `U_BUCK5`.
   - `GND` di `U_ORING` → GND di `U_BUCK5`.

3. **Uscita Buck**
   - Uscita +5 V → net `+5V` (che alimenta UEDX, eventuale Slave, ES8388, WS2812, ecc.).
   - Uscita GND → GND di sistema.

---

## Foglio 5 – Slave: Encoder EC11 + WS2812 (senza relè)

**Obiettivo**: collegare encoder Volume/Balance e due anelli WS2812 al secondo ESP32 (Slave).

### Simboli da piazzare

- `ENC_VOL` = `Device:Rotary_Encoder_Switch` (encoder Volume).
- `ENC_BAL` = `Device:Rotary_Encoder_Switch` (encoder Balance).
- `U_LED_VOL` = simbolo WS2812 (o generico) per anello Volume.
- `U_LED_BAL` = simbolo WS2812 per anello Balance.
- Connettore o net per `+5V` e `GND`.

### Collegamenti

1. **Encoder Volume (`ENC_VOL`)**
   - Pin `A` → net `GPIO15`.
   - Pin `B` → net `GPIO16`.
   - Pin `SW` → net `GPIO17`.
   - Pin comune (COM) → GND.

2. **Encoder Balance (`ENC_BAL`)**
   - Pin `A` → net `GPIO18`.
   - Pin `B` → net `GPIO21`.
   - Pin `SW` → net `GPIO22`.
   - COM → GND.

3. **Anello WS2812 Volume (`U_LED_VOL`)**
   - `DIN` → net `GPIO23`.
   - `VDD` → `+5V`.
   - `VSS` → `GND`.
   - Capacitore di disaccoppiamento: `Device:C` 100 nF (e, consigliato, 100 µF) tra 5 V e GND vicino a `U_LED_VOL`.

4. **Anello WS2812 Balance (`U_LED_BAL`)**
   - `DIN` → net `GPIO24`.
   - `VDD` → `+5V`.
   - `VSS` → `GND`.
   - Stessi condensatori di filtraggio vicino al symbol.

---

## Foglio 6 – Cablaggio Master ↔ Slave (I2S + IPC)

**Obiettivo**: documentare i 6 fili tra Master UEDX e Slave ESP32 (I2S + UART IPC + GND).

### Simboli da piazzare

- `J_MASTER_BUS` = `Connector_Generic:Conn_01x06` (lato Master).
- `J_SLAVE_BUS` = `Connector_Generic:Conn_01x06` (lato Slave).

### Assegnazione pin – lato Master

| Pin connettore `J_MASTER_BUS` | Segnale logico | GPIO reale Master |
|------------------------------:|----------------|-------------------|
| 1                             | `I2S_BCLK`     | GPIO12            |
| 2                             | `I2S_WS`       | GPIO13            |
| 3                             | `I2S_DIN_SHARED` (da ES8388 DOUT) | GPIO11 / net comune |
| 4                             | `IPC_TX`       | GPIO17            |
| 5                             | `IPC_RX`       | GPIO18            |
| 6                             | `GND`          | GND               |

### Assegnazione pin – lato Slave

| Pin connettore `J_SLAVE_BUS` | Segnale logico (stessa net) | GPIO reale Slave |
|-----------------------------:|-----------------------------|------------------|
| 1                            | `I2S_BCLK`                  | GPIO5            |
| 2                            | `I2S_WS`                    | GPIO6            |
| 3                            | `I2S_DIN_SHARED`            | GPIO7            |
| 4                            | `IPC_TX` (da Master)        | GPIO2 (RX)       |
| 5                            | `IPC_RX` (verso Master)     | GPIO1 (TX)       |
| 6                            | `GND`                       | GND              |

### Collegamenti

- Collega 1↔1, 2↔2, 3↔3, 4↔4, 5↔5, 6↔6 tra i due connettori.  
- In questo modo hai una tavola chiara del cavo a 6 fili che unisce fisicamente Master e Slave (più il cavetto dati dall’ES8388 DOUT al nodo `I2S_DIN_SHARED`).

---

Stampa questo file insieme alle tavole SVG/HTML (`schematics_electric.html`) per avere una vista sia **grafica** sia **testuale** dei pin e dei collegamenti necessari per ricostruire gli schemi in KiCad.

