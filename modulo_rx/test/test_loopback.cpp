// test_loopback.cpp – Test audio loopback ES8388 DAC → microfono acustico → ADC
//
// Scopo: verificare la catena audio completa del Modulo RX tramite un ciclo di
// loopback acustico (DAC → speaker → microfono → ADC), misurando il picco FFT
// a 1 kHz e il THD+N.
//
// Prerequisiti:
//   - ES8388 correttamente inizializzato con audio_rx_init()
//   - Speaker o IEM collegato all'uscita DAC
//   - Microfono collegato all'ingresso ADC (o microfono integrato)
//   - esp-dsp per il calcolo FFT
//
// Compilare con:
//   platformio test -e esp32s3 --filter test_loopback

#include <Arduino.h>
#include <Wire.h>
#include <driver/i2s.h>
#include <esp_dsp.h>
#include "../src/config.h"
#include "../src/audio_rx.h"

// ——— Parametri test ———
#define TEST_SAMPLE_RATE   44100
#define TEST_FREQ_HZ       1000
#define TEST_DURATION_MS   500     // durata generazione tono
#define TEST_DELAY_MS      100     // ritardo acustico (propagazione + processing)
#define FFT_SIZE           1024
// THD_N_THRESHOLD: soglia massima accettabile (0.01f = 1%)
#define THD_N_THRESHOLD    0.01f

static float s_sineBuffer[FFT_SIZE];
static float s_captureBuffer[FFT_SIZE];
static float s_fftOut[FFT_SIZE];

// Genera onda sinusoidale nel buffer DAC
static void generate_sine(float* buf, int n, float freq, float sampleRate, float amplitude = 0.5f) {
    for (int i = 0; i < n; i++) {
        buf[i] = amplitude * sinf(2.0f * M_PI * freq * i / sampleRate);
    }
}

// Restituisce il bin FFT corrispondente alla frequenza
static int freq_to_bin(float freq, float sampleRate, int fftSize) {
    return (int)roundf(freq * fftSize / sampleRate);
}

// Calcola potenza (linear) di un singolo bin FFT
static float bin_power(const float* fft, int bin) {
    return fft[bin] * fft[bin];
}

