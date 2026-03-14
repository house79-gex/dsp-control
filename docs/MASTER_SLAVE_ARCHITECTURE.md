# Architettura Master (UEDX) + Slave – un solo sistema

Il **Master** usa solo i pin obbligati (LCD, touch, I2S, IPC, RS-485).  
Tutto il resto è sullo **Slave**: encoder, LED ring, relay audio, DMX, FFT, wireless, storage NVS.

## Master – ruolo e pin

| GPIO | Funzione |
|------|----------|
| 11,12,13,10 | I2S ↔ ES8388 (no SD → DOUT su 10) |
| 17, 18 | IPC ↔ Slave |
| 19, 20 | Touch I2C |
| 38 | Reset GT911 (solo touch) |
| 43, 44 | RS-485 DSP (modulo auto o DE esterno) |
| Altri | LCD RGB (fissi) |

## Slave – periferiche (implementato in firmware)

| GPIO | Funzione |
|------|----------|
| 1 | IPC TX → Master |
| 2 | IPC RX ← Master |
| 4 | I2S DOUT opz. → ES8388 |
| 5, 6, 7 | I2S BCLK, WS, DIN |
| 8, 9 | DMX TX, DE |
| 10 | **Relay** audio (Mixer / TestTone) |
| 11 | Strobo |
| **15, 16, 17** | Encoder **Volume** A, B, BTN |
| **18, 21, 22** | Encoder **Balance** A, B, BTN |
| **23, 24** | **WS2812** ring Vol, Bal |
| 3, 12–48 | Liberi |

## IPC (comandi nuovi)

| Codice | Direzione | Uso |
|--------|-----------|-----|
| 0x20 | M→S | Poll encoder → risposta 0x85 (volΔ, balΔ, btn) |
| 0x21 | M→S | LED ring (vol %, bal) |
| 0x22 | M→S | Relay ON/OFF |

## Build

- Master: `USE_SLAVE_PERIPHERALS=1` (default in `config.h`)
- Slave: `SLAVE_HAS_ENCODERS_LED=1`, NeoPixel in `platformio.ini`

Stampa: **`docs/schematics_print_system.html`**
