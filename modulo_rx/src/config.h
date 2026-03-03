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

// ======= I2C AUDIO CODEC =======
#define ES8388_I2C_ADDR  0x10  // Indirizzo I2C ES8388 DAC

// ======= I2S PINS (Audio DAC) =======
#define I2S_SCK      5   // I2S Bit Clock
#define I2S_WS       25  // I2S Word Select (LRCLK)
#define I2S_SD_OUT   26  // I2S Data Out (DAC)

// ======= AUDIO CONFIG =======
#define AUDIO_SAMPLE_RATE  44100  // 44.1 kHz (SC LIVE 4 match)
#define AUDIO_BIT_DEPTH    16     // 16-bit audio
#define WIRELESS_AUDIO_SAMPLES_PER_FRAME  64  // Campioni per frame wireless