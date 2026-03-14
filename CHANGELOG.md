# Changelog

Tutte le modifiche significative a questo progetto sono documentate in questo file.
Formato: [Semantic Versioning](https://semver.org/lang/it/)

---

## [2.1.0] – 2026-03-13

### Added
- **Slave**: encoder (15–17, 18/21/22), LED ring (23–24), IPC **CMD_POLL_INPUTS / SET_LED_RING / RELAY_SET**
- **Master**: `USE_SLAVE_PERIPHERALS` – encoder/LED/relay via IPC; `storage_init` prima di `audio_init`
- **Docs**: `MASTER_SLAVE_ARCHITECTURE.md`, `schematics_print_system.html`

---

## [2.0.5] – 2026-03-13

### Changed (UEDX V3.3 – datasheet)
- **`docs/UEDX_V33_FREE_PINS.md`**: tabella GPIO occupati LCD/touch; **liberi reali: 10, 17, 18** (no SD); **GPIO38 solo RST CTP**
- **Master `config.h`**: **I2S_DOUT = GPIO10** (ex SD-CS); **RS485_DE_RE = -1** (modulo auto-RS485) se no pin DE
- **`rs485.cpp`**: DE/RE opzionali se `RS485_DE_RE < 0`

---

## [2.0.4] – 2026-03-13

### Added
- **`docs/UEDX_V33_PINOUT_PLAN.md`**: Strategia A (RST touch TCA9535 + GPIO38 DOUT) vs B (Slave **GPIO4** → ES8388 DSDIN)
- **`docs/schematics_print_full.html`**: diagrammi stampabili (dual-ESP32, I2S, RS-485, DMX, Pololu)
### Changed
- **Master `config.h`**: `USE_SLAVE_ES8388_DOUT` · I2S DOUT opzionale da Slave
- **Slave `config.h` + `audio_slave.cpp`**: `SLAVE_DRIVES_ES8388_DOUT`, I2S TX su GPIO4, silenzio su bus DAC se attivo
- **`audio_mode.cpp`**: `I2S_PIN_NO_CHANGE` su DOUT quando Strategia B

---

## [2.0.3] – 2026-03-13

### Added
- **`docs/DATASHEETS_REFERENCE.md`**: sintesi da PDF in repo (UEDX V3.3 J2/TP/alim, M144 ES8388+BUS, Pololu 5398 VIN1/2/VOUT, relè, EC11, MAX485, LED ring)
### Changed
- **`docs/CABLING_COMPLETE.md`**, **`DISPLAY_SETUP.md`**, **`hardware/README`**, **`schematics_print.html`**: allineati ai contenuti dei PDF; nota **GPIO38** (touch RST vs I2S_DOUT)

---

## [2.0.2] – 2026-03-13

### Added
- **`docs/CABLING_COMPLETE.md`**: encoder + LED ring + relay TCA9535 + ES8388 I2S + RCA + XLR bilanciato (THAT1206) + jack + Pololu ORing + RJ45 DSP + secondo MAX485 DMX + box Slave esteso
- **`docs/hardware/README.md`**: istruzioni per PDF display UEDX e schematico Atomstack

---

## [2.0.1] – 2026-03-13

### Added
- **Schemi**: `docs/SCHEMATICS.md` (Mermaid + tabelle), `docs/schematics_print.html` (stampa/PDF), `docs/schematica_architettura_dsp_control.png`
- **CI**: job build `firmware/esp32_slave` su GitHub Actions

### Changed
- **DSP**: `send_param` / ID dispositivo da ID compatto discovery `(grp<<4)|id`; preset broadcast `0xFF`
- **RS-485 beep**: identificazione tramite impulsi **SET_PARAM** gain (documentato in `PROTOCOL_RS485_CQ260D.md`)
- **UI LVGL** `ui_dsp_advanced`: gain, delay e preset applicati alla selezione (Tutti/SX/DX/SUB/Cassa)
- README / firmware README: stato moduli allineato al codice

---

## [2.0.0] – 2026-03-08

### 🎯 Architettura Dual-ESP32 (Breaking Change)

#### Added
- **Dual-MCU Architecture**: split Master (Audio/Display/RS-485/WiFi) + Slave (DMX/Wireless/Storage)
- **I2S Bus Sharing**: audio ES8388 condiviso tra ESP32 #1 e #2 con latenza < 1ms (hardware DMA)
- **UART IPC Protocol**: comunicazione inter-processor binaria con sync bytes + CRC8 (polynomial 0x07)
  - `firmware/esp32/src/ipc_master.h/cpp`: lato Master con retry logic e timeout
  - `firmware/esp32_slave/src/ipc_slave.h/cpp`: lato Slave con state machine parser
- **Storage Delegation**: NVS preset/scene delegato a ESP32 #2 via IPC con fallback locale
- **ESP32 #2 Codebase**: nuovo firmware completo `firmware/esp32_slave/`
  - I2S Slave RX + FFT 6 bande (Bass/LowMid/Mid/HiMid/Treble/Presence)
  - DMX512 output @ 40Hz (UART2, 250kbaud, task FreeRTOS Core 1)
  - ESP-NOW wireless TX broadcast con antenna IPEX 3dBi
  - NVS storage completo (preset DSP, scene DMX, config WLED)
  - Relay + strobo audio-reactive con isteresi
- **GPIO I/O Expander TCA9535**: relay/LED/TOUCH_RST su I2C (addr 0x20), libera GPIO45/48
- **Documentation**:
  - `docs/DUAL_ESP32_INTEGRATION.md`: guida completa integrazione (hardware + software)
  - `docs/WIRING_GUIDE.md`: cablaggio fisico dettagliato con BOM e checklist
  - `docs/PINOUT_REFERENCE.md`: tabelle pinout complete entrambi gli ESP32
  - `docs/TESTING.md`: procedure test IPC/I2S/DMX/wireless
  - Aggiornato `ARCHITECTURE.md` con diagrammi dual-MCU e protocollo IPC

#### Changed
- **ESP32 #1 (Master) – GPIO refactoring**:
  - I2S Master: BCLK=GPIO12, WS=GPIO13, DIN=GPIO11, DOUT=GPIO38
    (v1: SCK=GPIO15 conflitto LCD_G3, WS=GPIO16 conflitto LCD_G4)
  - UART IPC: TX=GPIO17, RX=GPIO18 (liberati da I2S ridisegnato)
  - RS-485: DE+RE uniti su GPIO10 (v1: DE=GPIO21 conflitto LCD_R3, RE=GPIO20 conflitto TOUCH_SCL)
  - TOUCH_RST (GPIO38) → TCA9535 P1_0 (GPIO38 ora I2S_DOUT)
  - TOUCH_INT (GPIO18) → polling GT911 (GPIO18 ora IPC_UART_RX)
  - DMX512 rimosso dal Master (GPIO1/GPIO2 rimangono a LCD_B4/LCD_BL_EN)
  - Storage: NVS locale come fallback, primario su ESP32 #2 via IPC
- **platformio.ini Master**: aggiunto `-DFIRMWARE_VERSION="2.0.0"`, `-DFIRMWARE_ROLE="master"`

#### Removed
- `RS485_DE`, `RS485_RE` (separati) → sostituiti con `RS485_DE_RE` (GPIO10)
- `DMX_TX`, `DMX_DE` su Master → spostati su ESP32 #2
- `RELAY_PIN=45`, `LED_STATUS=48` come GPIO diretti → sostituiti con TCA9535 expander
- `I2S_SCK`, `I2S_WS` (nomi legacy) → rinominati `I2S_BCLK`, nuovi pin
- `I2S_SD_OUT`, `I2S_SD_IN` → rinominati `I2S_DOUT`, `I2S_DIN`, nuovi pin
- `TOUCH_RST`, `TOUCH_INT` come define dirette → gestiti via TCA9535/polling

#### Fixed
- **GPIO Conflicts risolti**:
  - I2S_SCK=GPIO15 conflitto con LCD_G3 → I2S_BCLK=GPIO12 (libero)
  - I2S_WS=GPIO16 conflitto con LCD_G4 → I2S_WS=GPIO13 (libero)
  - RS485_DE=GPIO21 conflitto con LCD_R3 → RS485_DE_RE=GPIO10 (libero)
  - RS485_RE=GPIO20 conflitto con TOUCH_SCL → rimosso (DE+RE uniti)
  - RELAY_PIN=GPIO45 conflitto con LCD_R0 → TCA9535 P0_0
  - LED_STATUS=GPIO48 conflitto con LCD_R1 → TCA9535 P0_1
- **CPU Overload**: Master ridotto a ~60% (da ~85% single-MCU)
  - FFT 6 bande delegato a ESP32 #2 (Slave)
  - DMX512 @ 40Hz delegato a ESP32 #2
  - ESP-NOW wireless delegato a ESP32 #2

#### Performance
- **Latenza I2S**: < 1ms (hardware DMA vs 5-10ms UART audio streaming)
- **CPU Load Master**: ~60% (vs ~85% single-MCU)
- **CPU Load Slave**: ~40% (margine 60% per espansioni future)
- **Wireless Range**: 150-300m con antenna 3dBi IPEX (vs 50m antenna PCB)
- **DMX FPS**: 40Hz stabile su Core 1 FreeRTOS (nessuna interferenza display)

#### Breaking Changes
- **Hardware richiesto**: necessario secondo ESP32-S3-WROOM1-N16R8 + antenna IPEX + cavi
- **Pin Mapping Changed**: GPIO17/18/10/38/11/12/13 riassegnati (I2S + IPC + RS485)
- **Firmware Split**: due firmware separati (`firmware/esp32/` e `firmware/esp32_slave/`)
- **API Storage**: `storage_*` ora primariamente su IPC (trasparente, con fallback NVS)

#### Migration Guide
1. Acquistare ESP32-S3-WROOM1-N16R8 Dev Board + antenna IPEX 3dBi + cavi ribbon 7-pin
2. Aggiornare hardware Master: TCA9535 expander per relay/LED/TOUCH_RST
3. Cablare I2S/UART/GND secondo `docs/WIRING_GUIDE.md`
4. Flash ESP32 #1 con `firmware/esp32/` (aggiornato)
5. Flash ESP32 #2 con `firmware/esp32_slave/` (nuovo)
6. Avviare ESP32 #2 prima di ESP32 #1 (IPC ping al boot)
7. Verificare log: `[STORAGE] ESP32 #2 disponibile via IPC`
8. I preset NVS locali esistenti vengono usati come fallback e poi migrati

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
