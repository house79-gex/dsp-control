# Riferimento datasheet (PDF in `docs/`)

Sintesi operativa estratta dai PDF presenti in repository. Per dettagli originali aprire il file indicato.

---

## 1. UEDX80480050E-WB-A-V3.3-SPEC.pdf

| Voce | Valore (da SPEC) |
|------|-------------------|
| Modello | UEDX80480050E-WB-A |
| MCU | ESP32-S3-WROOM-1 MCN16R8, 240 MHz |
| Flash / PSRAM | 16 MB / 8 MB OPI |
| Alimentazione | **DC 5 V**, tip. **300 mA** max (backlight), ~150 mA backlight off |
| Range tensione | 4.0 – 5.5 V |
| Alimentatore consigliato | **5 V ≥ 1 A** |
| Display | 5" 800×480 IPS, RGB 24 bit, ST7262, touch **GT911** |
| USB | Type-C (alimentazione + seriale CH340C) |

### Connettore **J2 – GPIO esterni** (occupazione con display RGB attivo)

| Pin J2 | GPIO | Uso sul modulo (SPEC) |
|--------|------|------------------------|
| — | GPIO1 | LCD B4 |
| — | GPIO2 | LCD-BL-EN (backlight) |
| — | GPIO3 | LCD B1 |
| — | GPIO4 | LCD G5 |
| — | GPIO5 | LCD G0 |
| — | GPIO6 | LCD G1 |
| — | GPIO7 | LCD G2 |
| — | GPIO8 | LCD B0 |
| — | GPIO9 | LCD B3 |
| — | GPIO10 | SD-CS (libero se no SD) |
| — | **GPIO11** | **RTP-DIN** (= I2S data **da codec verso ESP** / ADC out) |
| — | **GPIO12** | **RTP-CLK** (= I2S **BCLK**) |
| — | **GPIO13** | **RTP-DOUT** (= linea I2S verso codec; in standard I2S spesso **WS/LRCK** o data out – allineare a `config.h`) |
| — | GPIO14 | LCD R4 |
| — | GPIO15 | LCD G3 |
| — | GPIO16 | LCD G4 |
| — | **GPIO17** | **U1TXD – libero** se display ok (firmware: **IPC TX**) |
| — | **GPIO18** | INT touch “not used” su TP (firmware: **IPC RX**) |
| — | GPIO19 | TP SDA |
| — | GPIO20 | TP SCL |
| — | GPIO21 | LCD R3 |
| — | **GPIO38** | Su **TP pin 6**: **RTP-csb-CTP-rst** (reset GT911) |
| — | GPIO39–42 | LCD HS, DE, VS, PCLK |
| — | GPIO43/44 | UART0 TX/RX |
| — | GPIO45–48 | LCD R0–R2, B2, R1 |

**Attenzione GPIO38:** sulla SPEC, **GPIO38** è il **reset touch**. Il firmware usa **GPIO38 = I2S_DOUT** verso il DAC. Sul PCB UEDX, GPIO38 è collegato al touch: **non usare contemporaneamente** reset GT911 e I2S DOUT sullo stesso pin senza modifica hardware. Soluzioni: (1) codec **M5 M144** su bus separato con cavi verso altri GPIO se riconfiguri firmware; (2) **TCA9535** per reset touch (come già nota v2.0) e **spostare I2S_DOUT** su GPIO libero solo se il touch non usa più GPIO38 (verificare schema PCB V3.3).

### Interfaccia touch (TP)

| Pin TP | Segnale |
|--------|---------|
| 1 | GPIO20 – SCL |
| 2 | GPIO19 – SDA |
| 3 | GPIO18 – INT (documentato come non usato) |
| 6 | **GPIO38 – reset CTP** |

---

## 2. M144_sch_moduleaudio_v10.pdf (M5 Stack – modulo audio ES8388)

Codec **U7 ES8388** (QFN-28), jack cuffie **J1/J2 PJ-342B**, connettore **BUS1 M5_BUS_Core2**.

### ES8388 – collegamenti rilevanti (da schematico)

| Pin ES8388 | Nome | Funzione |
|------------|------|----------|
| 5 | SCLK | **I2S BCLK** |
| 6 | DSDIN | **I2S SDIN** (DAC dati **verso** codec) |
| 7 | LRCK | **I2S WS / LRCK** |
| 8 | ASDOUT | **I2S DOUT** (ADC dati **da** codec verso MCU) |
| 1 | MCLK | Master clock (opz.; modulo ha switch **S1** tra percorsi MCLK) |
| 22–23 | LIN2, RIN2 | Ingresso linea 2 |
| 23–24 | LIN1, RIN1 | Ingresso linea 1 |
| 11–12 | LOUT1, ROUT1 | Uscita verso **jack cuffia** (serie 22 Ω) |
| 17 | AVDD | Analog supply |
| 18 | AGND | Massa analogica |

Segnali scheda verso bus: **I2S_MCLK**, **I2S_SCLK**, **I2S_LRCK**, **I2S_MAIN_DOUT**, **I2S_MAIN_DIN**, **CODEC_LIN1/RIN1**, **CODEC_LIN2/RIN2**, **BUS_SDA**, **BUS_SCL**.

### Connettore BUS (estratto netlist PDF)

