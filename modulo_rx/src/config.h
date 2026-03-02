#pragma once

// ======= Configurazione Modulo RX =======
// MY_CHANNEL_ID è definito in platformio.ini (1 o 2)
// Cambiare il build flag per il secondo modulo

#ifndef MY_CHANNEL_ID
#define MY_CHANNEL_ID 1
#endif

// ——— Pin I2S – ES8388 DAC Playback ———
#define I2S_SCK       15
#define I2S_WS        16
#define I2S_SD_OUT    17

// I2C per ES8388
#define ES8388_SDA    5
#define ES8388_SCL    6

// ——— Pin UART1 – RS-485 verso CQ260D ———
#define RS485_TX_PIN  43
#define RS485_RX_PIN  44
#define RS485_DE_PIN  21   // Driver Enable (HIGH = trasmissione)

// ——— Pin UART2 – DMX512 output ———
#define DMX_TX_PIN     1
#define DMX_DE_PIN     2   // Driver Enable DMX (sempre HIGH)

// ——— LED stato ———
#define LED_STATUS_PIN 48   // LED integrato ESP32-S3 (WS2812B su alcune board)

// ——— Parametri audio ———
#define AUDIO_SAMPLE_RATE  48000
#define AUDIO_BITS         16
#define AUDIO_RING_FRAMES  16    // Buffer circolare 16 frame per compensare jitter wireless

// ——— Parametri ESP-NOW ———
#define WIRELESS_TIMEOUT_MS  3000  // Timeout offline
