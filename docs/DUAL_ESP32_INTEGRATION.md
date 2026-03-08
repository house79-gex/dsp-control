# Integrazione Dual-ESP32: Master + Slave

Guida completa per l'integrazione del sistema audio PA con architettura dual-ESP32.

---

## 1. Panoramica Hardware

Il sistema utilizza **due ESP32-S3** con ruoli distinti:

| Caratteristica | ESP32 #1 (Master) | ESP32 #2 (Slave) |
|----------------|-------------------|------------------|
| **Board** | UEDX80480050E-WB-A | ESP32-S3-WROOM1-N16R8 |
| **Display** | 5" 800×480 RGB touch | — |
| **Audio** | I2S Master (ES8388 ADC/DAC) | I2S Slave RX + FFT |
| **DSP** | RS-485 → CQ260D | — |
| **Luci** | — | DMX512 @ 40Hz |
| **Wireless** | WiFi AP + REST API | ESP-NOW TX (antenna IPEX) |
| **Storage** | Delega a Slave via IPC | NVS completo |
| **CPU Load** | ~60% | ~40% |

---

## 2. Pinout Completo

### ESP32 #1 (Master – UEDX80480050E-WB-A)

#### Display RGB (16 pin dati + 4 controllo)

| GPIO | Segnale | Direzione |
|------|---------|-----------|
| GPIO45 | LCD_R0 | OUT |
| GPIO48 | LCD_R1 | OUT |
| GPIO47 | LCD_R2 | OUT |
| GPIO21 | LCD_R3 | OUT |
| GPIO14 | LCD_R4 | OUT |
| GPIO5  | LCD_G0 | OUT |
| GPIO6  | LCD_G1 | OUT |
| GPIO7  | LCD_G2 | OUT |
| GPIO15 | LCD_G3 | OUT |
| GPIO16 | LCD_G4 | OUT |
| GPIO4  | LCD_G5 | OUT |
| GPIO8  | LCD_B0 | OUT |
| GPIO3  | LCD_B1 | OUT |
| GPIO46 | LCD_B2 | OUT |
| GPIO9  | LCD_B3 | OUT |
| GPIO1  | LCD_B4 | OUT |
| GPIO42 | LCD_PCLK | OUT |
| GPIO39 | LCD_HSYNC | OUT |
| GPIO41 | LCD_VSYNC | OUT |
| GPIO40 | LCD_DE | OUT |
| GPIO2  | LCD_BL_EN | OUT |

#### Touch GT911 (I2C)

| GPIO | Segnale | Direzione |
|------|---------|-----------|
| GPIO20 | TOUCH_SCL | OUT |
| GPIO19 | TOUCH_SDA | IN/OUT |

> **Nota**: GPIO38 (TOUCH_RST) e GPIO18 (TOUCH_INT) riassegnati in v2.0.
> TOUCH_RST → TCA9535 P1_0. Touch interrupt gestito in polling su GT911.

#### I2S ES8388 (Master Mode)

