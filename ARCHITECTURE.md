# Architecture – DSP Control System

## Dual-ESP32 Architecture (v2.0+)

```
┌─────────────────────────────────────────────────────────────────────┐
│                    SISTEMA AUDIO PA PROFESSIONALE                   │
│                       Architettura Dual-ESP32                       │
└─────────────────────────────────────────────────────────────────────┘

┌──────────────────────────────┐     I2S Shared     ┌──────────────────────────────┐
│   ESP32 #1 (MASTER)          │◄──────────────────►│   ESP32 #2 (SLAVE)           │
│   UEDX80480050E-WB-A         │                    │   ESP32-S3-WROOM1-N16R8      │
│   5" 800×480 RGB             │   UART IPC         │   Antenna IPEX 3dBi          │
│                              │◄──────────────────►│                              │
│  ┌────────────────────┐      │   115200 baud      │  ┌────────────────────┐      │
│  │ LVGL Display       │      │   CRC8 frames      │  │ I2S Slave RX       │      │
│  │ Touch GT911        │      │                    │  │ FFT 6 bande        │      │
│  └────────────────────┘      │                    │  └────────────────────┘      │
│  ┌────────────────────┐      │                    │  ┌────────────────────┐      │
│  │ ES8388 ADC/DAC     │      │                    │  │ DMX512 @ 40Hz      │      │
│  │ I2S Master         │──────┼──BCLK/WS/DIN──────►│  │ 512 canali         │      │
│  └────────────────────┘      │                    │  └────────────────────┘      │
│  ┌────────────────────┐      │                    │  ┌────────────────────┐      │
│  │ RS-485 → CQ260D    │      │                    │  │ ESP-NOW TX         │      │
│  │ UART2              │      │                    │  │ Antenna IPEX       │      │
│  └────────────────────┘      │                    │  └────────────────────┘      │
│  ┌────────────────────┐      │                    │  ┌────────────────────┐      │
│  │ WiFi AP            │      │                    │  │ NVS Storage        │      │
│  │ REST API           │      │                    │  │ Preset/Scene/WLED  │      │
│  └────────────────────┘      │                    │  └────────────────────┘      │
│  CPU Load: ~60%              │                    │  CPU Load: ~40%              │
└──────────────────────────────┘                    └──────────────────────────────┘
         │                                                    │
         │ RS-485                                             │ DMX512
         ▼                                                    ▼
    DSP CQ260D                                         Fixture Luci
    (48kHz fisso)                                      (512 canali)
```

## Pipeline Audio con I2S Condiviso

```
ES8388 ADC
    │ DOUT (I2S data)
    ├──────────────────────────────────────┐
    │                                      │
    ▼                                      ▼
ESP32 #1 DIN (GPIO11)              ESP32 #2 DIN (GPIO7)
    │ I2S Master RX                     │ I2S Slave RX
    │                                   │
    ▼                                   ▼
audio_fft_process()              audio_slave_process_fft()
    │ FFT 512pt, 32 bande              │ FFT 512pt, 6 bande
    │ VU meter L+R                     │ Audio-reactive DMX
    ▼                                  ▼
ES8388 DAC                       dmx_slave_task()
    │                                  │ 40 Hz, 512 canali
    ▼                                  ▼
Amplificatori                    Fixture luci
    │
    ▼
RS-485 → DSP CQ260D

ESP32 #1 genera BCLK (GPIO12) + WS (GPIO13)
  → entrambi si propagano a ES8388 + ESP32 #2
  → latenza hardware I2S: <1ms
```

## Protocollo UART IPC

```
Frame binario Master → Slave:
┌────────┬────────┬─────────┬─────────────┬────────────────┬──────┐
│ 0xAA   │ 0x55   │ command │ payload_len │ payload[0..N]  │ CRC8 │
│ sync1  │ sync2  │ 1 byte  │ 1 byte      │ 0-128 bytes    │      │
└────────┴────────┴─────────┴─────────────┴────────────────┴──────┘

Comandi Master → Slave:
  0x01 CMD_SCENE_CHANGE        : [sceneIndex]
  0x02 CMD_DMX_RAW             : [startCh][count][values...]
  0x10 CMD_STORAGE_SAVE_PRESET : [slot][data...]
  0x11 CMD_STORAGE_LOAD_PRESET : [slot]
  0x12 CMD_STORAGE_SAVE_SCENE  : [slot][data...]
  0x13 CMD_STORAGE_LOAD_SCENE  : [slot]
  0xF0 CMD_PING                : []

Risposte Slave → Master (stesso formato frame):
  0x80 RESP_ACK                : []
  0x81 RESP_NACK               : []
  0x84 RESP_STORAGE_DATA       : [data...]
  0xF0 RESP_PONG               : []

CRC8: polynomial 0x07, init 0x00, calcolato su command+payload_len+payload
Timeout ACK: 100ms | Retry: 3 tentativi | Baudrate: 115200 bps
```

