#include "audio_mode.h"
#include "config.h"
#include <driver/i2s.h>
#include <math.h>
#include <esp_dsp.h>

// ======= Configurazione I2S =======
// TODO: verificare numeri porta e pin con il modulo ES8388 fisico
#define I2S_PORT        I2S_NUM_0
#define I2S_SAMPLE_RATE 48000
#define I2S_BUFFER_SIZE 256
#define FFT_SIZE              512
#define NUM_BANDS             32
// Fattore di normalizzazione FFT: FFT_SIZE/4 perché le magnitudini tipiche dopo
// dsps_cplx2reC_fc32 sono nell'ordine di N/4 per segnali a piena ampiezza
#define FFT_NORM_FACTOR       (FFT_SIZE / 4.0f)

static AudioMode s_currentMode = AudioMode::MixerPassThrough;

// Buffer per generazione tono sinusoidale
static int16_t s_toneBuf[I2S_BUFFER_SIZE * 2];  // stereo interleaved

// FFT e VU meter
static float s_fftInput[FFT_SIZE * 2];   // Complesso: re,im interleaved
static float s_window[FFT_SIZE];
static FftResult s_fftResult = {};
static portMUX_TYPE s_fftMux = portMUX_INITIALIZER_UNLOCKED;
static float s_peakDb = -60.0f;
static float s_levelLeft = 0.0f;
static float s_levelRight = 0.0f;
static bool s_fftInitialized = false;

void audio_init() {
    // Configura relay in modalità default (MixerPassThrough)
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);

    // Configurazione I2S per ES8388
    // TODO: adattare i parametri al codec ES8388 specifico (init via I2C se necessario)
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
        .sample_rate = I2S_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = I2S_BUFFER_SIZE,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num   = I2S_SCK,
        .ws_io_num    = I2S_WS,
        .data_out_num = I2S_SD_OUT,
        .data_in_num  = I2S_SD_IN
    };

    esp_err_t err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    if (err != ESP_OK) {
        Serial.printf("[AUDIO] Errore init I2S driver: %d\n", err);
        return;
    }

    err = i2s_set_pin(I2S_PORT, &pin_config);
    if (err != ESP_OK) {
        Serial.printf("[AUDIO] Errore config pin I2S: %d\n", err);
        return;
    }

    Serial.println("[AUDIO] I2S inizializzato");

    // Inizializza tabelle FFT e finestra Hann
    dsps_fft2r_init_fc32(nullptr, FFT_SIZE);
    for (int i = 0; i < FFT_SIZE; i++) {
        s_window[i] = 0.5f * (1.0f - cosf(2.0f * M_PI * i / (FFT_SIZE - 1)));
    }
    s_fftInitialized = true;
    Serial.println("[AUDIO] FFT inizializzato");
}

void setAudioMode(AudioMode mode) {
    s_currentMode = mode;

    if (mode == AudioMode::MixerPassThrough) {
        // Relay in posizione NC: segnale XLR IN → XLR OUT diretto
        digitalWrite(RELAY_PIN, LOW);
        Serial.println("[AUDIO] Modalità: MixerPassThrough");
    } else {
        // Relay in posizione NA: DAC ES8388 → XLR OUT
        digitalWrite(RELAY_PIN, HIGH);
        Serial.println("[AUDIO] Modalità: TestTone");
    }
}

AudioMode getAudioMode() {
    return s_currentMode;
}

void audio_generate_test_tone(float frequencyHz, float amplitude) {
    if (s_currentMode != AudioMode::TestTone) {
        Serial.println("[AUDIO] TestTone richiesto ma modalità non attiva");
        return;
    }

    // Accumulatore di fase statico per evitare click tra buffer consecutivi
    static float s_phase = 0.0f;
    const float twoPi   = 2.0f * (float)M_PI;
    const float maxAmp  = 32767.0f * amplitude;
    const float phaseStep = twoPi * frequencyHz / (float)I2S_SAMPLE_RATE;

    for (int i = 0; i < I2S_BUFFER_SIZE; i++) {
        int16_t s = (int16_t)(sinf(s_phase) * maxAmp);
        s_toneBuf[i * 2]     = s;  // canale sinistro
        s_toneBuf[i * 2 + 1] = s;  // canale destro
        s_phase += phaseStep;
        if (s_phase >= twoPi) s_phase -= twoPi;  // mantieni fase in [0, 2π)
    }

    // Scrivi i campioni sul DAC via I2S
    size_t bytesWritten = 0;
    esp_err_t err = i2s_write(I2S_PORT, s_toneBuf, sizeof(s_toneBuf), &bytesWritten, pdMS_TO_TICKS(100));
    if (err != ESP_OK) {
        Serial.printf("[AUDIO] Errore i2s_write: %d\n", err);
    }
}

