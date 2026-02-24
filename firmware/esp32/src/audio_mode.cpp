#include "audio_mode.h"
#include "config.h"
#include <driver/i2s.h>
#include <math.h>

// ======= Configurazione I2S =======
// TODO: verificare numeri porta e pin con il modulo ES8388 fisico
#define I2S_PORT        I2S_NUM_0
#define I2S_SAMPLE_RATE 48000
#define I2S_BUFFER_SIZE 256

static AudioMode s_currentMode = AudioMode::MixerPassThrough;

// Buffer per generazione tono sinusoidale
static int16_t s_toneBuf[I2S_BUFFER_SIZE * 2];  // stereo interleaved

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

void audio_fft_process_stub() {
    // TODO: implementare analisi FFT con ESP-DSP library
    // Esempio di utilizzo futuro:
    //   float fftBuf[I2S_BUFFER_SIZE];
    //   dsps_fft2r_fc32(fftBuf, I2S_BUFFER_SIZE);
    //   dsps_bit_rev_fc32(fftBuf, I2S_BUFFER_SIZE);
    //   dsps_cplx2reC_fc32(fftBuf, I2S_BUFFER_SIZE);
    // Per ora si limita a leggere e scartare i dati ADC
    static int16_t adcBuf[I2S_BUFFER_SIZE * 2];
    size_t bytesRead = 0;
    i2s_read(I2S_PORT, adcBuf, sizeof(adcBuf), &bytesRead, 10 / portTICK_PERIOD_MS);
    (void)bytesRead;
}
