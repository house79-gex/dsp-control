# Sistema Wireless 2 Canali – Documentazione Tecnica

## 1. Architettura del Sistema

```
┌─────────────────────────────────────┐
│          MASTER TX (ESP32-S3)       │
│   WiFi AP + REST API + ESP-NOW TX   │
│                                     │
│  ┌──────────┐   ┌──────────────┐    │
│  │ ES8388   │   │  DMX512 +    │    │
│  │ ADC/DAC  │   │  RS-485      │    │
│  └──────────┘   └──────────────┘    │
│         │               │           │
│    wireless_tx.h/cpp                │
│         │               │           │
└─────────┼───────────────┼───────────┘
          │    ESP-NOW    │
    ┌─────┴──────┐  ┌─────┴──────┐
    │  RX CH1    │  │  RX CH2    │
    │ (ESP32-S3) │  │ (ESP32-S3) │
    │            │  │            │
    │ ES8388 DAC │  │ ES8388 DAC │
    │ RS-485 OUT │  │ RS-485 OUT │
    │ DMX512 OUT │  │ DMX512 OUT │
    └────────────┘  └────────────┘
```

---

## 2. Specifiche Tecniche

| Parametro          | Valore                    |
|--------------------|---------------------------|
| Protocollo         | ESP-NOW (802.11)          |
| Range stimato      | 50-200m (line of sight)   |
| Latenza audio      | <15ms (1ms frame + jitter buffer) |
| Sample rate        | 48kHz 16bit stereo        |
| Frame audio        | 48 campioni @ 1ms         |
| Frequenza DMX      | 40Hz (ogni 25ms)          |
| Max canali DMX     | 512 (3 chunk da 200 byte) |
| Max payload RS-485 | 64 byte per frame         |
| Timeout offline    | 3 secondi                 |
| Throughput audio   | ~1.5 Mbps (incluso overhead) |

---

## 3. Modalità Operative Audio

### STEREO_SPLIT (default)
- **CH1**: canale sinistro (Left) dal codec ES8388 stereo
- **CH2**: canale destro (Right) dal codec ES8388 stereo
- Uso: sistemi stereo con amplificatori separati per L e R

### DUAL_MONO
- **CH1**: segnale stereo completo (copia identica)
- **CH2**: segnale stereo completo (copia identica)
- Uso: due zone audio identiche (es. palco + foyer)

### MAIN_SUB
- **CH1**: segnale full range stereo (20Hz–20kHz)
- **CH2**: segnale sub monofonico (<120Hz, mix L+R)
- Uso: sistema main + subwoofer wireless separati

---

## 4. Protocollo Frame ESP-NOW

### Frame Audio (`magic = 0xAA55`)
```c
struct AudioFramePerChannel {
    uint16_t magic;         // 0xAA55
    uint8_t  channelId;     // 1 o 2
    uint8_t  flags;         // riservato
    uint16_t seqNum;        // contatore sequenza per canale
    uint32_t timestampUs;   // sync microsecondo
    uint16_t sampleRate;    // 48000
    uint8_t  audioMode;     // 0=Mono, 1=Stereo, 2=Sub
    uint8_t  numSamples;    // 48
    int16_t  samples[96];   // max 48 campioni stereo
    uint8_t  crc8;          // CRC8 del frame
} __attribute__((packed));  // 213 byte totali
```

### Frame RS-485 (`magic = 0xBB66`)
```c
struct Rs485FrameWireless {
    uint16_t magic;         // 0xBB66
    uint8_t  channelId;     // 1 o 2
    uint8_t  dataLen;       // lunghezza payload (max 64)
    uint8_t  data[64];      // payload RS-485 DSP
    uint8_t  crc8;
} __attribute__((packed));  // 69 byte totali
```