void audio_fft_process() {
    // Leggi buffer I2S dall'ES8388 ADC
    static int16_t adcBuf[FFT_SIZE * 2];  // stereo interleaved
    size_t bytesRead = 0;
    i2s_read(I2S_PORT, adcBuf, sizeof(adcBuf), &bytesRead, 10 / portTICK_PERIOD_MS);
    if (bytesRead < sizeof(adcBuf)) return;

    // Calcola livelli VU meter (canale L e R separati)
    float sumL = 0.0f, sumR = 0.0f, peakL = 0.0f, peakR = 0.0f;
    for (int i = 0; i < FFT_SIZE; i++) {
        float l = adcBuf[i * 2]     / 32768.0f;
        float r = adcBuf[i * 2 + 1] / 32768.0f;
        sumL += l * l;
        sumR += r * r;
        if (fabsf(l) > peakL) peakL = fabsf(l);
        if (fabsf(r) > peakR) peakR = fabsf(r);
    }
    float rmsL = sqrtf(sumL / FFT_SIZE);
    float rmsR = sqrtf(sumR / FFT_SIZE);
    float peak = fmaxf(peakL, peakR);

    // Aggiorna livelli con thread safety
    portENTER_CRITICAL(&s_fftMux);
    s_levelLeft  = rmsL;
    s_levelRight = rmsR;
    s_peakDb = (peak > 1e-6f) ? 20.0f * log10f(peak) : -60.0f;
    portEXIT_CRITICAL(&s_fftMux);

    if (!s_fftInitialized) return;

    // Applica finestra Hann al canale sinistro e prepara input FFT
    for (int i = 0; i < FFT_SIZE; i++) {
        float sample = (adcBuf[i * 2] / 32768.0f) * s_window[i];
        s_fftInput[i * 2]     = sample;  // parte reale
        s_fftInput[i * 2 + 1] = 0.0f;   // parte immaginaria
    }

    // Esegui FFT reale
    dsps_fft2r_fc32(s_fftInput, FFT_SIZE);
    dsps_bit_rev_fc32(s_fftInput, FFT_SIZE);
    dsps_cplx2reC_fc32(s_fftInput, FFT_SIZE);

    // Calcola 32 bande logaritmiche (20Hz-20kHz)
    FftResult result;
    result.timestamp = millis();
    const float freqRes = (float)I2S_SAMPLE_RATE / FFT_SIZE;
    const float logMin = log10f(20.0f);
    const float logMax = log10f(20000.0f);

    for (int b = 0; b < NUM_BANDS; b++) {
        float freqLow  = powf(10.0f, logMin + (float)b       * (logMax - logMin) / NUM_BANDS);
        float freqHigh = powf(10.0f, logMin + (float)(b + 1) * (logMax - logMin) / NUM_BANDS);
        int binLow  = (int)(freqLow  / freqRes);
        int binHigh = (int)(freqHigh / freqRes);
        if (binHigh >= FFT_SIZE / 2) binHigh = FFT_SIZE / 2 - 1;
        if (binLow  < 1)             binLow  = 1;
        if (binLow  > binHigh)       binLow  = binHigh;

        float maxMag = 0.0f;
        for (int bin = binLow; bin <= binHigh; bin++) {
            float re = s_fftInput[bin * 2];
            float im = s_fftInput[bin * 2 + 1];
            float mag = sqrtf(re * re + im * im);
            if (mag > maxMag) maxMag = mag;
        }
        // Normalizza (0.0-1.0)
        result.bands[b] = fminf(1.0f, maxMag / FFT_NORM_FACTOR);
    }

    portENTER_CRITICAL(&s_fftMux);
    s_fftResult = result;
    portEXIT_CRITICAL(&s_fftMux);
}

float audio_get_peak_db() {
    float val;
    portENTER_CRITICAL(&s_fftMux);
    val = s_peakDb;
    portEXIT_CRITICAL(&s_fftMux);
    return val;
}

void audio_get_channel_levels(float& left, float& right) {
    portENTER_CRITICAL(&s_fftMux);
    left  = s_levelLeft;
    right = s_levelRight;
    portEXIT_CRITICAL(&s_fftMux);
}

const FftResult& audio_get_fft_result() {
    return s_fftResult;
}
