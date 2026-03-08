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
// Riferimento: SPEC pag.9, GT911 datasheet
#define TOUCH_SCL       20   // GPIO20 – Touch I2C clock
#define TOUCH_SDA       19   // GPIO19 – Touch I2C data
// NOTA: GPIO38 riassegnato a I2S_DOUT (v2.0); TOUCH_RST collegare a expander TCA9535 P1_0
// NOTA: GPIO18 riassegnato a IPC_UART_RX (v2.0); touch interrupt gestito in polling su GT911
// Indirizzo I2C GT911: 0x5D (INT=LOW @ reset) oppure 0x14 (INT=HIGH @ reset)
#define TOUCH_I2C_ADDR  0x5D

// ——— I2S ES8388 (Master Mode – genera BCLK/WS) ———
// BCLK e WS escono da ESP32 #1 verso ES8388 E verso ESP32 #2 (I2S Slave)
#define I2S_BCLK    12   // GPIO12 – Bit Clock out → ES8388 + ESP32 #2 GPIO5
#define I2S_WS      13   // GPIO13 – Word Select out → ES8388 + ESP32 #2 GPIO6
#define I2S_DIN     11   // GPIO11 – Data In ← ES8388 DOUT (condiviso con ESP32 #2)
#define I2S_DOUT    38   // GPIO38 – Data Out → ES8388 DIN (DAC playback)
#define I2S_MCLK    -1   // Non usato (ES8388 usa BCLK come riferimento clock)

// ——— UART IPC verso ESP32 #2 (Slave) ———
// Protocollo binario con sync+CRC8; vedere ipc_master.h per il protocollo
#define IPC_UART_NUM    1        // UART1
#define IPC_UART_TX     17       // GPIO17 – IPC TX → ESP32 #2 GPIO2 (RX)
#define IPC_UART_RX     18       // GPIO18 – IPC RX ← ESP32 #2 GPIO1 (TX)
#define IPC_UART_BAUD   115200   // Baudrate IPC

// ——— RS-485 DSP CQ260D ———
// UART2; DE e RE uniti su singolo GPIO (HIGH=TX, LOW=RX)
#define RS485_UART_NUM  2    // UART2
#define RS485_TX        43   // GPIO43 – UART2 TX
#define RS485_RX        44   // GPIO44 – UART2 RX
#define RS485_DE_RE     10   // GPIO10 – DE+RE uniti (HIGH=trasmissione, LOW=ricezione)

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

// WiFi AP
#define WIFI_SSID   "SISTEMA_AUDIO_01"
#define WIFI_PASS   "audio1234"

// LED Ring encoder (WS2812B)
// NOTA: GPIO39/40/41/42 condivisi con LCD_HSYNC/DE/VSYNC/PCLK
// Spostare su TCA9535 o usare GPIO liberi dopo verifica schema elettrico
#define LED_RING_VOL_PIN    39  // TODO: verificare GPIO (conflitto LCD_HSYNC)
#define LED_RING_BAL_PIN    40  // TODO: verificare GPIO (conflitto LCD_DE)
#define LED_RING_COUNT      16  // Numero LED per anello

// Encoder rotativi
// NOTA: GPIO41/42/47/46 alcuni condivisi con segnali LCD – verificare schema
#define ENCODER_VOL_A       41  // TODO: verificare GPIO encoder volume A
#define ENCODER_VOL_B       42  // TODO: verificare GPIO encoder volume B
#define ENCODER_BAL_A       47  // GPIO47 – encoder balance A
#define ENCODER_BAL_B       46  // GPIO46 – encoder balance B

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
