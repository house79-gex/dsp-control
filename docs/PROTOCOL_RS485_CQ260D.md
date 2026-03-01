# Protocollo RS-485 CQ260D – Documentazione Tecnica

> Estratto dal reverse engineering dell'applicazione Windows **KTV2i3oForm**  
> File sorgente: `docs/reverse_data_parts/reverse_data_part007.md`

---

## 1. Parametri Fisici

| Parametro       | Valore    | Note                              |
|-----------------|-----------|-----------------------------------|
| Baud Rate       | 115200    | bps                               |
| Data Bits       | 8         |                                   |
| Parity          | None (N)  |                                   |
| Stop Bits       | 1         |                                   |
| Flow Control    | None      |                                   |
| Connettore      | RS-485    | half-duplex, driver MAX485 o sim. |
| Livello logico  | 3.3V / 5V | compatibile TTL                   |

---

## 2. Struttura Frame TX (Host → DSP)

```
Offset  Byte      Tipo      Descrizione
─────────────────────────────────────────────────────────────────
0       0xFF      Header    Sync byte 1 (fisso)
1       0x55      Header    Sync byte 2 (fisso)
2       LEN       uint8     Lunghezza payload = buf[0] + 1 + 2
3       GRP       uint8     Numero gruppo DSP (mDSPID.grp)
4       ID_L      uint8     DSP ID byte basso (mDSPID.dspID & 0xFF) – Little Endian
5       ID_H      uint8     DSP ID byte alto  (mDSPID.dspID >> 8)
6..N    DATA      byte[]    Payload del comando (buf[1]..buf[N])
N+1     CHK       uint8     Checksum = somma di tutti i byte & 0xFF
```

### 2.1 Calcolo LEN

```
LEN = payload[0] + 1 + 2
    = (lunghezza dati comando) + 3
```

### 2.2 Checksum

Il checksum è la somma (senza carry) di **tutti** i byte del frame tranne l'ultimo:

```c
uint8_t calc_checksum(uint8_t* frame, uint8_t totalLen) {
    int sum = 0;
    for (int i = 0; i < totalLen; i++) sum += frame[i];
    return (uint8_t)(sum & 0xFF);
}
```

---

## 3. Struttura Frame RX (DSP → Host) – State Machine

### Diagramma di stato

```
┌─────────────────────────────────────────────────────────────┐
│                    STATE MACHINE RX                         │
│                                                             │
│  ┌─────────┐  rx==0xFF  ┌─────────┐  rx==0x55  ┌────────┐  │
│  │ STATE 0 │──────────→│ STATE 1 │──────────→│STATE 2 │  │
│  │ (attesa │           │(attesa  │           │(leggi  │  │
│  │  0xFF)  │           │  0x55)  │           │  LEN)  │  │
│  └─────────┘           └─────────┘           └────────┘  │
│       ↑                     │                     │       │
│       │ rx≠0x55            reset                  │ LEN   │
│       └─────────────────────┘               ┌─────▼────┐  │
│                                             │ STATE 3  │  │
│                                             │(accumula │  │
│                                             │  byte)   │  │
│                                             └──────────┘  │
│                                                   │        │
│                                    cnt >= msglen+1 │        │
│                                                   ▼        │
│                                          UCTRL_CmdProcessor │
└─────────────────────────────────────────────────────────────┘
```

### Implementazione pseudocodice

```c
// Variabili di stato
uint8_t  rxState  = 0;
int      msgLen   = 0;
int      rxCnt    = 0;
uint8_t  rxBuffer[601];

void process_rx_byte(uint8_t rx) {
    switch (rxState) {
        case 0:
            if (rx == 0xFF) rxState = 1;
            break;
        case 1:
            if (rx == 0x55) { rxState = 2; rxCnt = 0; }
            else rxState = 0;  // reset se non 0x55
            break;
        case 2:
            msgLen      = rx;
            rxBuffer[0] = rx;
            rxCnt       = 1;
            rxState     = 3;
            break;
        case 3:
            rxBuffer[rxCnt++] = rx;
            if (rxCnt >= msgLen + 1) {
                // Verifica checksum e processa
                if (verify_checksum(rxBuffer, rxCnt)) {
                    UCTRL_CmdProcessor();
                }
                rxState = 0;
                rxCnt   = 0;
            }
            break;
    }
}
```

