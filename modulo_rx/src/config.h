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

// ======= ES8388 I2C =======
#define ES8388_I2C_ADDR  0x10

// ======= AUDIO =======
#define AUDIO_SAMPLE_RATE              44100
#define AUDIO_BIT_DEPTH                16

// I2S Pin Configuration for ESP32-S3
#define I2S_SCK    16
#define I2S_WS     17
#define I2S_SD_OUT 18

// ======= WIRELESS AUDIO =======
#define WIRELESS_AUDIO_SAMPLES_PER_FRAME  128