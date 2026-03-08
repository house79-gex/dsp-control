#include "audio_slave.h"
#include "config.h"
#include <driver/i2s.h>
#include <esp_dsp.h>
#include <math.h>

static int16_t s_audioBuf[FFT_SIZE * 2];   // Stereo interleaved
static float   s_fftInput[FFT_SIZE * 2];   // Complesso re,im interleaved
static float   s_window[FFT_SIZE];
static float   s_fftBins[FFT_NUM_BANDS];   // Bande audio-reactive
static float   s_peakDb = -60.0f;
static portMUX_TYPE s_mux = portMUX_INITIALIZER_UNLOCKED;

// Frequenze di taglio per le 6 bande (Hz)
static const float kBandLow[FFT_NUM_BANDS]  = {  20,   60,  250, 1000,  4000, 10000 };
static const float kBandHigh[FFT_NUM_BANDS] = {  60,  250, 1000, 4000, 10000, 20000 };
static const char* kBandName[FFT_NUM_BANDS] = { "Bass","LowMid","Mid","HiMid","Treble","Presence" };

static float magnitude_band(const float* fftData, float freqLow, float freqHigh) {
    const float freqRes = 48000.0f / FFT_SIZE;
    int binLow  = (int)(freqLow  / freqRes);
    int binHigh = (int)(freqHigh / freqRes);
    if (binLow  < 1)            binLow  = 1;
    if (binHigh >= FFT_SIZE / 2) binHigh = FFT_SIZE / 2 - 1;
    if (binLow  > binHigh)       binLow  = binHigh;

    float maxMag = 0.0f;
    for (int bin = binLow; bin <= binHigh; bin++) {
        float re  = fftData[bin * 2];
        float im  = fftData[bin * 2 + 1];
        float mag = sqrtf(re * re + im * im);
        if (mag > maxMag) maxMag = mag;
    }
    return maxMag;
}

void audio_slave_init() {
    // Configura I2S come Slave RX-only
    // BCLK e WS ricevuti dal Master (non generati da questo ESP32)
    i2s_config_t cfg = {
        .mode             = (i2s_mode_t)(I2S_MODE_SLAVE | I2S_MODE_RX),
        .sample_rate      = 48000,
        .bits_per_sample  = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format   = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count    = 8,
        .dma_buf_len      = 256,
        .use_apll         = false,   // Slave non genera clock
        .tx_desc_auto_clear = false,
        .fixed_mclk       = 0
    };

    i2s_pin_config_t pin_cfg = {
        .bck_io_num       = I2S_BCLK_IN,           // GPIO5 ← da Master
        .ws_io_num        = I2S_WS_IN,             // GPIO6 ← da Master
        .data_out_num     = I2S_PIN_NO_CHANGE,      // Non usa DAC
        .data_in_num      = I2S_DIN                 // GPIO7 ← ES8388 (parallelo con Master)
    };

    esp_err_t err = i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL);
    if (err != ESP_OK) {
        Serial.printf("[AUDIO_SLAVE] Errore init I2S driver: %d\n", err);
        return;
    }
    err = i2s_set_pin(I2S_NUM_0, &pin_cfg);
    if (err != ESP_OK) {
        Serial.printf("[AUDIO_SLAVE] Errore config pin I2S: %d\n", err);
        return;
    }

    // Inizializza tabelle FFT e finestra Hann
    dsps_fft2r_init_fc32(nullptr, FFT_SIZE);
    for (int i = 0; i < FFT_SIZE; i++) {
        s_window[i] = 0.5f * (1.0f - cosf(2.0f * M_PI * i / (FFT_SIZE - 1)));
    }

    Serial.printf("[AUDIO_SLAVE] I2S Slave RX configurato (BCLK=GPIO%d, WS=GPIO%d, DIN=GPIO%d)\n",
                  I2S_BCLK_IN, I2S_WS_IN, I2S_DIN);
}

void audio_slave_process_fft() {
    size_t bytesRead = 0;
    i2s_read(I2S_NUM_0, s_audioBuf, sizeof(s_audioBuf), &bytesRead, pdMS_TO_TICKS(10));
    if (bytesRead < sizeof(s_audioBuf)) return;

    // Calcola peak
    float peak = 0.0f;
    for (int i = 0; i < FFT_SIZE; i++) {
        float s = fabsf((float)s_audioBuf[i * 2] / 32768.0f);
        if (s > peak) peak = s;
    }
    float peakDb = (peak > 1e-6f) ? 20.0f * log10f(peak) : -60.0f;

    // Prepara input FFT con finestra Hann (canale sinistro, mono)
    for (int i = 0; i < FFT_SIZE; i++) {
        float sample = ((float)s_audioBuf[i * 2] / 32768.0f) * s_window[i];
        s_fftInput[i * 2]     = sample;
        s_fftInput[i * 2 + 1] = 0.0f;
    }

    // Esegui FFT
    dsps_fft2r_fc32(s_fftInput, FFT_SIZE);
    dsps_bit_rev_fc32(s_fftInput, FFT_SIZE);
    dsps_cplx2reC_fc32(s_fftInput, FFT_SIZE);

    // Estrai 6 bande e normalizza (0.0-1.0)
    const float normFactor = FFT_SIZE / 4.0f;
    float bins[FFT_NUM_BANDS];
    for (int b = 0; b < FFT_NUM_BANDS; b++) {
        float mag = magnitude_band(s_fftInput, kBandLow[b], kBandHigh[b]);
        bins[b] = fminf(1.0f, mag / normFactor);
    }

    portENTER_CRITICAL(&s_mux);
    memcpy(s_fftBins, bins, sizeof(bins));
    s_peakDb = peakDb;
    portEXIT_CRITICAL(&s_mux);
}

const float* audio_slave_get_fft_bins() {
    return s_fftBins;
}

float audio_slave_get_peak_db() {
    float val;
    portENTER_CRITICAL(&s_mux);
    val = s_peakDb;
    portEXIT_CRITICAL(&s_mux);
    return val;
}