### Layout TuningCmdBuffer (all'interno di UCTRL_CmdProcessor)

```
Offset  Campo        Descrizione
──────────────────────────────────────────
[0]     LEN          Lunghezza messaggio
[1]     SUB_FIELD    Sub-field (tipo risposta)
[2]     DATA_L       Dato 16-bit LE – byte basso
[3]     DATA_H       Dato 16-bit LE – byte alto
[4]     CMD_ID       Command ID (b3)
[5]     SUB_CMD      Sub-command (b4)
[6..]   PAYLOAD      Dati aggiuntivi
```

---

## 4. Comandi TX – Tabella Completa

| Codice | Hex  | Nome              | Payload                                   | Risposta attesa  |
|--------|------|-------------------|-------------------------------------------|------------------|
| 0x30   | 48   | CONNECT           | `[3, 0x30, 0, 0, 0, 0, 0, 0]`             | CMD_CONNECTED    |
| 0x31   | 49   | DOWNLOAD          | `[len, 0x31, block_L, block_H, data...]`   | CMD_DOWNLOAD_OK  |
| 0x32   | 50   | UPLOAD_REQUEST    | `[1, 0x32]`                               | CMD_UPLOAD_DATA  |
| 0x40   | 64   | SET_PARAM         | `[4, 0x40, paramType, channel, val_L, val_H]` | –            |
| 0x80   | 128  | LEVEL_REQUEST     | `[1, 0x80]`                               | CMD_LEVEL_DISPLAY|

---

## 5. Risposte RX – Tabella Completa

| CMD_ID | Hex  | Nome              | Azione                                                    |
|--------|------|-------------------|-----------------------------------------------------------|
| 0x30   | 48   | CMD_CONNECTED     | `Connected_OK = true`; inizia upload parametri           |
| 0x31   | 49   | CMD_DOWNLOAD_OK   | `DownloadOK = true`                                      |
| 0x32   | 50   | CMD_UPLOAD_DATA   | Popola SYSREG: blocchi da 50 byte, `offset = data16 * 50`|
| 0x60   | 96   | CMD_DEVICE_INFO   | `dspID = data16`; `nDeviceGroup = subCmd`                 |
| 0x80   | 128  | CMD_LEVEL_DISPLAY | 8 × int16 LE: `inputLevels[4]` + `outputLevels[4]`       |

---

## 6. Sequenza di Connessione

```
1. Apri seriale: 115200 baud, 8N1
2. Invia: [0xFF][0x55][LEN][GRP][ID_L][ID_H][3][0x30][0][0][0][0][0][0][CHK]
3. Attendi 300ms
4. Se ricevuto CMD_CONNECTED (0x30) → Connected_OK = true
5. Imposta UartRxState = 4 (inizio upload parametri)
6. Invia UPLOAD_REQUEST per ricevere SYSREG completo
7. Avvia heartbeat (contatore = 100, decremento periodico)
```

### Diagramma di sequenza

```
ESP32                                         DSP CQ260D
  │                                                │
  │──── TX: [FF 55 LEN GRP ID_L ID_H 03 30 ...] ─→│
  │                                                │
  │     (attesa ≤300ms)                            │
  │                                                │
  │←─── RX: [FF 55 ... CMD=0x30 ...] ─────────────│
  │     Connected_OK = true                        │
  │                                                │
  │──── TX: UPLOAD_REQUEST [FF 55 ... 01 32 ...] ─→│
  │                                                │
  │←─── RX: CMD_UPLOAD_DATA blocco 0 (50 byte) ───│
  │←─── RX: CMD_UPLOAD_DATA blocco 1 (50 byte) ───│
  │     (... tutti i blocchi SYSREG ...)           │
  │←─── RX: CMD_UPLOAD_DATA blocco N ─────────────│
  │                                                │
  │──── TX: LEVEL_REQUEST (heartbeat) ────────────→│
  │←─── RX: CMD_LEVEL_DISPLAY ─────────────────────│
```

---

## 7. Struttura SYSREG Completa

