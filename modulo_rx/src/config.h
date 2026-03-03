// ======= GPIO PINS =======
// RS485 (controllo luci/mixer)
#define RS485_RX_PIN     18
#define RS485_TX_PIN     19
#define RS485_DE_PIN     21  // Driver Enable (trasmissione)

// DMX512 output
#define DMX_TX_PIN       22
#define DMX_DE_PIN       23  // Driver Enable

// I2S - ES8388 DAC
#define I2S_SCK          36  // I2S bit clock (BCK)
#define I2S_WS           35  // I2S word select (LRCLK)
#define I2S_SD_OUT       37  // I2S data out (DOUT)

// I2C - ES8388 codec control
#define I2C_SDA          8   // I2C data
#define I2C_SCL          9   // I2C clock
#define ES8388_I2C_ADDR  0x10

// ======= AUDIO =======
#define AUDIO_SAMPLE_RATE                44100  // Hz – match master TX e SC LIVE 4
#define AUDIO_BIT_DEPTH                  16     // bit – DAC output
#define AUDIO_CHANNELS                   2      // Stereo
#define WIRELESS_AUDIO_SAMPLES_PER_FRAME 256    // Samples per wireless packet

// ======= WIRELESS =======
#define WIRELESS_TIMEOUT_MS  5000  // Timeout offline dopo 5 secondi senza pacchetti