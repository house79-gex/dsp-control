#include "config.h"
#include <Arduino.h>
#include <driver/i2s.h>
#include <Wire.h>

// ======= Modulo RX – Inizializzazione Audio DAC =======
// Configura ES8388 DAC e I2S per riproduzione audio wireless a 44.1 kHz

#define I2S_PORT        I2S_NUM_0
#define I2S_BUFFER_SIZE 256

static float s_outputVolumePercent = 80.0f;  // Volume default 80%

// ——— Helper: scrittura registro ES8388 via I2C ———
static void es8388_write_reg(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(ES8388_I2C_ADDR);
    Wire.write(reg);
    Wire.write(value);
    if (Wire.endTransmission() != 0) {
        Serial.printf("[RX] ES8388 writeReg(0x%02X) errore\n", reg);
    }
}

void audio_rx_init() {
    Serial.println("[RX] Inizializzazione ES8388 DAC...");

    // Inizializza I2C per controllo codec.
    // Wire.begin() è idempotente su ESP32 – sicuro chiamare più volte.
    Wire.begin();
    Wire.beginTransmission(ES8388_I2C_ADDR);
    if (Wire.endTransmission() != 0) {
        Serial.println("[RX] ES8388 non risponde su I2C");
    } else {
        Serial.printf("[RX] ES8388 trovato su I2C addr 0x%02X\n", ES8388_I2C_ADDR);
        // Configurazione DAC per 44.1 kHz playback
        // Registro 0x17 (SDP_IN): word length 16-bit → wl = 0b11
        es8388_write_reg(0x17, 0x03);
        // Registro 0x18 (SDP_OUT): word length 16-bit → wl = 0b11
        es8388_write_reg(0x18, 0x03);
    }

    // Configurazione I2S master TX per DAC
    i2s_config_t i2s_config = {
        .mode                = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate         = AUDIO_SAMPLE_RATE,
        .bits_per_sample     = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format      = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format= I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags    = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count       = 4,
        .dma_buf_len         = I2S_BUFFER_SIZE,
        .use_apll            = false,
        .tx_desc_auto_clear  = true,
        .fixed_mclk          = 0
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num   = I2S_SCK,
        .ws_io_num    = I2S_WS,
        .data_out_num = I2S_SD_OUT,
        .data_in_num  = I2S_PIN_NO_CHANGE
    };

    esp_err_t err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    if (err != ESP_OK) {
        Serial.printf("[RX] Errore init I2S: %d\n", err);
        return;
    }
    i2s_set_pin(I2S_PORT, &pin_config);

    Serial.printf("[RX] ES8388 DAC configurato: %d Hz, %d bit, volume %d%%\n",
                  AUDIO_SAMPLE_RATE, AUDIO_BIT_DEPTH, (int)s_outputVolumePercent);
}

// Riproduce campioni audio ricevuti via ESP-NOW
void audio_rx_play_samples(const int16_t* samples, uint8_t numSamples, bool isStereo) {
    if (numSamples == 0 || !samples) return;

    // Buffer stereo interleaved
    static int16_t playBuf[WIRELESS_AUDIO_SAMPLES_PER_FRAME * 2];
    uint8_t outSamples = numSamples;
    if (outSamples > WIRELESS_AUDIO_SAMPLES_PER_FRAME) {
        outSamples = WIRELESS_AUDIO_SAMPLES_PER_FRAME;
    }

    // Applica volume (scalatura lineare)
    float vol = s_outputVolumePercent / 100.0f;
    for (uint8_t i = 0; i < outSamples; i++) {
        int16_t s = isStereo ? samples[i * 2] : samples[i];
        int16_t sR = isStereo ? samples[i * 2 + 1] : samples[i];
        playBuf[i * 2]     = (int16_t)(s  * vol);
        playBuf[i * 2 + 1] = (int16_t)(sR * vol);
    }

    size_t bytesWritten = 0;
    i2s_write(I2S_PORT, playBuf, outSamples * 2 * sizeof(int16_t),
              &bytesWritten, pdMS_TO_TICKS(10));
}

void audio_rx_set_volume(float volumePercent) {
    s_outputVolumePercent = constrain(volumePercent, 0.0f, 100.0f);
}
