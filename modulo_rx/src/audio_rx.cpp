#include "audio_rx.h"
#include "config.h"
#include <Arduino.h>
#include <Wire.h>
#include <driver/i2s.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <string.h>

// ======= Registri ES8388 (subset DAC) =======
#define ES8388_ADDR          0x10
#define ES8388_REG_CHIPPOWER 0x02
#define ES8388_REG_DACPOWER  0x04
#define ES8388_REG_DACCTRL1  0x17
#define ES8388_REG_DACCTRL2  0x18
#define ES8388_REG_DACCTRL3  0x19
#define ES8388_REG_DACCTRL4  0x1A
#define ES8388_REG_DACCTRL5  0x1B
#define ES8388_REG_DACVOL    0x2E

// ======= Buffer circolare audio =======
#define AUDIO_FRAME_SAMPLES  48
static int16_t  s_ringBuf[AUDIO_RING_FRAMES * AUDIO_FRAME_SAMPLES * 2] = {};
static uint32_t s_writeIdx = 0;
static uint32_t s_readIdx  = 0;
static uint32_t s_count    = 0;
static SemaphoreHandle_t s_mutex = nullptr;

static uint32_t s_underruns     = 0;
static uint32_t s_framesPlayed  = 0;

// ======= I2C helper =======
static void es8388_write(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(ES8388_ADDR);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}

// ======= Inizializzazione =======

static void init_es8388_dac() {
    Wire.beginTransmission(ES8388_ADDR);
    if (Wire.endTransmission() != 0) {
        Serial.println("[AUDIO_RX] ES8388 non trovato su I2C");
        return;
    }
    // Reset
    es8388_write(0x00, 0x80);
    delay(10);
    es8388_write(0x00, 0x00);

    es8388_write(ES8388_REG_CHIPPOWER, 0x00);   // chip on
    es8388_write(0x08, 0x00);                    // slave mode
    es8388_write(ES8388_REG_DACCTRL1,  0x18);   // 16bit I2S
    es8388_write(ES8388_REG_DACCTRL2,  0x02);   // ratio 256
    es8388_write(ES8388_REG_DACCTRL3,  0x00);   // no mute
    es8388_write(ES8388_REG_DACCTRL4,  0x00);   // vol L 0dB
    es8388_write(ES8388_REG_DACCTRL5,  0x00);   // vol R 0dB
    es8388_write(0x27, 0xB8);                    // MixerL
    es8388_write(0x2A, 0xB8);                    // MixerR
    es8388_write(0x2B, 0x80);
    es8388_write(0x2E, 0x1E);                    // LOUT1
    es8388_write(0x2F, 0x1E);                    // ROUT1
    es8388_write(0x30, 0x1E);                    // LOUT2
    es8388_write(0x31, 0x1E);                    // ROUT2
    es8388_write(ES8388_REG_DACPOWER,  0x3C);   // DAC L+R + OUT power on
    Serial.println("[AUDIO_RX] ES8388 DAC inizializzato");
}

static void init_i2s() {
    i2s_config_t cfg = {
        .mode                 = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate          = AUDIO_SAMPLE_RATE,
        .bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format       = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count        = 8,
        .dma_buf_len          = 64,
        .use_apll             = true,
        .tx_desc_auto_clear   = true,
        .fixed_mclk           = 0,
    };
    i2s_pin_config_t pins = {
        .mck_io_num   = I2S_PIN_NO_CHANGE,
        .bck_io_num   = I2S_SCK,
        .ws_io_num    = I2S_WS,
        .data_out_num = I2S_SD_OUT,
        .data_in_num  = I2S_PIN_NO_CHANGE,
    };
    i2s_driver_install(I2S_NUM_0, &cfg, 0, nullptr);
    i2s_set_pin(I2S_NUM_0, &pins);
    Serial.println("[AUDIO_RX] I2S DAC inizializzato");
}

bool audio_rx_init() {
    Wire.begin(ES8388_SDA, ES8388_SCL);
    init_es8388_dac();
    init_i2s();
    s_mutex = xSemaphoreCreateMutex();
    Serial.println("[AUDIO_RX] Modulo playback inizializzato");
    return true;
}

// ======= Buffer management =======

void audio_rx_push_samples(const int16_t* samples, uint8_t numSamples, uint8_t audioMode) {
    if (!s_mutex) return;
    if (xSemaphoreTake(s_mutex, pdMS_TO_TICKS(2)) != pdTRUE) return;

    // Se buffer pieno, scarta frame più vecchio
    if (s_count >= AUDIO_RING_FRAMES) {
        s_readIdx = (s_readIdx + 1) % AUDIO_RING_FRAMES;
        s_count--;
    }

    size_t pos = s_writeIdx * AUDIO_FRAME_SAMPLES * 2;

    if (audioMode == 1) {
        // Stereo: copia direttamente
        uint8_t copyN = (numSamples > AUDIO_FRAME_SAMPLES) ? AUDIO_FRAME_SAMPLES : numSamples;
        // Bounds check: verify write stays within ring buffer
        if (pos + (size_t)copyN * 2 <= sizeof(s_ringBuf) / sizeof(s_ringBuf[0])) {
            memcpy(&s_ringBuf[pos], samples, copyN * 2 * sizeof(int16_t));
        }
    } else {
        // Mono o Sub: duplica su L e R
        uint8_t copyN = (numSamples > AUDIO_FRAME_SAMPLES) ? AUDIO_FRAME_SAMPLES : numSamples;
        for (int i = 0; i < copyN; i++) {
            size_t idx = pos + (size_t)i * 2;
            if (idx + 1 >= sizeof(s_ringBuf) / sizeof(s_ringBuf[0])) break;
            s_ringBuf[idx]     = samples[i];  // L
            s_ringBuf[idx + 1] = samples[i];  // R
        }
    }

    s_writeIdx = (s_writeIdx + 1) % AUDIO_RING_FRAMES;
    s_count++;

    xSemaphoreGive(s_mutex);
}

// ======= Task playback =======

void audio_rx_task(void* param) {
    Serial.println("[AUDIO_RX] Task playback avviato");
    int16_t silenceBuf[AUDIO_FRAME_SAMPLES * 2] = {};

    for (;;) {
        int16_t frameBuf[AUDIO_FRAME_SAMPLES * 2] = {};
        bool hasData = false;

        if (s_mutex && xSemaphoreTake(s_mutex, pdMS_TO_TICKS(2)) == pdTRUE) {
            if (s_count > 0) {
                size_t pos = s_readIdx * AUDIO_FRAME_SAMPLES * 2;
                memcpy(frameBuf, &s_ringBuf[pos], sizeof(frameBuf));
                s_readIdx = (s_readIdx + 1) % AUDIO_RING_FRAMES;
                s_count--;
                hasData = true;
                s_framesPlayed++;
            } else {
                s_underruns++;
            }
            xSemaphoreGive(s_mutex);
        }

        size_t bytesWritten = 0;
        if (hasData) {
            i2s_write(I2S_NUM_0, frameBuf, sizeof(frameBuf), &bytesWritten, pdMS_TO_TICKS(10));
        } else {
            // Silence per evitare click
            i2s_write(I2S_NUM_0, silenceBuf, sizeof(silenceBuf), &bytesWritten, pdMS_TO_TICKS(10));
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

uint32_t audio_rx_get_underruns()     { return s_underruns; }
uint32_t audio_rx_get_frames_played() { return s_framesPlayed; }
