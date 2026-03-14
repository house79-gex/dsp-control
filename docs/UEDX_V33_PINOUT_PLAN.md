# Piano pinout UEDX V3.3 + Slave (storico)

**Fonte vincolante pin liberi:** **[`UEDX_V33_FREE_PINS.md`](UEDX_V33_FREE_PINS.md)** (estratto SPEC: solo **GPIO 10, 17, 18** con display ON + no SD).

---

Obiettivo: **GPIO38 = solo reset touch (GT911)** come da SPEC; **I2S DAC (DOUT)** verso ES8388 senza occupare GPIO38.

---

## Problema (SPEC UEDX)

| GPIO | Uso SPEC | Uso firmware precedente |
|------|----------|-------------------------|
| **38** | **TP pin 6 – reset CTP** | I2S_DOUT → ES8388 |

Sul PCB, GPIO38 è collegato al touch: **non** usare lo stesso pin per I2S e reset senza modifica.

---

## Strategia A – **Consigliata (minimo filo extra)**

1. **Reset touch** spostato su **TCA9535** (es. **P1_0**): un filo da uscita expander al pin **RST** del GT911 (o taglio piste + wire).
2. Sul PCB, **GPIO38** non deve più pilotare il reset (solo dopo modifica); allora **GPIO38** può restare **I2S_DOUT** → **ES8388 DSDIN** come oggi nel firmware.

**Cablaggio**: nessun cambio Slave; Master `config.h` invariato per I2S (11/12/13/38).

---

## Strategia B – **Nessuna modifica touch (GPIO38 solo reset)**

Il **DAC** va pilotato dallo **Slave**, su pin **libero**:

| Segnale | Master | Slave | Verso ES8388 |
|---------|--------|-------|--------------|
| BCLK | GPIO12 | GPIO5 (in) | SCLK |
| WS | GPIO13 | GPIO6 (in) | LRCK |
| ADC → ESP | GPIO11 | GPIO7 (in) | ASDOUT |
| **DAC ← ESP** | *(non usato)* | **GPIO4 (out)** | **DSDIN** |

- Un filo **Slave GPIO4** → **ES8388 pin DSDIN** (stesso segnale che prima usciva da Master GPIO38).
- Master continua **solo lettura ADC** su GPIO11; **test tone / playback** richiede che lo **Slave** faccia **I2S Slave TX** (stessi BCLK/WS).

Firmware: `USE_SLAVE_ES8388_DOUT=1` in `config.h` (Master) + Slave `I2S_DOUT_PIN=4` e driver I2S RX+TX slave.

**GPIO Slave dopo piano**:

| GPIO Slave | Funzione |
|------------|----------|
| 1 | IPC TX |
| 2 | IPC RX |
| 4 | **I2S DOUT → ES8388** (solo strategia B) |
| 5 | I2S BCLK in |
| 6 | I2S WS in |
| 7 | I2S DIN in |
| 8 | DMX TX |
| 9 | DMX DE |
| 10 | Relay |
| 11 | Strobo |
| **3** | **Libero** (RS485 DE remoto *non* consigliato per timing) |
| 12–48 | Liberi per espansioni |

---

## Strategia C – **Encoder / LED sullo Slave (opzionale)**

Se sul Master mancano GPIO per periferiche:

| Periferica | Spostamento possibile | Note |
|------------|----------------------|------|
| Encoder Vol/Bal | Slave GPIO 12–15 + IPC | Firmware: Slave invia delta ogni N ms |
| LED ring | Slave GPIO 16 + RMT | Stesso bus 5 V |

Sconsigliato se non necessario (latency IPC).

---

## Riepilogo Master UEDX (immutato display)

| GPIO | Funzione |
|------|----------|
| 1–9,14–16,21,39–48 | LCD RGB |
| 2 | BL |
| 11 | I2S DIN |
| 12 | I2S BCLK |
| 13 | I2S WS |
| 17 | IPC TX |
| 18 | IPC RX |
| 19–20 | Touch I2C |
| **38** | **Strategia A**: DOUT *oppure* RST (mai entrambi senza HW) · **Strategia B**: solo RST touch |
| 43–44 | UART / RS485 TX RX |
| 10 | RS485 DE |
| 35–37, 26–28, 33–34 | Encoder + LED (input only) |

---

## Scelta rapida

| Situazione | Azione |
|------------|--------|
| Puoi spostare **RST touch** su **TCA9535** | **Strategia A** – firmware Master attuale |
| Touch deve restare su **GPIO38** | **Strategia B** – filo **Slave GPIO4** → ES8388 DSDIN + build con `USE_SLAVE_ES8388_DOUT` |

Vedi anche **`schematics_print_full.html`** per diagrammi stampabili.
