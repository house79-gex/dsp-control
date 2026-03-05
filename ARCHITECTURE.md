# Architecture – DSP Control System

## Pipeline Audio

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

| Componente | Dimensione | Note |
|------------|-----------|------|
| Display FB 1 | ~768KB | 800×480×2 byte (RGB565) |
| Display FB 2 | ~768KB | Double buffering |
| AudioDelayBuffer | ~19.2KB | 100ms @ 48kHz stereo |
| AudioSRC buffers | ~10KB | Ring buffer storia campioni |
| LVGL heap | ~512KB | Allocazioni UI |
| **Totale** | **~2MB** | 25% degli 8MB disponibili |

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
