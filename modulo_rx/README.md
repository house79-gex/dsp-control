# Modulo RX – Ricevitore Wireless 2CH

Firmware PlatformIO per il modulo ricevitore wireless del sistema Audio PA.  
Riceve dati ESP-NOW dal Master TX e gestisce:
- **Playback audio** via ES8388 DAC I2S
- **Forward RS-485** al DSP CQ260D
- **Output DMX512** verso controller luci

---

## Requisiti

- ESP32-S3 DevKit (es. ESP32-S3-DevKitC-1)
- Codec audio ES8388 (connessione I2C + I2S)
- Driver RS-485 MAX485 (UART1)
- Driver DMX485 (UART2)
- PlatformIO IDE o CLI

---

## Struttura

```
modulo_rx/
├── platformio.ini     → Configurazione PlatformIO
└── src/
    ├── config.h       → Pin GPIO + parametri
    ├── main.cpp       → Setup ESP-NOW + loop
    ├── audio_rx.h/cpp → Playback ES8388 via I2S
    ├── rs485_rx.h/cpp → Forward RS-485 al CQ260D
    └── dmx_rx.h/cpp   → Generazione DMX512 output
```

---

## Configurazione Canale

Ogni modulo RX riceve su un canale specifico (1 o 2).  
Modificare in `platformio.ini`:

```ini
build_flags =
    ...
    -DMY_CHANNEL_ID=1   ; ← CH1
    ; oppure
    -DMY_CHANNEL_ID=2   ; ← CH2
```

---

## Compilazione e Flash

```bash
# Compilare per CH1
cd modulo_rx
pio run -e esp32s3

# Flash su dispositivo
pio run -e esp32s3 -t upload

# Monitor seriale per leggere MAC address
pio device monitor
```

---

## Lettura MAC Address

Dopo il flash, aprire il monitor seriale. Il modulo stampa:
```
[RX] MAC: 24:0A:C4:12:34:56
```

Annotare questo indirizzo e configurarlo nel master TX tramite:
```
POST /api/wireless/peer1
{ "mac": [0x24, 0x0A, 0xC4, 0x12, 0x34, 0x56] }
```

---

## Pin Connections

| Funzione      | GPIO | Componente              |
|---------------|------|-------------------------|
| I2S SCK       | 15   | ES8388 BCLK             |
| I2S WS        | 16   | ES8388 LRCK             |
| I2S SD OUT    | 17   | ES8388 DIN              |
| I2C SDA       | 5    | ES8388 SDA              |
| I2C SCL       | 6    | ES8388 SCL              |
| RS485 TX      | 43   | MAX485 DI               |
| RS485 RX      | 44   | MAX485 RO               |
| RS485 DE      | 21   | MAX485 DE+RE            |
| DMX TX        | 1    | DMX485 DI               |
| DMX DE        | 2    | DMX485 DE               |
| LED Status    | 48   | LED integrato           |

---

## Statistiche

Ogni 5 secondi il modulo stampa via Serial:
```
[STATS CH1] Ricevuti=1000 Persi=2 Ignorati=0 CRC_ERR=0 Loss=0.2% AudioUnderrun=0 AudioFrames=998
```

---

## LED Indicatore Stato

| Colore          | Significato              |
|-----------------|--------------------------|
| Verde fisso     | Ricezione OK             |
| Giallo lampeggiante | Packet loss > 5%    |
| Rosso lampeggiante  | Offline (>3 secondi) |