| Posizione BUS | Segnale tipico |
|---------------|----------------|
| G18 | SCK (I2S clock sul modulo) |
| G21 / G22 | SYS_SDA / SYS_SCL (I2C codec) |
| G23 / G25 / G26 / G38 | MOSI, DAC, DAC, MISO (uso Core M5; per I2S verso ESP32 esterno collegare **BCLK/LRCK/DIN/DOUT** ai pin ES8388 equivalenti o al header I2S del modulo) |
| 5V / 3.3V | Alimentazione modulo |

**Uso con UEDX:** collegare **I2S** del modulo M144 ai GPIO **11/12/13/38** del firmware **solo se** GPIO38 non è in conflitto con touch; in caso contrario spostare **I2S_DOUT** nel firmware su un GPIO libero (es. SD-CS GPIO10 **solo se** si rinuncia a SD e si riconfigura I2S – **GPIO10 è anche RS485 DE** sul firmware attuale → **meglio filo diretto ES8388** o modulo su cavo corto con pin I2S dedicati).

---

## 3. ES8388.pdf (Everest Semiconductor)

- ADC/DAC **24 bit**, **8–96 kHz**
- Alimentazione **1.8–3.3 V**
- Interfaccia **I2C** (o SPI) + **I2S** Master/Slave
- Uscita cuffia ~40 mW; **LOUT1/ROUT1** per line out

Registri e sequenza power-up: vedere PDF sezione 6.

---

## 4. Relè Module interface.pdf

| Pin | Funzione |
|-----|----------|
| **DC+** | Positivo alimentazione bobina (tensione nominale relè) |
| **DC−** | Negativo |
| **IN** | Controllo: livello HIGH o LOW attiva bobina (secondo jumper) |
| **NO** | Normalmente aperto |
| **COM** | Comune |
| **NC** | Normalmente chiuso |
| Jumper | **LOW** = trigger su livello **basso**; **HIGH** = trigger su livello **alto** |

Pilotaggio da **3.3 V MCU**: uscita GPIO/expander → transistor NPN/MOSFET o modulo a **optocoppia** se bobina 5 V/12 V.

---

## 5. Encoder rotativo.pdf

| Voce | Valore |
|------|--------|
| Modello | **EC11** (tipo potenziometro encoder) |
| Tensione | **5 V DC** |
| Rotazione | 360° continua |
| Perni | **7 pin** (tipicamente A, B, comune, pulsante, + alimentazione LED se presente) |
| Albero | 20 mm |

Cablaggi tipici EC11: **A, B** a GPIO con pull-up; **COM** a GND; **SW** pulsante a GPIO (pull-up interno).

---

## 6. pololu-power-oring-ideal-diode-pair.pdf + Pololu #5398

| Voce | Valore |
|------|--------|
| Tensione | **4–60 V** (da avvio ~3.2 V) |
| Corrente | **fino a 6 A per canale** (resistenze bilanciamento); bypass resistenze → limiti termici MOSFET |
| Funzione | Due **ideal diode** in parallelo in uscita → **OR** di due alimentatori |

### Connessioni (testo Pololu)

| Pad | Uso |
|-----|-----|
| **VIN1** / **VIN1R** | Ingresso 1 (VIN1R attraverso ~20 mΩ bilanciamento) |
| **VIN2** / **VIN2R** | Ingresso 2 |
| **VOUT** | Uscita OR verso carico |
| **GND** | Comune |

Per bypass resistenza: collegare **VINx** direttamente (fori piccoli) o ponticellare VINx–VINxR. Due sorgenti alla **stessa tensione** per load sharing equilibrato.

---

## 7. MAX485.pdf

Transceiver **RS-485** half-duplex (tipico **MAX485/SP485**):

| Pin | Funzione |
|-----|----------|
| RO | Receive Out → RX MCU |
| RE | Receive Enable (attivo basso) |
| DE | Driver Enable |
| DI | Data In ← TX MCU |
| GND | |
| A, B | Bus differenziale |
| VCC | 5 V (moduli 3.3 V-compatibili esistono) |

**DE + RE** spesso collegati insieme: HIGH = trasmetti; LOW = ricevi.

---

## 8. Led Ring WS2811.pdf

Anello LED **WS2811/WS2812** (12 LED tipici):

- Alimentazione **5 V** (WS2811 a volte 12 V strip – verificare tuo anello; molti anelli encoder sono **5 V WS2812**)
- **DIN** dati da MCU (3.3 V spesso ok; altrimenti level shifter)
- **GND** comune
- Condensatore **100 nF–10 µF** vicino al primo LED consigliato

Firmware: **NeoPixel** su **GPIO27** (vol) e **GPIO28** (bal).

---

## Indice file PDF

| File in `docs/` | Contenuto |
|-----------------|-----------|
| UEDX80480050E-WB-A-V3.3-SPEC.pdf | Display 5" + ESP32-S3, J2, TP, alimentazione |
| M144_sch_moduleaudio_v10.pdf | M5 modulo audio ES8388 + BUS |
| ES8388.pdf | Datasheet chip |
| Relè Module interface.pdf | Pin relè modulo |
| Encoder rotativo.pdf | EC11 5 V 7 pin |
| pololu-power-oring-ideal-diode-pair.pdf | Dimensioni + #5398 |
| MAX485.pdf | Transceiver RS-485 |
| Led Ring WS2811.pdf | Anello LED |

---

*Aggiornato in base all’estrazione testo dai PDF in repository.*
