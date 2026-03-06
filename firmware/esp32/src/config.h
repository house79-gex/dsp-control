#pragma once

// ======= CONFIGURAZIONE PIN ESP32-S3 =======

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
// Riferimento: SPEC pag.9, GT911 datasheet
#define TOUCH_SCL       20   // GPIO20 – Touch I2C clock
#define TOUCH_SDA       19   // GPIO19 – Touch I2C data
#define TOUCH_RST       38   // GPIO38 – Touch reset (active LOW)
#define TOUCH_INT       18   // GPIO18 – Touch interrupt
// Indirizzo I2C GT911: 0x5D (INT=LOW @ reset) oppure 0x14 (INT=HIGH @ reset)
#define TOUCH_I2C_ADDR  0x5D

// I2S - ES8388
#define I2S_SCK     15  // TODO: verificare
#define I2S_WS      16  // TODO: verificare
#define I2S_SD_OUT  17  // TODO: verificare
#define I2S_SD_IN   18  // TODO: verificare

// RS-485 MAX485
#define RS485_TX    43  // UART TX
#define RS485_RX    44  // UART RX
#define RS485_DE    21  // Driver Enable (HIGH = trasmissione) – NOTA: condiviso con LCD_R3
#define RS485_RE    20  // Receiver Enable (LOW = ricezione)

// DMX512 (UART1)
#define DMX_TX       1  // GPIO1 – DMX TX output – NOTA: condiviso con LCD_B4
#define DMX_DE       2  // GPIO2 – DMX Driver Enable – NOTA: condiviso con LCD_BL_EN

// Relay DPDT (optoisolatore)
#define RELAY_PIN   45  // HIGH = TestTone, LOW = MixerPassThrough – NOTA: condiviso con LCD_R0

// LED di stato
#define LED_STATUS  48  // LED integrato ESP32-S3 – NOTA: condiviso con LCD_R1

// WiFi AP
#define WIFI_SSID   "SISTEMA_AUDIO_01"
#define WIFI_PASS   "audio1234"

// LED Ring encoder (WS2812B)
#define LED_RING_VOL_PIN    39  // TODO: verificare GPIO
#define LED_RING_BAL_PIN    40  // TODO: verificare GPIO
#define LED_RING_COUNT      16  // Numero LED per anello

// Encoder rotativi
#define ENCODER_VOL_A       41  // TODO: verificare GPIO encoder volume A
#define ENCODER_VOL_B       42  // TODO: verificare GPIO encoder volume B
#define ENCODER_BAL_A       47  // TODO: verificare GPIO encoder balance A
#define ENCODER_BAL_B       46  // TODO: verificare GPIO encoder balance B
// NOTA: GPIO 48 è usato da LED_STATUS, usare GPIO 46 per ENCODER_BAL_B

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