| Campo              | Tipo       | Dim  | Offset | Range     | Unità     | Descrizione                           |
|--------------------|------------|------|--------|-----------|-----------|---------------------------------------|
| `InGain[4]`        | uint16[4]  | 8B   | 0      | 0–65535   | lineare   | Gain canali ingresso (4 ch)           |
| `OutGain[4]`       | uint16[4]  | 8B   | 8      | 0–65535   | lineare   | Gain canali uscita                    |
| `EQ_Freq_In[124]`  | uint16[124]| 248B | 16     | 20–20000  | Hz        | Frequenza EQ ingressi (4×31 bande)   |
| `EQ_Gain_In[124]`  | uint16[124]| 248B | 264    | 0–65535   | ±24dB scal| Gain EQ ingressi                      |
| `EQ_Q_In[124]`     | uint8[124] | 124B | 512    | 0–255     | Q×10      | Q factor EQ ingressi                  |
| `EQ_Type_In[124]`  | uint8[124] | 124B | 636    | 0–5       | –         | Tipo filtro: 0=PEQ,1=LSh,2=HSh,3=LP,4=HP,5=Notch |
| `EQ_Freq[28]`      | uint16[28] | 56B  | 760    | 20–20000  | Hz        | Frequenza EQ uscite (4×7 bande)      |
| `EQ_Gain[28]`      | uint16[28] | 56B  | 816    | 0–65535   | ±24dB scal| Gain EQ uscite                        |
| `EQ_Q[28]`         | uint8[28]  | 28B  | 872    | 0–255     | Q×10      | Q factor EQ uscite                    |
| `EQ_Type[28]`      | uint8[28]  | 28B  | 900    | 0–5       | –         | Tipo filtro EQ uscite                 |
| `EQ_Bypass[8]`     | uint8[8]   | 8B   | 928    | 0–1       | –         | Bypass EQ uscite                      |
| `HLPF_Freq[8]`     | uint16[8]  | 16B  | 936    | 20–20000  | Hz        | Freq HPF[0..3]+LPF[4..7]             |
| `HLPF_Type[8]`     | uint8[8]   | 8B   | 952    | 0–2       | –         | 0=12dB/oct,1=24dB/oct,2=48dB/oct    |
| `HLPF_Bypass[8]`   | uint8[8]   | 8B   | 960    | 0–1       | –         | Bypass HPF/LPF                        |
| `Delay[4]`         | uint16[4]  | 8B   | 968    | 0–65535   | campioni  | Delay (÷48000 = secondi)             |
| `Threhold_Noise[4]`| uint8[4]   | 4B   | 976    | 0–255     | –         | Soglia noise gate                     |
| `ATack_Noise[4]`   | uint8[4]   | 4B   | 980    | 0–255     | ms        | Attack noise gate                     |
| `Decay_Noise[4]`   | uint8[4]   | 4B   | 984    | 0–255     | ms        | Decay noise gate                      |
| `Threhold[4]`      | uint8[4]   | 4B   | 988    | 0–255     | –         | Soglia compressore                    |
| `ATack[4]`         | uint8[4]   | 4B   | 992    | 0–255     | ms        | Attack compressore                    |
| `Decay[4]`         | uint8[4]   | 4B   | 996    | 0–255     | ms        | Release compressore                   |
| `CompRatio[4]`     | uint8[4]   | 4B   | 1000   | 1–20      | ratio:1   | Ratio compressore                     |
| `CompMakeup[4]`    | uint8[4]   | 4B   | 1004   | 0–48      | dB        | Makeup gain compressore               |
| `Mute[4]`          | uint8[4]   | 4B   | 1008   | 0–1       | –         | Mute: 1=muto                          |
| `Phase[4]`         | uint8[4]   | 4B   | 1012   | 0–1       | –         | Fase: 1=invertito                     |
| `MixVol[16]`       | uint16[16] | 32B  | 1016   | 0–65535   | lineare   | Matrice mixer (4in × 4out)           |
| `MixRoute[16]`     | uint8[16]  | 16B  | 1048   | –         | –         | Routing matrice mixer                 |
| `masterVol`        | uint16     | 2B   | 1064   | 0–65535   | lineare   | Volume master globale                 |
| `mode`             | uint8      | 1B   | 1066   | –         | –         | Modalità operativa                    |
| `lock`             | uint8      | 1B   | 1067   | 0–1       | –         | Lock pannello                         |
| `password[6]`      | uint8[6]   | 6B   | 1068   | –         | ASCII     | Password 6 cifre                      |
| `bootChar[7]`      | uint8[7]   | 7B   | 1074   | –         | ASCII     | Stringa avvio display                 |
| `ver`              | uint8      | 1B   | 1081   | –         | –         | Versione firmware DSP                 |

