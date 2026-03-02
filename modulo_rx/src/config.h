#pragma once

// ======= CONFIGURAZIONE MODULO RX =======
// Ricevitore wireless audio (ESP-NOW) → ES8388 DAC → XLR Out

// I2S - ES8388 DAC
#define I2S_SCK      15
#define I2S_WS       16
#define I2S_SD_OUT   17

// ES8388 I2C
#define ES8388_I2C_ADDR  0x10

// WiFi AP del master (per pairing)
#define MASTER_SSID  "SISTEMA_AUDIO_01"

// ======= CONFIGURAZIONE AUDIO =======
// Deve corrispondere al master TX (match SC LIVE 4)
#define AUDIO_SAMPLE_RATE                 44100  // Hz – match master TX e SC LIVE 4
#define AUDIO_BIT_DEPTH                   16     // bit – DAC output
#define AUDIO_CHANNELS                    2      // Stereo

// Frame audio: 44 campioni per frame @ 44.1 kHz (≈1 ms per frame)
#define WIRELESS_AUDIO_SAMPLES_PER_FRAME  44
