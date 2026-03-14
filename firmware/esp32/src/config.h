#pragma once

// ======= CONFIGURAZIONE PIN ESP32-S3 #1 (MASTER – Display + Audio) =======
// Board: UEDX80480050E-WB-A (5" 800×480 RGB, 16MB Flash, 8MB PSRAM)
// Architettura: Dual-ESP32 – questo è il Master; vedere firmware/esp32_slave/ per lo Slave

// ——— Display RGB UEDX80480050E-WB-A 5" (800×480, 40-pin RGB interface) ———
// Riferimento: docs/UEDX80480050E-WB-A-V3.3-SPEC.pdf, pagine 6-9

// Canale rosso (R0-R4)
#define LCD_R0  45   // GPIO45 – RGB R0  (SPEC pag.6)
#define LCD_R1  48   // GPIO48 – RGB R1
#define LCD_R2  47   // GPIO47 – RGB R2
#define LCD_R3  21   // GPIO21 – RGB R3
#define LCD_R4  14   // GPIO14 – RGB R4

// Canale verde (G0-G5)
#define LCD_G0   5   // GPIO5  – RGB G0  (SPEC pag.7)
#define LCD_G1   6   // GPIO6  – RGB G1
#define LCD_G2   7   // GPIO7  – RGB G2
#define LCD_G3  15   // GPIO15 – RGB G3
#define LCD_G4  16   // GPIO16 – RGB G4
#define LCD_G5   4   // GPIO4  – RGB G5

// Canale blu (B0-B4)
#define LCD_B0   8   // GPIO8  – RGB B0  (SPEC pag.7)
#define LCD_B1   3   // GPIO3  – RGB B1
#define LCD_B2  46   // GPIO46 – RGB B2
#define LCD_B3   9   // GPIO9  – RGB B3
#define LCD_B4   1   // GPIO1  – RGB B4

// Segnali sincronia RGB
#define LCD_PCLK  42  // GPIO42 – Pixel clock   (SPEC pag.8)
#define LCD_HSYNC 39  // GPIO39 – H-Sync
#define LCD_VSYNC 41  // GPIO41 – V-Sync
#define LCD_DE    40  // GPIO40 – Data Enable

// Backlight
#define LCD_BL_EN  2  // GPIO2  – Backlight enable (PWM, HIGH=on)  (SPEC pag.9)

// ——— Touch GT911 (I2C) ———
// UEDX V3.3 SPEC: GPIO38 = TP pin 6 (reset CTP). Per usare GPIO38 anche come I2S_DOUT
// serve spostare RST su TCA9535. Altrimenti USE_SLAVE_ES8388_DOUT=1 e DOUT da Slave GPIO4.
#define TOUCH_SCL       20   // GPIO20 – Touch I2C clock
#define TOUCH_SDA       19   // GPIO19 – Touch I2C data
// Reset touch: ideale TCA9535 P1_0 (libera GPIO38 per I2S se cablato così)
// Indirizzo I2C GT911: 0x5D (INT=LOW @ reset) oppure 0x14 (INT=HIGH @ reset)
#define TOUCH_I2C_ADDR  0x5D

// ——— I2S ES8388 (UEDX V3.3 – solo pin liberi con display ON + NO SD) ———
// SPEC: GPIO38 = reset GT911 (TP); GPIO10 = SD-CS → LIBERO se non usi SD → I2S_DOUT su 10
// SPEC: GPIO17/18 = "not used" → IPC. Opzionale: DOUT da Slave se USE_SLAVE_ES8388_DOUT=1
#ifndef USE_SLAVE_ES8388_DOUT
#define USE_SLAVE_ES8388_DOUT  0
#endif
#define I2S_BCLK    12   // RTP-CLK
#define I2S_WS      13   // RTP-DOUT
#define I2S_DIN     11   // RTP-DIN ← ES8388 ASDOUT
#if USE_SLAVE_ES8388_DOUT
#define I2S_DOUT    (-1) // Slave GPIO4 → ES8388 DSDIN
#else
#define I2S_DOUT    10   // ex SD-CS – verso ES8388 DSDIN (NO scheda SD)
#endif
#define I2S_MCLK    -1

// ——— UART IPC verso ESP32 #2 (Slave) ———
// Protocollo binario con sync+CRC8; vedere ipc_master.h per il protocollo
#define IPC_UART_NUM    1        // UART1
#define IPC_UART_TX     17       // GPIO17 – IPC TX → ESP32 #2 GPIO2 (RX)
#define IPC_UART_RX     18       // GPIO18 – IPC RX ← ESP32 #2 GPIO1 (TX)
#define IPC_UART_BAUD   115200   // Baudrate IPC