---

## 8. Heartbeat Protocol

```
- Contatore: inizializzato a 100 alla connessione
- Decremento: ad ogni tick del loop principale (~10ms)
- Azione a 0: invia LEVEL_REQUEST, resetta contatore a 100
- Effetto: heartbeat ogni ~1s
- Scopo: mantenere connessione attiva + ricevere livelli VU meter
```

---

## 9. Timing Constraints

| Vincolo                  | Valore     | Note                              |
|--------------------------|------------|-----------------------------------|
| Delay tra frame TX       | 30ms       | `SendDelay` – minimo raccomandato |
| Timeout risposta CONNECT | 300ms      | `DSP_CONNECT_TIMEOUT_MS`          |
| Dimensione max frame     | 601 byte   | `DSP_MAX_FRAME_SIZE`              |
| Dimensione blocco upload | 50 byte    | `DSP_UPLOAD_BLOCK_SIZE`           |
| Baud rate                | 115200     | 8N1                               |

---

## 10. Esempi Frame Esadecimali

### 10.1 Richiesta Connessione

```
FF 55 06 00 00 00 03 30 00 00 00 00 00 00 33
│  │  │  │  │  │  └──────────────────────┘  │
│  │  │  │  │  │  Payload (8 byte)           │
│  │  │  │  │  └─ ID_H = 0 (broadcast)      │
│  │  │  │  └──── ID_L = 0 (broadcast)      │
│  │  │  └─────── GRP = 0                   │
│  │  └─────────── LEN = 6                  │
│  └────────────── Sync 2                   │
└──────────────── Sync 1                    CHK
```

### 10.2 Risposta CMD_CONNECTED

```
FF 55 05 01 00 00 00 30 CHK
                        └─── CMD_ID = 0x30 = CONNECTED
```

### 10.3 Imposta Gain Canale 1 (0dB → valore 0x8000 = 32768)

```
FF 55 08 00 01 00 04 40 01 00 00 80 CHK
                     │  │  │  │  └─ val_H (0x80)
                     │  │  │  └──── val_L (0x00) → valore 0x8000
                     │  │  └─────── channel = 0
                     │  └────────── paramType = 0x01 (gain uscita)
                     └───────────── cmd = 0x40 (SET_PARAM)
```

---

## 11. Endianness

Tutti i valori multi-byte nel protocollo CQ260D sono **Little-Endian (LE)**:

```
uint16_t value = 0x1234
  → byte[0] = 0x34 (LSB)
  → byte[1] = 0x12 (MSB)

Esempio: DspID = 0x0003
  ID_L = 0x03
  ID_H = 0x00
```

---

## 12. Note Implementative

1. **Broadcast**: usare `GRP=0, ID_L=0xFF, ID_H=0xFF` per indirizzare tutti i dispositivi
2. **Unicast**: usare l'ID specifico rilevato durante la sequenza di connessione
3. **Half-duplex**: commutare DE/RE del MAX485 prima di trasmettere, tornare in ricezione dopo il flush
4. **Checksum**: include TUTTI i byte del frame (sync, LEN, GRP, ID, payload), NON il byte checksum stesso
5. **Nome campo**: `Threhold` e `ATack` sono scritti con questo typo nel firmware originale; mantenere per compatibilità

---

## 13. Nota: WLED – Protocollo Wi-Fi Separato

Il controllo delle **strip neon LED WS2811** montate sulle casse è gestito tramite un
protocollo **completamente separato** rispetto a RS-485:

| Aspetto           | RS-485 (CQ260D)          | WLED (Neon LED)                  |
|-------------------|--------------------------|----------------------------------|
| Mezzo fisico      | RS-485 half-duplex       | Wi-Fi 802.11 b/g/n               |
| Protocollo        | Frame binario proprietario | HTTP JSON API + UDP 21324       |
| Controller target | DSP CQ260D / PDA1001     | GLEDOPTO GL-C-015WL-D (ESP32)   |
| API firmware      | `rs485.h/cpp`            | `wled_client.h/cpp`              |
| Terminazione      | 120Ω sull'ultimo device  | Non applicabile (Wi-Fi)          |

Il terminatore da 120Ω è richiesto **solo** sull'ultimo dispositivo della catena RS-485.
Le strip WS2811 usano un protocollo unidirezionale a un filo (DATA) e **non** richiedono terminazione.
