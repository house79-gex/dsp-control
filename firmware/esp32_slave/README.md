# Firmware ESP32-S3 Slave – DMX + Wireless + Storage

Firmware PlatformIO per il modulo slave del sistema audio PA dual-ESP32.

## Hardware

| Caratteristica | Valore |
|----------------|--------|
| **Board**      | ESP32-S3-WROOM1-N16R8 Dev Board |
| **Flash**      | 16 MB |
| **PSRAM**      | 8 MB |
| **Antenna**    | IPEX connector + antenna 2.4GHz 3dBi esterna |
| **USB**        | USB-CDC on boot (debug seriale) |

## Funzionalità

| Modulo | Stato | Note |
|--------|-------|------|
| I2S Slave RX | ✅ | Audio da ESP32 Master (48kHz, 16bit stereo) |
| FFT 6 bande | ✅ | Bass/LowMid/Mid/HiMid/Treble/Presence, 40Hz |
| DMX512 output | ✅ | UART2, 250kbaud, 40Hz, 512 canali |
| ESP-NOW TX | ✅ | Broadcast audio-reactive frames, antenna IPEX |
| NVS Storage | ✅ | Preset DSP, scene DMX, config WLED |
| UART IPC | ✅ | Comandi da Master (115200 baud, CRC8) |
| Relay / Strobo | ✅ | GPIO diretti, strobo audio-reactive con isteresi |

## Compilazione e Flash

```bash
cd firmware/esp32_slave

# Compilazione
pio run -e esp32s3

# Flash (con ESP32-S3 collegato via USB)
pio run -e esp32s3 -t upload

# Monitor seriale
pio device monitor --baud 115200
```

## Pinout GPIO

| GPIO | Funzione | Direzione | Collegato a |
|------|----------|-----------|-------------|
| GPIO1  | IPC UART TX   | OUT | Master GPIO18 (RX) |
| GPIO2  | IPC UART RX   | IN  | Master GPIO17 (TX) |
| GPIO5  | I2S BCLK IN   | IN  | Master GPIO12 (BCLK out) |
| GPIO6  | I2S WS IN     | IN  | Master GPIO13 (WS out) |
| GPIO7  | I2S DIN       | IN  | ES8388 DOUT (parallelo con Master) |
| GPIO8  | DMX TX        | OUT | Linea DMX512 |
| GPIO9  | DMX DE        | OUT | Driver Enable (sempre HIGH) |
| GPIO10 | Relay GPIO    | OUT | Relay generale |
| GPIO11 | Strobe GPIO   | OUT | Uscita strobo |

## Struttura Sorgenti

```
src/
├── main.cpp               – Setup + loop IPC + task FreeRTOS
├── config.h               – Pin GPIO e costanti
├── ipc_slave.h/cpp        – Ricevitore UART IPC (comandi da Master)
├── audio_slave.h/cpp      – I2S Slave RX + FFT 6 bande
├── dmx512_slave.h/cpp     – DMX512 output 40Hz task FreeRTOS
├── wireless_slave.h/cpp   – ESP-NOW TX broadcast
├── storage_slave.h/cpp    – NVS storage completo
└── relay_control.h/cpp    – Relay + strobo audio-reactive
```

## Protocollo IPC

Comunicazione binaria UART con sync bytes + CRC8. Vedere
[`docs/DUAL_ESP32_INTEGRATION.md`](../../docs/DUAL_ESP32_INTEGRATION.md) per il
protocollo completo.

### Comandi ricevuti dal Master

| Comando | Codice | Payload | Risposta |
|---------|--------|---------|----------|
| `CMD_PING` | 0xF0 | — | `RESP_PONG` |
| `CMD_SCENE_CHANGE` | 0x01 | sceneIndex (1 byte) | `RESP_ACK` |
| `CMD_DMX_RAW` | 0x02 | startCh + count + values | `RESP_ACK` |
| `CMD_STORAGE_SAVE_PRESET` | 0x10 | slot + data | `RESP_ACK/NACK` |
| `CMD_STORAGE_LOAD_PRESET` | 0x11 | slot | `RESP_STORAGE_DATA/NACK` |
| `CMD_STORAGE_SAVE_SCENE` | 0x12 | slot + data | `RESP_ACK/NACK` |
| `CMD_STORAGE_LOAD_SCENE` | 0x13 | slot | `RESP_STORAGE_DATA/NACK` |

## Cablaggio verso Master

Vedere [`docs/WIRING_GUIDE.md`](../../docs/WIRING_GUIDE.md) per lo schema completo.

```
ESP32 #1 (Master)          ESP32 #2 (Slave)
GPIO12 (BCLK) ────────────→ GPIO5  (I2S_BCLK_IN)
GPIO13 (WS)   ────────────→ GPIO6  (I2S_WS_IN)
GPIO11 (DIN)  ←────────────  (condiviso ES8388)
GPIO17 (TX)   ────────────→ GPIO2  (IPC_UART_RX)
GPIO18 (RX)   ←──────────── GPIO1  (IPC_UART_TX)
GND           ─────────────  GND (comune obbligatorio)
```

## Note Tecniche

- **I2S Slave**: non genera clock (usa BCLK/WS dal Master). Se il Master non è acceso,
  `i2s_read()` bloccherà sul timeout → il task audio gestisce con `pdMS_TO_TICKS(10)`.
- **DMX timing**: generazione Break (88µs) via `delayMicroseconds()` nel task FreeRTOS
  su Core 1 (nessuna interferenza con altri task).
- **ESP-NOW range**: con antenna 3dBi IPEX in linea di vista → 150-300m.
  Ostacoli (muri) riducono a 30-80m.
- **Storage**: NVS namespace `slave_store`. Chiavi: `preset_0..15`, `scene_0..15`.
