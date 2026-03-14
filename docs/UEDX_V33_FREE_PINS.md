# UEDX80480050E-WB-A V3.3 – Pin occupati vs liberi (datasheet ufficiale)

Fonte: **`docs/UEDX80480050E-WB-A-V3.3-SPEC.pdf`**, §2.1 Interface J2, Note pag. 8.

Il display RGB è **saldato internamente**: i GPIO sotto sono **impegnati dal pannello** e **non** sono riassegnabili senza spegnere il LCD.

---

## 1. Citazione datasheet (fondamentale)

> *"When using Display interface, most of the pins are occupied because the RGB interface is used."*  
> *"GPIO17 and GPIO18 are **not used**, you are free to use them"*  
> *"If the Display interface is used but the **SD is not used**, then the **SD pins are freely usable**."*

---

## 2. GPIO occupati dal LCD (Current Usage = LCD …)

| GPIO | Uso sul modulo |
|------|----------------|
| 1 | LCD B4 |
| 2 | LCD-BL-EN (backlight) |
| 3 | LCD B1 |
| 4 | LCD G5 |
| 5 | LCD G0 |
| 6 | LCD G1 |
| 7 | LCD G2 |
| 8 | LCD B0 |
| 9 | LCD B3 |
| 14 | LCD R4 |
| 15 | LCD G3 |
| 16 | LCD G4 |
| 21 | LCD R3 |
| 39 | LCD HS |
| 40 | LCD DE |
| 41 | LCD VS |
| 42 | LCD PCLK |
| 45 | LCD R0 |
| 46 | LCD B2 |
| 47 | LCD R2 |
| 48 | LCD R1 |

**Totale: 22 GPIO solo per RGB + sync + backlight** → non toccare.

---

## 3. GPIO impegnati da touch + reset (hardware CTP)

| GPIO | Uso |
|------|-----|
| 19 | TP SDA (I2C) |
| 20 | TP SCL (I2C) |
| **38** | **TP pin 6 – RTP-csb-CTP-rst** (reset GT911, **fisso sul PCB**) |

**GPIO38 non va usato come I2S_DOUT** sul modulo così com’è: è il reset del touch.

---

## 4. GPIO RTP / I2S verso codec (già assegnati nel firmware)

| GPIO | Current Usage (SPEC) | Funzione tipica codec |
|------|----------------------|------------------------|
| 11 | RTP-DIN | I2S DIN ← ES8388 ASDOUT |
| 12 | RTP-CLK | I2S BCLK |
| 13 | RTP-DOUT | I2S WS (e/o data verso pannello – restano bus I2S codec) |

Questi tre sono **già il bus I2S**; non sono “liberi” per altro.

---

## 5. UART di bordo (debug / seriale)

| GPIO | Uso |
|------|-----|
| 43 | U0TXD (UARTTX) |
| 44 | U0RXD (UARTRX) |

Restano per seriale/flash; **RS485 UART2** sul firmware usa gli stessi 43/44 per MAX485 (TX/RX).

---

## 6. Pin **liberi** con display ON + **nessuna SD**

### 6.1 Esplicitamente liberi (nota datasheet)

| GPIO | Note |
|------|------|
| **17** | U1TXD – **Not used** (IPC verso Slave) |
| **18** | U1RXD – INT touch “not used” sul TP pin 3 (**IPC RX**) |

### 6.2 Liberi **solo senza SD**

| GPIO | Current Usage senza SD |
|------|-------------------------|
| **10** | In tabella: **SD-CS** (FSPICS0, …). **Senza scheda SD → non usato dal socket SD** → **libero per firmware** |

L’unico pin nominato “SD” nella tabella J2 è **GPIO10**. Gli altri segnali FSPI (11–13) sul chip sono **già impiegati** come RTP-DIN/CLK/DOUT, **non** come SD sul prodotto finito.

Quindi, con **SD non montata / non usata**:

- **Liberi reali su J2: GPIO 10, 17, 18** → **3 GPIO**.

---

## 7. Assegnazione consigliata (solo pin liberi + Slave)

| Funzione | GPIO Master | Motivo |
|----------|-------------|--------|
| IPC TX | **17** | Datasheet: free |
| IPC RX | **18** | Datasheet: free |
| **I2S DOUT → ES8388 DSDIN** | **10** | SD non usata; **libera GPIO38** per reset touch |
| RS485 DE | **Non su 10** | Conflitto con DOUT |

**RS485:** con **GPIO10 = I2S_DOUT** non resta alcun GPIO Master per **DE**. Opzioni:

1. **Modulo RS-485 auto-direzionale** (nessun DE/re da MCU) → consigliato.  
2. **DE su ESP32 Slave** (es. GPIO3) + filo al MAX485 sul Master (timing da validare).  
3. **MAX485 sempre in TX** (solo broadcast) – sconsigliato per RX.

---

## 8. Encoder / LED ring (35–37, 27, 33–34, 26, 28)

Il **datasheet J2 non elenca** GPIO35, 36, 37, … → **non garantiti** su header UEDX. Vanno usati **solo se** presenti sul **tuo** connettore laterale o su espansione; altrimenti spostare encoder sullo **Slave** (GPIO liberi).

---

## 9. Riepilogo una riga

| Condizione | GPIO Master user-free |
|------------|------------------------|
| Display ON, Touch ON, SD **no** | **10, 17, 18** (3 pin) |
| Display ON, SD sì | Solo **17, 18** (10 occupato da SD-CS) |

**GPIO38** = solo reset touch (interno). **I2S DOUT** → **GPIO10** se no SD.

---

*Documento vincolante per cablaggio; allineato a SPEC V3.3 2025-01-08.*