| GPIO | Segnale | Direzione | Connesso a |
|------|---------|-----------|------------|
| GPIO12 | I2S_BCLK | OUT | ES8388 + ESP32 #2 GPIO5 |
| GPIO13 | I2S_WS   | OUT | ES8388 + ESP32 #2 GPIO6 |
| GPIO11 | I2S_DIN  | IN  | ES8388 DOUT (condiviso con ESP32 #2) |
| GPIO38 | I2S_DOUT | OUT | ES8388 DIN (DAC playback) |

#### UART IPC verso ESP32 #2

| GPIO | Segnale | Direzione | Connesso a |
|------|---------|-----------|------------|
| GPIO17 | IPC_TX | OUT | ESP32 #2 GPIO2 (RX) |
| GPIO18 | IPC_RX | IN  | ESP32 #2 GPIO1 (TX) |

#### RS-485 DSP CQ260D

| GPIO | Segnale | Direzione |
|------|---------|-----------|
| GPIO43 | RS485_TX | OUT |
| GPIO44 | RS485_RX | IN  |
| GPIO10 | RS485_DE_RE | OUT (HIGH=TX, LOW=RX) |

#### TCA9535 I2C Expander (addr 0x20)

| Pin Expander | Segnale | Descrizione |
|--------------|---------|-------------|
| P0_0 | RELAY_DPDT | HIGH=TestTone, LOW=Passthrough |
| P0_1 | LED_STATUS | LED di stato |
| P1_0 | TOUCH_RST  | Touch reset (active LOW) |

**GPIO liberi Master**: GPIO12, GPIO13, GPIO11, GPIO17, GPIO18, GPIO10, GPIO38
(prima tutti occupati/conflitto → ora assegnati a I2S/IPC/RS485)

---

### ESP32 #2 (Slave – WROOM1-N16R8 Dev Board)

| GPIO | Segnale | Direzione | Connesso a |
|------|---------|-----------|------------|
| GPIO1  | IPC_UART_TX | OUT | Master GPIO18 (RX) |
| GPIO2  | IPC_UART_RX | IN  | Master GPIO17 (TX) |
| GPIO5  | I2S_BCLK_IN | IN  | Master GPIO12 (BCLK) |
| GPIO6  | I2S_WS_IN   | IN  | Master GPIO13 (WS) |
| GPIO7  | I2S_DIN     | IN  | ES8388 DOUT (fan-out) |
| GPIO8  | DMX_TX      | OUT | Linea DMX512 |
| GPIO9  | DMX_DE      | OUT | Driver Enable (sempre HIGH) |
| GPIO10 | RELAY_GPIO  | OUT | Relay generale |
| GPIO11 | STROBE_GPIO | OUT | Strobo |
| IPEX   | Antenna WiFi | —  | Antenna 2.4GHz 3dBi esterna |

**GPIO liberi Slave**: GPIO12-GPIO48 (eccetto quelli sopra) – >10 pin liberi

---

## 3. Schema Collegamento Fisico

```
┌─────────────────────────────────────────────────────────────────────┐
│                    CABLAGGIO INTER-ESP32                            │
└─────────────────────────────────────────────────────────────────────┘

 ES8388 BCLK ←──┬── ESP32 #1 GPIO12 (I2S_BCLK, Master OUT)
                 └── ESP32 #2 GPIO5  (I2S_BCLK_IN, Slave IN)

 ES8388 WS   ←──┬── ESP32 #1 GPIO13 (I2S_WS, Master OUT)
                 └── ESP32 #2 GPIO6  (I2S_WS_IN, Slave IN)

 ES8388 DOUT ────┬── ESP32 #1 GPIO11 (I2S_DIN, Master IN)
                 └── ESP32 #2 GPIO7  (I2S_DIN, Slave IN)

 ESP32 #1 GPIO17 (TX) ──────────→ ESP32 #2 GPIO2 (RX)  [IPC UART]
 ESP32 #1 GPIO18 (RX) ←────────── ESP32 #2 GPIO1 (TX)  [IPC UART]

 Alimentatore 5V 2A
    ├─ +5V ──┬─→ ESP32 #1 VIN
    │        └─→ ESP32 #2 5V
    └─ GND ──┴─→ GND comune (OBBLIGATORIO!)
```

**Fili totali inter-ESP32**: 7 (3×I2S + 2×UART + GND + opz. +5V)

---

## 4. Configurazione Software

### 4.1 Setup I2S Master (ESP32 #1)

Il Master genera BCLK e WS che si propagano sia a ES8388 che a ESP32 #2:

```cpp
// In audio_mode.cpp – già configurato in v2.0
i2s_config_t i2s_cfg = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
    .sample_rate = 48000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    // ...
};
i2s_pin_config_t pin_cfg = {
    .bck_io_num   = I2S_BCLK,   // GPIO12
    .ws_io_num    = I2S_WS,     // GPIO13
    .data_out_num = I2S_DOUT,   // GPIO38
    .data_in_num  = I2S_DIN     // GPIO11
};
```

### 4.2 Setup I2S Slave (ESP32 #2)

Lo Slave riceve il clock dal Master e acquisisce il dato audio:

```cpp
// In audio_slave.cpp
i2s_config_t cfg = {
    .mode = (i2s_mode_t)(I2S_MODE_SLAVE | I2S_MODE_RX),
    .sample_rate = 48000,
    // ...
};
i2s_pin_config_t pin_cfg = {
    .bck_io_num       = I2S_BCLK_IN,       // GPIO5
    .ws_io_num        = I2S_WS_IN,         // GPIO6
    .data_out_num     = I2S_PIN_NO_CHANGE, // Non usato
    .data_in_num      = I2S_DIN            // GPIO7
};
```

### 4.3 Setup UART IPC

**Master** (ESP32 #1):
```cpp
// In ipc_master.cpp – inizializzato automaticamente da storage_init()
ipc_master_init();  // UART1, 115200 baud, TX=GPIO17, RX=GPIO18
```

**Slave** (ESP32 #2):
```cpp
// In ipc_slave.cpp – chiamato da setup()
ipc_slave_init();   // UART1, 115200 baud, RX=GPIO2, TX=GPIO1
```

### 4.4 Test Comunicazione

```cpp
// Sul Master: invia ping e attende pong
bool ok = ipc_master_ping(200);  // timeout 200ms
if (ok) Serial.println("IPC OK");
```

---

## 5. Alimentazione

### Opzione A: Condivisa (consigliata)
```
Alimentatore 5V 2A
    ├─ +5V → ESP32 #1 (VIN) + ESP32 #2 (5V)
    └─ GND → massa comune
```
**Pro**: GND comune automatico, un solo cavo  
**Contro**: se un ESP32 si riavvia, l'altro potrebbe vedere un picco

### Opzione B: Separata con GND comune
```
Alimentatore 5V 1A → ESP32 #1 (VIN)
Alimentatore 5V 1A → ESP32 #2 (5V)
GND ────────────── GND comune (filo 20AWG)
```
**Pro**: isolamento migliore  
**Attenzione**: il GND comune è **obbligatorio** per I2S e UART

---

## 6. Troubleshooting

### I2S: nessuna sincronizzazione (Slave non riceve audio)
- Verificare che Master sia avviato e I2S inizializzato prima dello Slave
- Controllare cablaggio BCLK/WS (usare multimetro o oscilloscopio)
- Verificare GND comune tra i due ESP32
- Log atteso su Slave: `[AUDIO_SLAVE] I2S Slave RX configurato`

### UART IPC: timeout / NACK
- Verificare TX/RX incrociati (TX Master → RX Slave, TX Slave → RX Master)
- Verificare stesso baudrate (115200) su entrambi i lati
- Testare con `ipc_master_ping()` → atteso `RESP_PONG`
- Log atteso su Master: `[IPC_MASTER] UART1 inizializzato @ 115200 baud`

### Storage: errore caricamento
- Il Master ha fallback automatico su NVS locale se IPC non risponde
- Log su Master: `[STORAGE] ESP32 #2 non risponde – fallback su NVS locale`
- Verificare che ESP32 #2 sia avviato prima del Master
- NVS slave: namespace `slave_store`, chiavi `preset_0..15`, `scene_0..15`

### ESP-NOW: raggio limitato
- Verificare che l'antenna IPEX sia correttamente avvitata
- Controllare potenza TX: `WIFI_TX_POWER_DBM = 20` (max per ESP32-S3)
- Evitare ostacoli metallici vicino all'antenna
- Log su Slave: `[WIRELESS] ESP-NOW inizializzato, TX power 20 dBm`

### DMX: nessun output
- Verificare `DMX_DE = GPIO9` sempre HIGH
- Controllare cablaggio TX (GPIO8) verso driver DMX (MAX485 o SN65HVD)
- Log atteso: `[DMX_SLAVE] Inizializzato (TX=GPIO8, DE=GPIO9, 512 canali @ 40 Hz)`
