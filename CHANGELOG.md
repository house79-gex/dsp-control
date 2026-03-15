# Changelog

Tutte le modifiche significative a questo progetto sono documentate in questo file.
Formato: [Semantic Versioning](https://semver.org/lang/it/)

---

## [2.1.0] – 2026-03-13

### Added
- **Slave**: encoder (15–17, 18/21/22), LED ring (23–24), IPC **CMD_POLL_INPUTS / SET_LED_RING / RELAY_SET**
- **Master**: `USE_SLAVE_PERIPHERALS` – encoder/LED/relay via IPC; `storage_init` prima di `audio_init`
- **Docs**: `MASTER_SLAVE_ARCHITECTURE.md`, `SCHEMATIC_DSP_CONNECTIONS.html`

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
- **`docs/SCHEMATIC_DSP_CONNECTIONS.html`**: schema unico stampabile (sostituisce i vecchi print_*)
### Changed
- **Master `config.h`**: `USE_SLAVE_ES8388_DOUT` · I2S DOUT opzionale da Slave
- **Slave `config.h` + `audio_slave.cpp`**: `SLAVE_DRIVES_ES8388_DOUT`, I2S TX su GPIO4, silenzio su bus DAC se attivo
- **`audio_mode.cpp`**: `I2S_PIN_NO_CHANGE` su DOUT quando Strategia B

---

## [2.0.3] – 2026-03-13

### Added
- **`docs/DATASHEETS_REFERENCE.md`**: sintesi da PDF in repo (UEDX V3.3 J2/TP/alim, M144 ES8388+BUS, Pololu 5398 VIN1/2/VOUT, relè, EC11, MAX485, LED ring)
### Changed
- **`docs/CABLING_COMPLETE.md`**, **`DISPLAY_SETUP.md`**, **`hardware/README`**: allineati ai PDF; nota **GPIO38** (touch RST vs I2S_DOUT)

---

## [2.0.2] – 2026-03-13

### Added
- **`docs/CABLING_COMPLETE.md`**: encoder + LED ring + relay TCA9535 + ES8388 I2S + RCA + XLR bilanciato (THAT1206) + jack + Pololu ORing + RJ45 DSP + secondo MAX485 DMX + box Slave esteso
- **`docs/hardware/README.md`**: istruzioni per PDF display UEDX e schematico Atomstack

---

## [2.0.1] – 2026-03-13

### Added
- **Schemi**: `docs/SCHEMATICS.md` + unico `docs/SCHEMATIC_DSP_CONNECTIONS.html` (tutti i componenti, PDF). Rimossi `schematics_graphic/electric/print*.html` (duplicati o pin obsoleti).
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

## [1.5.0] – 2025-03-05

### Bug Fix Critici
- **CRITICAL** `dsp_control.cpp`: Corretto calcolo delay per DSP CQ260D (`ms_to_samples` → `audio_ms_to_samples_dsp()`, 48kHz fisso).
- **CRITICAL** `config.h`: Pin display aggiornati al datasheet UEDX80480050E-WB-A-V3.3-SPEC.pdf.

### Aggiunte
- Sistema audio adattivo multi-sample-rate (`audio_config`, 32k/44.1k/48k/96k).
- Sample Rate Converter (`audio_src`), Audio Protection (`audio_protection`), Delay buffer PSRAM (`audio_delay_buffer`).
- Display LovyanGFX UEDX80480050E-WB-A + GT911.
- Test unitari `tests/firmware/test_sample_rate/`.

---

## [1.4.0] – 2024-12-01

### Nuove Funzionalità
- Streaming audio wireless via ESP-NOW (TX→RX), DMX512-A, REST API (AsyncWebServer).
- Preset DSP con NVS, autotune delay, LED ring encoder WS2812B.
- Driver ES8388 custom, protocollo RS-485 CQ260D reverse-engineered.
- Separazione firmware ESP32 (TX) e modulo_rx (RX).

---

## [1.x] – 2024

### Storia Iniziale
- v1.0.0: Controllo base DSP via RS-485
- v1.1.0: FFT real-time e VU meter
- v1.2.0: WiFi AP + WebUI base
- v1.3.0: Relay DPDT per routing audio
