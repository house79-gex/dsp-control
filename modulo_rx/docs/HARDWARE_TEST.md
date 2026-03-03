# Modulo RX – Procedure di Test Hardware

Questo documento descrive le procedure di validazione hardware per il Modulo RX wireless, con criteri di accettazione e istruzioni per l'esecuzione dei test.

---

## Attrezzatura Necessaria

| Strumento | Modello (esempio) | Uso |
|-----------|-------------------|-----|
| Oscilloscopio dual-channel | Rigol DS1054Z | Test latenza, forma d'onda |
| Audio analyzer | QuantAsylum QA403 | THD+N, SNR |
| Multimetro | Fluke 87V | Misure tensioni |
| Sorgente audio | Denon DJ SC LIVE 4 | Segnale XLR di riferimento |
| PC con Serial monitor | PlatformIO / Arduino IDE | Log packet loss |

---

## Test 1: Latenza End-to-End

**Obiettivo:** Verificare che la latenza dal segnale XLR di ingresso all'uscita DAC del Modulo RX sia inferiore a 30 ms.

**Target:** latenza < 30 ms

### Setup

```
SC LIVE 4 XLR OUT ──► Oscilloscopio CH1
                  └──► ESP32 Master TX (ADC ES8388)
                              │ ESP-NOW wireless
                              ▼
                       Modulo RX (DAC ES8388) ──► Oscilloscopio CH2
```

### Procedura

1. Collegare il canale CH1 dell'oscilloscopio all'uscita XLR del SC LIVE 4.
2. Collegare il canale CH2 all'uscita DAC analogica del Modulo RX (jack 3.5mm o RCA).
3. Sul SC LIVE 4 avviare la riproduzione di un'onda quadra a 1 kHz (file WAV incluso in `/test/assets/1khz_square.wav`).
4. Sull'oscilloscopio impostare il trigger su CH1 (rising edge) e misurare il delay del rising edge di CH2.

### Misura

- **Cursore T1:** rising edge CH1 (segnale sorgente)
- **Cursore T2:** rising edge CH2 (segnale uscita Modulo RX)
- **Latenza misurata:** ΔT = T2 − T1

### Criteri di Accettazione

| Parametro | Valore Target | Pass/Fail |
|-----------|---------------|-----------|
| Latenza (ΔT) | < 30 ms | [ ] |
| Jitter (variazione ΔT) | < 2 ms | [ ] |

---

## Test 2: Packet Loss

**Obiettivo:** Verificare che la percentuale di pacchetti persi durante 1 ora di riproduzione continua sia inferiore a 0,5%.

**Target:** packet loss < 0,5%

### Setup

1. Collegare il Modulo RX al PC tramite USB (Serial monitor a 115200 baud).
2. Avviare la riproduzione audio continua sul SC LIVE 4 (playlist 1 ora, qualsiasi contenuto).
3. Aprire il monitor seriale: `pio device monitor -e esp32s3`.

### Procedura

1. Reset del Modulo RX (pulsante EN) per azzerare i contatori.
2. Avviare la riproduzione e il timer (1 ora).
3. Ogni 5 secondi il modulo stampa:
   ```
   [STATS CH1] Ricevuti=1000 Persi=2 Ignorati=0 CRC_ERR=0 Loss=0.20% AudioUnderrun=0 AudioFrames=998
   ```
4. Al termine dell'ora, registrare i valori finali.

### Calcolo

```
Loss Rate (%) = (Persi / (Ricevuti + Persi)) × 100
```

### Criteri di Accettazione

| Parametro | Valore Target | Pass/Fail |
|-----------|---------------|-----------|
| Packet loss rate | < 0,5% | [ ] |
| CRC errors | < 10 / ora | [ ] |
| Audio underrun | 0 | [ ] |

---

## Test 3: Qualità Audio (THD+N)

**Obiettivo:** Verificare che la distorsione armonica totale più rumore (THD+N) con segnale sinusoidale 1 kHz a −10 dBFS sia inferiore a 0,01%.

**Target:** THD+N < 0,01%

### Setup

```
Audio Analyzer OUT (1 kHz, −10 dBFS) ──► SC LIVE 4 LINE IN
                                               │ (via Master TX + Modulo RX)
                                               ▼
                                        Modulo RX DAC OUT ──► Audio Analyzer IN
```

### Procedura

1. Configurare l'audio analyzer per generare 1 kHz sine, livello −10 dBFS, su CH1 OUT.
2. Collegare l'uscita all'ingresso LINE-IN del SC LIVE 4 (sorgente per il Master TX).
3. Collegare l'uscita DAC del Modulo RX all'ingresso dell'audio analyzer.
4. Sul QA403: selezionare "THD+N" measurement, frequenza 1 kHz, BW 20–20000 Hz.
5. Registrare i valori dopo 30 secondi di stabilizzazione.

### Criteri di Accettazione

| Parametro | Valore Target | Pass/Fail |
|-----------|---------------|-----------|
| THD+N | < 0,01% (−80 dBFS) | [ ] |
| SNR | > 90 dB | [ ] |
| Risposta in frequenza (20 Hz – 20 kHz) | ±0,5 dB | [ ] |
| Diaphonie (crosstalk L/R) | < −60 dB | [ ] |

---

## Test 4: Connessione Wireless (range e stabilità)

**Obiettivo:** Verificare il corretto handshake ESP-NOW e la stabilità della connessione a diverse distanze.

### Procedura

1. Posizionare Master TX e Modulo RX a 1 m di distanza (linea di vista).
2. Avviare riproduzione → verificare LED verde fisso su Modulo RX.
3. Portare a 5 m, 10 m, 20 m (con ostacoli tipici di un venue: pareti, pubblico).
4. Verificare che il LED rimanga verde (loss < 5%) o passi a giallo (loss 5–15%).

### LED Status

| LED | Stato | Significato |
|-----|-------|-------------|
| Verde fisso | Online OK | Loss < 5%, RSSI > −70 dBm |
| Giallo lampeggiante | Warning | Loss 5–15% o RSSI −70/−85 dBm |
| Rosso lampeggiante | Offline | Nessun pacchetto da > 3 s |

### Criteri di Accettazione

| Distanza | Stato Atteso | Pass/Fail |
|----------|-------------|-----------|
| 1 m LOS | LED Verde, loss 0% | [ ] |
| 10 m LOS | LED Verde, loss < 0,5% | [ ] |
| 20 m con ostacoli | LED Verde/Giallo, loss < 2% | [ ] |

---

## Script di Test Automatico (Loopback)

Vedere il file [`../test/test_loopback.cpp`](../test/test_loopback.cpp) per il test automatico di loopback audio ES8388.

---

## Risultati Test

Compilare questa tabella dopo l'esecuzione dei test:

| Test | Data | Operatore | Risultato | Note |
|------|------|-----------|-----------|------|
| T1 Latenza | | | PASS / FAIL | |
| T2 Packet Loss | | | PASS / FAIL | |
| T3 THD+N | | | PASS / FAIL | |
| T4 Wireless Range | | | PASS / FAIL | |