## Pipeline Audio (dettaglio Master)

```
Sorgente (Denon SC LIVE 4)
  │ XLR +18dBu, 44.1kHz
  ▼
ES8388 ADC
  │ I2S 16-bit stereo
  ▼
audio_config_init()
  │ Auto-detect SR (I2S timing)
  │ Configura SRC se SR ≠ 48kHz
  ▼
[AudioSRC] → se SR input ≠ 48kHz
  │ Linear interpolation
  │ 44.1kHz → 48kHz
  ▼
[AudioProtection]
  │ DC Block (IIR ~1Hz)
  │ HPF Butterworth 30Hz
  │ Brick-wall Limiter (-1dBFS)
  │ Soft Clip (tanh)
  ▼
audio_fft_process()
  │ FFT 512pt, 32 bande
  │ VU meter L+R
  ▼
  ├──→ ES8388 DAC (output diretto)
  │       │
  │    [AudioDelayBuffer] PSRAM
  │       │ max 100ms delay
  │       ▼
  │    Amplificatori esterni
  │
  └──→ RS-485 → DSP CQ260D
          │ @ 48kHz fisso
          │ delay in campioni @ 48kHz
          ▼
       Speaker Array
```

## State Machine Audio Mode

```
        ┌──────────────────┐
        │ MixerPassThrough │◄──────────────┐
        │ Relay LOW        │               │
        │ XLR IN → XLR OUT │               │
        └────────┬─────────┘               │
                 │ setAudioMode(TestTone)   │
                 ▼                          │ setAudioMode(MixerPassThrough)
        ┌──────────────────┐               │
        │    TestTone      ├───────────────┘
        │ Relay HIGH       │
        │ DAC → XLR OUT    │
        └──────────────────┘
```

## Memory Layout (PSRAM 8MB)

### ESP32 #1 (Master)

| Componente | Dimensione | Note |
|------------|-----------|------|
| Display FB 1 | ~768KB | 800×480×2 byte (RGB565) |
| Display FB 2 | ~768KB | Double buffering |
| AudioDelayBuffer | ~19.2KB | 100ms @ 48kHz stereo |
| AudioSRC buffers | ~10KB | Ring buffer storia campioni |
| LVGL heap | ~512KB | Allocazioni UI |
| **Totale** | **~2MB** | 25% degli 8MB disponibili |

### ESP32 #2 (Slave)

| Componente | Dimensione | Note |
|------------|-----------|------|
| I2S DMA buffers | ~32KB | 8 buf × 256 campioni × 2ch × 2byte |
| FFT buffer | ~8KB | 512pt complesso float32 |
| DMX universe | ~1KB | 512 canali + start code |
| NVS storage | (flash) | 4KB per chiave NVS |
| **Totale PSRAM** | **~41KB** | <1% degli 8MB – ampio margine |

## Comunicazione RS-485 / DSP CQ260D

```
Frame TX: FF 55 [LEN] [GRP] [ID_L] [ID_H] [PAYLOAD...] [CHK]
Frame RX: FF 55 [LEN] [GRP] [ID_L] [ID_H] [CMD_RESP]   [CHK]

SYSREG upload: blocchi da 50 byte, delay 30ms tra blocchi
Baud rate: 115200
```

## REST API (AsyncWebServer)

| Method | Endpoint | Descrizione |
|--------|---------|-------------|
| GET | `/api/status` | Stato sistema |
| GET | `/api/audio/config` | Config audio (SR, SRC, gain) |
| POST | `/api/dsp/delay` | Imposta delay cassa |
| POST | `/api/dsp/preset` | Applica preset |
| GET | `/api/fft` | Dati FFT real-time |
| POST | `/api/dmx/fixture` | Controllo fixture DMX |

## Dipendenze Librerie

| Libreria | Versione | Uso |
|---------|---------|-----|
| lvgl | ^8.3 | UI display |
| LovyanGFX | ^1.1.16 | Driver display RGB |
| ESP Async WebServer | ^1.2.4 | REST API |
| ArduinoJson | ^6.21 | JSON NVS/API |
| Adafruit NeoPixel | ^1.12.0 | LED ring WS2812B |
| esp-dsp | ^1.3.2 | FFT accelerata |
| Unity | ^2.6.1 | Test unitari (native) |
