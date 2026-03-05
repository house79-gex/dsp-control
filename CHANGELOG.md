# Changelog

Tutte le modifiche significative a questo progetto sono documentate in questo file.
Formato: [Semantic Versioning](https://semver.org/lang/it/)

---

## [2.1.0] – 2025-03-05

### Bug Fix Critici

- **CRITICAL** `dsp_control.cpp`: Corretto calcolo delay per DSP CQ260D.
  La funzione `ms_to_samples()` usava erroneamente 48 campioni/ms come costante
  non documentata. Ora delega a `audio_ms_to_samples_dsp()` che usa esplicitamente
  `AUDIO_PROCESSING_SAMPLE_RATE` (48000 Hz fisso da datasheet). Questo corregge
  desincronizzazione del delay quando l'input era a 44.1kHz.

- **CRITICAL** `config.h`: Rimossi tutti i pin display con `TODO: verificare`.
  Sostituiti con i valori corretti dal datasheet UEDX80480050E-WB-A-V3.3-SPEC.pdf
  (interfaccia RGB 40-pin + touch GT911 I2C).

### Nuove Funzionalità

#### Sistema Audio Adattivo Multi-Sample-Rate
- `audio_config.h/.cpp`: Struttura `AudioConfig` con rilevamento automatico SR
  (32k/44.1k/48k/96k Hz). Nuova costante `AUDIO_PROCESSING_SAMPLE_RATE = 48000`
  per separare chiaramente SR input (variabile) da SR processing DSP (fisso 48kHz).
- `audio_ms_to_samples_dsp()`: Conversione delay sempre con 48kHz DSP.
- `audio_ms_to_samples_local()`: Conversione delay con SR input corrente.

#### Sample Rate Converter
- `audio_src.h/.cpp`: Classe `AudioSRC` con interpolazione lineare stereo.
  Latenza < 0.1ms, ratio dinamico, supporto 32k/44.1k/48k/96k → 48kHz.
  Istanza globale `g_audioSrc`.

#### Audio Protection Pipeline
- `audio_protection.h/.cpp`: Classe `AudioProtection` con pipeline completa:
  DC Blocking (IIR ~1Hz) → HPF Butterworth 30Hz → Brick-wall Limiter
  (-1dBFS, attack 0.5ms, release 50ms) → Soft Clip (tanh).
  Istanza globale `g_audioProtection`.

#### Delay Buffer Software
- `audio_delay_buffer.h/.cpp`: Classe `AudioDelayBuffer` con buffer circolare
  stereo in PSRAM (ps_malloc). Max 100ms @ 48kHz (~19.2KB).
  Per sincronizzare output DAC ES8388 con DSP CQ260D.
  Istanza globale `g_audioDelay`.

#### Display UEDX80480050E-WB-A
- `display/lvgl_display.h/.cpp`: Configurazione LovyanGFX completa per
  pannello 800×480 RGB con touch GT911. Timing RGB 20MHz, I2C touch 400kHz.

### Modifiche a File Esistenti

- `config.h`: Aggiunte `AUDIO_SAMPLE_RATE_DEFAULT` e `AUDIO_PROCESSING_SAMPLE_RATE`.
  `AUDIO_SAMPLE_RATE` mantenuto come alias per retrocompatibilità.
  Pin display aggiornati al datasheet.
- `audio_mode.cpp`: Integra `audio_config_init()` e `g_audioSrc`. I2S configurato
  con SR rilevato dinamicamente.
- `dsp_control.cpp`: Include `audio_config.h`. `ms_to_samples()` → `audio_ms_to_samples_dsp()`.

### Test

- `tests/firmware/test_sample_rate/test_main.cpp`: 12 test unitari per:
  - Conversione delay DSP (10ms→480s, 20ms→960s, saturazione, negativi)
  - Verifica che il DSP usi sempre 48kHz (non 44.1kHz)
  - SRC init, output frame count, preservazione frequenza, passthrough 48k→48k

---

## [2.0.0] – 2024-12-01

### Nuove Funzionalità
- Streaming audio wireless via ESP-NOW (TX→RX)
- Controllo luci DMX512-A (512 canali, libreria custom)
- REST API completa (AsyncWebServer)
- Gestione preset DSP con NVS (Preferences)
- Autotune delay automatico via risposta in frequenza
- LED ring encoder WS2812B

### Architettura
- Separazione firmware ESP32 (TX) e modulo_rx (RX)
- Driver ES8388 custom (I2C control + I2S audio)
- Protocollo RS-485 CQ260D reverse-engineered

---

## [1.x] – 2024

### Storia Iniziale
- v1.0.0: Controllo base DSP via RS-485
- v1.1.0: Aggiunta FFT real-time e VU meter
- v1.2.0: WiFi AP + WebUI base
- v1.3.0: Relay DPDT per routing audio
