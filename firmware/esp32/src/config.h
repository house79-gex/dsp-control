#pragma once

// ======= CONFIGURAZIONE PIN ESP32-S3 =======
// TODO: impostare pin corretti per il modulo specifico

// Display & Touch (LVGL)
#define LCD_MOSI    11  // TODO: verificare
#define LCD_MISO    13  // TODO: verificare
#define LCD_SCLK    12  // TODO: verificare
#define LCD_CS      10  // TODO: verificare
#define LCD_DC       9  // TODO: verificare
#define LCD_RST      8  // TODO: verificare
#define LCD_BL       7  // TODO: verificare
#define TOUCH_SDA    5  // TODO: verificare (I2C touch)
#define TOUCH_SCL    6  // TODO: verificare

// I2S - ES8388
#define I2S_SCK     15  // TODO: verificare
#define I2S_WS      16  // TODO: verificare
#define I2S_SD_OUT  17  // TODO: verificare
#define I2S_SD_IN   18  // TODO: verificare

// RS-485 MAX485
#define RS485_TX    43  // UART TX
#define RS485_RX    44  // UART RX
#define RS485_DE    21  // Driver Enable (HIGH = trasmissione)
#define RS485_RE    20  // Receiver Enable (LOW = ricezione)

// Relay DPDT (optoisolatore)
#define RELAY_PIN   45  // HIGH = TestTone, LOW = MixerPassThrough

// LED di stato
#define LED_STATUS  48  // LED integrato ESP32-S3

// WiFi AP
#define WIFI_SSID   "SISTEMA_AUDIO_01"
#define WIFI_PASS   "audio1234"
