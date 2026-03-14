#include "audio_mode.h"
#include "config.h"
#if USE_SLAVE_PERIPHERALS
#include "ipc_master.h"
#include "storage.h"
#endif
#include "audio_config.h"
#include "audio_src.h"
#include "drivers/ES8388.h"
#include <driver/i2s.h>
#include <math.h>
#include <esp_dsp.h>

// ======= Configurazione I2S =======
// TODO: adattare i parametri al codec ES8388 specifico (init via I2C se necessario)
#define I2S_PORT        I2S_NUM_0
// I2S_SAMPLE_RATE usa il sample rate di default; aggiornato dinamicamente da audio_config_init()
#define I2S_SAMPLE_RATE AUDIO_SAMPLE_RATE_DEFAULT
#define I2S_BUFFER_SIZE 256
#define FFT_SIZE              512
#define NUM_BANDS             32
// Fattore di normalizzazione FFT: FFT_SIZE/4 perché le magnitudini tipiche dopo
// dsps_cplx2reC_fc32 sono nell'ordine di N/4 per segnali a piena ampiezza
#define FFT_NORM_FACTOR       (FFT_SIZE / 4.0f)

// ES8388 codec driver (controllo I2C per ADC gain)
static ES8388 s_es8388;
static float  s_inputGainDb = -14.0f;  // Default per SC LIVE 4 (+18 dBu → -14 dB att.)

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
#if USE_SLAVE_PERIPHERALS
    if (storage_ipc_slave_available()) ipc_slave_relay(false);
    else
#endif
    {
        pinMode(RELAY_PIN, OUTPUT);
        digitalWrite(RELAY_PIN, LOW);
    }

    // 1. Inizializza ES8388 via I2C per ottenere il sample rate rilevato
    // Wire.begin() è idempotente su ESP32 – sicuro chiamare più volte.
    if (!Wire.isEnabled()) Wire.begin();
    if (s_es8388.begin(&Wire, ES8388_I2C_ADDR)) {
        // 2. Auto-detect sample rate e configura il sistema audio adattivo
        audio_config_init();
        uint32_t detectedSR = g_audioConfig.detectedInputSR;
        if (detectedSR == 0) detectedSR = AUDIO_SAMPLE_RATE_DEFAULT;

        s_es8388.setSampleRate(detectedSR);
        s_es8388.setBitsPerSample(AUDIO_BIT_DEPTH);
        s_es8388.setADCGain(s_inputGainDb);
        s_es8388.startCapture();
        s_es8388.startPlayback();
        Serial.printf("[AUDIO] ES8388 configurato: %u Hz, %d bit, gain %.1f dB\n",
                      detectedSR, AUDIO_BIT_DEPTH, s_inputGainDb);

        // 3. Init SRC se necessario (input SR ≠ 48kHz)
        if (g_audioConfig.srcActive) {
            g_audioSrc.init(detectedSR, AUDIO_PROCESSING_SAMPLE_RATE);
        }
    } else {
        Serial.println("[AUDIO] ES8388 non risponde via I2C – gain software only");
        audio_config_init();  // Usa default 44.1kHz
    }

    // 4. Configurazione I2S con SR rilevato
    uint32_t i2sSR = g_audioConfig.detectedInputSR;
    if (i2sSR == 0) i2sSR = AUDIO_SAMPLE_RATE_DEFAULT;

    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
        .sample_rate = i2sSR,
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

    // I2S configurato come Master: genera BCLK (GPIO12) e WS (GPIO13)
    // BCLK e WS escono verso ES8388 E verso ESP32 #2 (I2S Slave) in parallelo
    i2s_pin_config_t pin_config = {
        .bck_io_num   = I2S_BCLK,
        .ws_io_num    = I2S_WS,
#if USE_SLAVE_ES8388_DOUT
        .data_out_num = I2S_PIN_NO_CHANGE, // DOUT da Slave GPIO4 → ES8388
#else
        .data_out_num = (gpio_num_t)I2S_DOUT,
#endif
        .data_in_num  = I2S_DIN
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

    Serial.printf("[AUDIO] I2S inizializzato @ %u Hz\n", i2sSR);

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
#if USE_SLAVE_PERIPHERALS
        if (storage_ipc_slave_available()) ipc_slave_relay(false);
        else
#endif
        digitalWrite(RELAY_PIN, LOW);
        Serial.println("[AUDIO] Modalità: MixerPassThrough");
    } else {
#if USE_SLAVE_PERIPHERALS
        if (storage_ipc_slave_available()) ipc_slave_relay(true);
        else
#endif
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
    // Usa il sample rate rilevato (o default) per il calcolo del passo di fase
    uint32_t sr = g_audioConfig.detectedInputSR;
    if (sr == 0) sr = AUDIO_SAMPLE_RATE_DEFAULT;
    const float phaseStep = twoPi * frequencyHz / (float)sr;

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
    // Usa il sample rate rilevato per calcolare la risoluzione in frequenza
    uint32_t sr = g_audioConfig.detectedInputSR;
    if (sr == 0) sr = AUDIO_SAMPLE_RATE_DEFAULT;
    const float freqRes = (float)sr / FFT_SIZE;
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

void audio_set_input_gain(float gainDb) {
    gainDb = constrain(gainDb, -96.0f, 24.0f);
    s_inputGainDb = gainDb;
    s_es8388.setADCGain(gainDb);
}

float audio_get_input_gain() {
    return s_inputGainDb;
}