void test_audio_loopback() {
    Serial.println("[TEST] === test_audio_loopback ===");

    // 1. Genera seno 1 kHz nel buffer PCM
    generate_sine(s_sineBuffer, FFT_SIZE, TEST_FREQ_HZ, TEST_SAMPLE_RATE);

    // Converti in int16 interleaved stereo per scrittura I2S
    static int16_t txBuf[FFT_SIZE * 2];
    for (int i = 0; i < FFT_SIZE; i++) {
        int16_t sample = (int16_t)(s_sineBuffer[i] * 32767.0f);
        txBuf[i * 2]     = sample;  // Left
        txBuf[i * 2 + 1] = sample;  // Right
    }

    // 2. Scrivi sul DAC via I2S per TEST_DURATION_MS
    size_t written = 0;
    i2s_write(I2S_NUM_0, txBuf, sizeof(txBuf), &written, pdMS_TO_TICKS(200));
    Serial.printf("[TEST] DAC: scritti %d byte\n", (int)written);

    // 3. Attendi propagazione acustica
    vTaskDelay(pdMS_TO_TICKS(TEST_DELAY_MS));

    // 4. Cattura dal microfono via ADC
    static int32_t rxBuf[FFT_SIZE];
    size_t bytesRead = 0;
    i2s_read(I2S_NUM_0, rxBuf, sizeof(rxBuf), &bytesRead, pdMS_TO_TICKS(200));
    int samplesRead = bytesRead / sizeof(int32_t);
    Serial.printf("[TEST] ADC: letti %d campioni\n", samplesRead);

    if (samplesRead < FFT_SIZE / 2) {
        Serial.println("[TEST] FAIL: campioni insufficienti per analisi FFT");
        return;
    }

    // 5. Normalizza e applica finestra di Hann
    for (int i = 0; i < FFT_SIZE; i++) {
        float sample = (i < samplesRead) ? (rxBuf[i] / (float)INT32_MAX) : 0.0f;
        float hann   = 0.5f * (1.0f - cosf(2.0f * M_PI * i / (FFT_SIZE - 1)));
        s_captureBuffer[i] = sample * hann;
    }

    // 6. Calcola FFT (esp-dsp, in-place, real FFT)
    memcpy(s_fftOut, s_captureBuffer, sizeof(float) * FFT_SIZE);
    dsps_fft2r_init_fc32(nullptr, FFT_SIZE);
    dsps_fft2r_fc32(s_fftOut, FFT_SIZE);
    dsps_bit_rev2r_fc32(s_fftOut, FFT_SIZE);

    // Converti in magnitudes (solo prima metà dello spettro)
    static float magnitude[FFT_SIZE / 2];
    for (int i = 0; i < FFT_SIZE / 2; i++) {
        float re = s_fftOut[i * 2];
        float im = s_fftOut[i * 2 + 1];
        magnitude[i] = sqrtf(re * re + im * im) / (FFT_SIZE / 2);
    }

    // 7. Trova picco FFT vicino a TEST_FREQ_HZ
    int targetBin = freq_to_bin(TEST_FREQ_HZ, TEST_SAMPLE_RATE, FFT_SIZE);
    float peakMag = 0.0f;
    int   peakBin = targetBin;
    for (int b = targetBin - 5; b <= targetBin + 5 && b < FFT_SIZE / 2; b++) {
        if (magnitude[b] > peakMag) { peakMag = magnitude[b]; peakBin = b; }
    }
    float peakFreq = peakBin * TEST_SAMPLE_RATE / (float)FFT_SIZE;
    Serial.printf("[TEST] Picco FFT: bin=%d freq=%.1f Hz mag=%.4f\n", peakBin, peakFreq, peakMag);

    // 8. Calcola THD (potenza armonica 2a e 3a / potenza fondamentale)
    float fundPow = peakMag * peakMag;
    float harmPow = 0.0f;
    for (int h = 2; h <= 5; h++) {
        int hBin = freq_to_bin(TEST_FREQ_HZ * h, TEST_SAMPLE_RATE, FFT_SIZE);
        if (hBin < FFT_SIZE / 2) harmPow += magnitude[hBin] * magnitude[hBin];
    }

    // 9. Stima rumore di fondo (media potenza nelle bande lontane dal segnale)
    float noisePow = 0.0f;
    int   noiseBins = 0;
    for (int b = 10; b < FFT_SIZE / 2; b++) {
        bool nearFundamental = (abs(b - peakBin) < 10);
        bool nearHarmonic = false;
        for (int h = 2; h <= 5; h++) {
            int hBin = freq_to_bin(TEST_FREQ_HZ * h, TEST_SAMPLE_RATE, FFT_SIZE);
            if (abs(b - hBin) < 5) { nearHarmonic = true; break; }
        }
        if (!nearFundamental && !nearHarmonic) {
            noisePow += magnitude[b] * magnitude[b];
            noiseBins++;
        }
    }
    if (noiseBins > 0) noisePow /= noiseBins;

    float thdn = sqrtf((harmPow + noisePow * (FFT_SIZE / 2)) / fundPow);
    Serial.printf("[TEST] THD+N: %.4f%% (threshold: %.2f%%)\n", thdn * 100.0f, THD_N_THRESHOLD * 100.0f);

    // 10. Validazione risultati
    bool freqOk = (fabsf(peakFreq - TEST_FREQ_HZ) <= 5.0f);  // ±5 Hz
    bool thdOk  = (thdn <= THD_N_THRESHOLD);
    bool sigOk  = (peakMag > 0.001f);  // segnale presente (almeno −60 dBFS)

    Serial.printf("[TEST] Freq: %s (%.1f Hz, target %d±5 Hz)\n",
                  freqOk ? "PASS" : "FAIL", peakFreq, TEST_FREQ_HZ);
    Serial.printf("[TEST] THD+N: %s (%.4f%% vs %.2f%%)\n",
                  thdOk ? "PASS" : "FAIL", thdn * 100.0f, THD_N_THRESHOLD * 100.0f);
    Serial.printf("[TEST] Segnale presente: %s (mag=%.4f)\n", sigOk ? "PASS" : "FAIL", peakMag);

    if (freqOk && thdOk && sigOk) {
        Serial.println("[TEST] === LOOPBACK AUDIO: PASS ===");
    } else {
        Serial.println("[TEST] === LOOPBACK AUDIO: FAIL ===");
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);
    Wire.begin(I2C_SDA, I2C_SCL);
    audio_rx_init();
    vTaskDelay(pdMS_TO_TICKS(500));
    test_audio_loopback();
}

void loop() {
    delay(10000);
}
