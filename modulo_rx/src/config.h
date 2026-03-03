// ======= GPIO PINS =======
// RS485 (controllo luci/mixer)
#define RS485_RX_PIN     18
#define RS485_TX_PIN     19
#define RS485_DE_PIN     21  // Driver Enable (trasmissione)

// DMX512 output
#define DMX_TX_PIN       22
#define DMX_DE_PIN       23  // Driver Enable

// ======= WIRELESS =======
#define WIRELESS_TIMEOUT_MS  5000  // Timeout offline dopo 5 secondi senza pacchetti

// ======= AUDIO CONFIG =======
// ES8388 DAC I2C
#define ES8388_I2C_ADDR  0x10  // Indirizzo I2C ES8388 standard

// I2S GPIO pins (ESP32-S3) – TODO: verificare pin GPIO hardware
#define I2S_SCK      5   // Bit Clock (SCK/BCK)
#define I2S_WS       25  // Word Select (WS/LRCK)
#define I2S_SD_OUT   26  // Serial Data Out (SDOUT)

// Audio parameters
#define AUDIO_SAMPLE_RATE  44100  // 44.1 kHz (match SC LIVE 4)
#define AUDIO_BIT_DEPTH    16     // 16-bit per sample

// Wireless audio frame size
#define WIRELESS_AUDIO_SAMPLES_PER_FRAME  64  // Campioni per frame wireless