### Frame DMX (`magic = 0xCC77`)
```c
struct DmxFrameWireless {
    uint16_t magic;         // 0xCC77
    uint8_t  channelId;     // 1, 2 o 0xFF (broadcast)
    uint8_t  chunkIdx;      // 0, 1, 2
    uint16_t startCh;       // canale DMX iniziale
    uint8_t  numCh;         // canali nel chunk (max 200)
    uint8_t  dmxData[200];  // valori DMX
    uint8_t  crc8;
} __attribute__((packed));  // 207 byte totali
```

**CRC8**: Polinomio 0x07, calcolato su tutto il frame eccetto l'ultimo byte (crc8).

**Rilevamento perdita pacchetti**: Il ricevitore confronta `seqNum` con quello atteso.  
Se `seqNum` non è `lastSeq + 1`, i pacchetti persi vengono contati.

---

## 5. Schema Elettrico Modulo RX

```
ESP32-S3 DevKitC-1
┌──────────────────────────────────────┐
│  GPIO15 ──── BCLK  ──► ES8388        │
│  GPIO16 ──── LRCK  ──► ES8388        │
│  GPIO17 ──── DIN   ──► ES8388        │
│  GPIO5  ──── SDA   ──► ES8388 (I2C)  │
│  GPIO6  ──── SCL   ──► ES8388 (I2C)  │
│  3.3V   ──── VDD   ──► ES8388        │
│  GND    ──── GND   ──► ES8388        │
│                                       │
│  GPIO43 ──── DI    ──► MAX485 RS-485  │
│  GPIO44 ──── RO    ◄── MAX485 RS-485  │
│  GPIO21 ──── DE/RE ──► MAX485         │
│  3.3V   ──── VCC   ──► MAX485         │
│                                       │
│  GPIO1  ──── DI    ──► DMX485         │
│  GPIO2  ──── DE    ──► DMX485         │
│  3.3V   ──── VCC   ──► DMX485         │
└──────────────────────────────────────┘

ES8388 → Amplificatore → Altoparlante
MAX485 → XLR RS-485 → CQ260D DSP
DMX485 → XLR DMX OUT → Controller luci
```

---

## 6. BOM – 2 Moduli RX (circa €59)

| Componente              | Qty | Prezzo unit. | Totale |
|-------------------------|-----|--------------|--------|
| ESP32-S3 DevKitC-1      | 2   | €8.00        | €16.00 |
| Modulo ES8388 I2S       | 2   | €5.50        | €11.00 |
| MAX485 TTL↔RS-485       | 2   | €0.80        | €1.60  |
| DMX485 TTL↔DMX512       | 2   | €1.20        | €2.40  |
| PCB perforata           | 2   | €1.50        | €3.00  |
| Connettore XLR 3pin M   | 4   | €1.00        | €4.00  |
| Connettore XLR 5pin F   | 2   | €1.20        | €2.40  |
| Alimentatore 5V 1A USB  | 2   | €4.00        | €8.00  |
| Box plastica ABS        | 2   | €3.50        | €7.00  |
| Varie (resistenze, fili)| 1   | €3.60        | €3.60  |
| **Totale**              |     |              | **€59.00** |

---

## 7. Troubleshooting

### Packet Loss Elevato (>5%)
- Verificare distanza e ostruzioni fisiche
- Controllare interferenze WiFi (cambiare canale AP se necessario)
- Ridurre il numero di dispositivi WiFi nell'area
- Verificare antenna ESP32-S3 libera da metallo

### Audio Glitch / Underrun
- Aumentare `AUDIO_RING_FRAMES` in `config.h` (default 16)
- Verificare che il task audio abbia priorità 8 in FreeRTOS
- Controllare carico CPU sul master TX

### Range Limitato
- Usare ESP32-S3 con antenna esterna (IPEX/SMA)
- Posizionare moduli RX con visibilità diretta al master
- Evitare pareti in cemento armato tra master e RX

### Modulo RX Non Appare Online
1. Verificare che `MY_CHANNEL_ID` corrisponda al canale configurato nel master
2. Rileggere il MAC address via Serial Monitor e aggiornare il master
3. Verificare che master e RX usino lo stesso canale WiFi