// ——— RS-485 DSP CQ260D ———
// GPIO10 = I2S_DOUT se no SD → DE non su 10. Usa transceiver RS485 auto-direzionale (senza DE)
// oppure -1 e ponticella DE su Slave (vedi docs/UEDX_V33_FREE_PINS.md)
#define RS485_UART_NUM  2
#define RS485_TX        43
#define RS485_RX        44
#ifndef RS485_DE_RE
#define RS485_DE_RE     (-1)   // -1 = modulo auto-RS485 (DE/RE interni)
#endif
#define RS485_DE        RS485_DE_RE
#define RS485_RE        RS485_DE_RE

// ——— DMX512 ———
// Spostato su ESP32 #2 (Slave); non più su Master per liberare GPIO e CPU
// Vedere firmware/esp32_slave/src/config.h

// ——— Relay DPDT e LED di stato ———
// Spostati su GPIO expander TCA9535 (I2C addr 0x20) per liberare GPIO45/48 per display
// TCA9535 P0_0 = Relay DPDT (HIGH=TestTone, LOW=MixerPassThrough)
// TCA9535 P0_1 = LED Status
#define TCA9535_I2C_ADDR    0x20   // Indirizzo I2C GPIO expander
#define TCA9535_RELAY_PIN   0      // P0_0 – Relay DPDT
#define TCA9535_LED_STATUS  1      // P0_1 – LED di stato
// Alias per compatibilità (usa valori TCA9535 virtuale via I2C)
#define RELAY_PIN   TCA9535_RELAY_PIN
#define LED_STATUS  TCA9535_LED_STATUS

// Encoder + LED sullo Slave (Master solo IPC – libera GPIO UEDX)
#ifndef USE_SLAVE_PERIPHERALS
#define USE_SLAVE_PERIPHERALS  1
#endif

// WiFi AP
#define WIFI_SSID   "SISTEMA_AUDIO_01"
#define WIFI_PASS   "audio1234"

// ===== ENCODER ROTATIVI con PULSANTI =====
// Encoder Volume (con LED ring 12 LED WS2812B)
// NOTA: GPIO 35-37 liberi; verificare schema PCB prima di assemblare
#define ENCODER_VOL_A       35   // GPIO35 (CLK encoder volume)
#define ENCODER_VOL_B       36   // GPIO36 (DT encoder volume)
#define ENCODER_VOL_BTN     37   // GPIO37 (pulsante encoder volume)
// NOTA: GPIO27 potenzialmente condiviso con PSRAM interno – verificare schema
#define LED_RING_VOL_PIN    27   // GPIO27 (data WS2812B ring volume)

// Encoder Balance (con LED ring 12 LED WS2812B)
// NOTA: GPIO11/12 in conflitto con I2S_DIN/BCLK → usare GPIO33/34 come alternativa
#define ENCODER_BAL_A       33   // GPIO33 (CLK encoder balance – alternativa a GPIO11)
#define ENCODER_BAL_B       34   // GPIO34 (DT encoder balance – alternativa a GPIO12)
// NOTA: GPIO26/28 potenzialmente condivisi con PSRAM interno – verificare schema
#define ENCODER_BAL_BTN     26   // GPIO26 (pulsante encoder balance)
#define LED_RING_BAL_PIN    28   // GPIO28 (data WS2812B ring balance)

#define LED_RING_COUNT      12   // 12 LED per ring (standard)
#define LED_RING_BRIGHTNESS 80   // 0-255 (80 = ~30% luminosità)

// ======= CONFIGURAZIONE AUDIO =======
// Ottimizzato per sorgente Denon DJ SC LIVE 4 (output +18 dBu, 44.1 kHz nativo)
#define AUDIO_SAMPLE_RATE_DEFAULT         44100  // Hz – default/fallback per auto-detect (SC LIVE 4)
// Mantenuto per compatibilità con codice esistente
#define AUDIO_SAMPLE_RATE                 AUDIO_SAMPLE_RATE_DEFAULT
// Il DSP CQ260D processa SEMPRE a 48kHz fisso (da datasheet CQ260D)
#define AUDIO_PROCESSING_SAMPLE_RATE      48000  // Hz – sample rate DSP (fisso hardware)
#define AUDIO_BIT_DEPTH                   24     // bit – ES8388 supporta 24-bit
#define AUDIO_CHANNELS                    2      // Stereo

// Frame audio wireless: 44100 Hz / 1000 frame/sec ≈ 44 campioni per frame
#define WIRELESS_AUDIO_SAMPLES_PER_FRAME  44

// ES8388 codec I2C address (default: 0x10 per CSADDR=0, 0x11 per CSADDR=1)
#define ES8388_I2C_ADDR                   0x10
