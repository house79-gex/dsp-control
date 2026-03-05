#include "audio_config.h"
#include "config.h"
#include <Arduino.h>
#include <driver/i2s.h>
#include <algorithm>

// ======= Stato globale audio =======

AudioConfig g_audioConfig = {
    .inputSampleRate   = AudioSampleRate::SR_44100,  // Default: Denon SC LIVE 4
    .processSampleRate = AudioSampleRate::SR_48000,  // Sempre 48kHz per DSP CQ260D
    .enableSRC         = false,
    .inputGainDb       = -14.0f,                     // Default per SC LIVE 4 (+18 dBu)
    .detectedInputSR   = 0,
    .srcActive         = false,
};

// ——— Inizializzazione ———

void audio_config_init() {
    // Tenta auto-detect del sample rate; fallback al default 44.1kHz
    uint32_t detected = audio_detect_sample_rate();
    if (detected > 0) {
        g_audioConfig.detectedInputSR = detected;

        // Seleziona enum corrispondente
        if      (detected <= 32000) g_audioConfig.inputSampleRate = AudioSampleRate::SR_32000;
        else if (detected <= 44100) g_audioConfig.inputSampleRate = AudioSampleRate::SR_44100;
        else if (detected <= 48000) g_audioConfig.inputSampleRate = AudioSampleRate::SR_48000;
        else                        g_audioConfig.inputSampleRate = AudioSampleRate::SR_96000;
    } else {
        // Fallback al default definito in config.h
        g_audioConfig.detectedInputSR = AUDIO_SAMPLE_RATE_DEFAULT;
        g_audioConfig.inputSampleRate = AudioSampleRate::SR_44100;
    }

    // SRC necessario solo se l'input non è già 48kHz
    bool needsSRC = (g_audioConfig.inputSampleRate != AudioSampleRate::SR_48000);
    g_audioConfig.srcActive = needsSRC && g_audioConfig.enableSRC;

    Serial.printf("[AUDIO_CFG] Input SR: %u Hz | Process SR: 48000 Hz | SRC: %s\n",
                  g_audioConfig.detectedInputSR,
                  g_audioConfig.srcActive ? "attivo" : "disattivo");
}

// ——— Auto-detect sample rate da I2S ———

uint32_t audio_detect_sample_rate() {
    // Stima il sample rate leggendo brevemente dall'I2S e misurando il tempo
    // tra sequenze di campioni note. Su hardware reale si legge il MCLK
    // oppure si usa il PLL lock indicator dell'ES8388.
    //
    // Implementazione leggera: misura il tempo per riempire N campioni via DMA.
    // Se I2S non è ancora attivo, ritorna 0 (caller userà il default).

    const size_t PROBE_SAMPLES = 512;
    const size_t PROBE_BYTES   = PROBE_SAMPLES * 2 * sizeof(int16_t);  // stereo 16-bit
    int16_t* probeBuf = (int16_t*)malloc(PROBE_BYTES);
    if (!probeBuf) return 0;

    size_t bytesRead = 0;
    uint32_t t0 = micros();
    esp_err_t err = i2s_read(I2S_NUM_0, probeBuf, PROBE_BYTES, &bytesRead,
                             pdMS_TO_TICKS(50));
    uint32_t elapsed = micros() - t0;
    free(probeBuf);

    if (err != ESP_OK || bytesRead < PROBE_BYTES || elapsed == 0) return 0;

    // Campioni stereo letti / tempo → stima SR
    uint32_t samplesRead = bytesRead / (2 * sizeof(int16_t));
    uint32_t estimatedSR = (uint32_t)((float)samplesRead / (elapsed * 1e-6f));

    // Snap alla frequenza standard più vicina
    const uint32_t standardRates[] = { 32000, 44100, 48000, 96000 };
    uint32_t bestRate = standardRates[0];
    uint32_t bestDiff = UINT32_MAX;
    for (uint32_t rate : standardRates) {
        uint32_t diff = (estimatedSR > rate) ? (estimatedSR - rate) : (rate - estimatedSR);
        if (diff < bestDiff) { bestDiff = diff; bestRate = rate; }
    }

    // Accetta solo se la stima è entro ±10% del valore standard
    if (bestDiff > bestRate / 10) return 0;

    Serial.printf("[AUDIO_CFG] Auto-detect SR: stima=%u Hz → snap=%u Hz\n",
                  estimatedSR, bestRate);
    return bestRate;
}

// ——— Controllo SRC ———

void audio_set_src_enabled(bool enable) {
    g_audioConfig.enableSRC = enable;
    bool needsSRC = (g_audioConfig.inputSampleRate != AudioSampleRate::SR_48000);
    g_audioConfig.srcActive = enable && needsSRC;
    Serial.printf("[AUDIO_CFG] SRC %s\n", g_audioConfig.srcActive ? "attivato" : "disattivato");
}

// ——— Conversione delay ———

// CORREZIONE CRITICA: DSP CQ260D lavora SEMPRE a 48kHz fisso.
// Questa funzione usa AUDIO_PROCESSING_SAMPLE_RATE (48000) definito in config.h,
// indipendente dal sample rate dell'input.
uint16_t audio_ms_to_samples_dsp(float delayMs) {
    // 48kHz = 48 campioni per millisecondo
    const float DSP_SR_KHZ = (float)AUDIO_PROCESSING_SAMPLE_RATE / 1000.0f;
    float samples = delayMs * DSP_SR_KHZ;
    if (samples < 0.0f) samples = 0.0f;
    // Satura a UINT16_MAX (massimo 1365.3 ms @ 48kHz)
    if (samples > 65535.0f) samples = 65535.0f;
    return (uint16_t)samples;
}

// Conversione per buffer locale al sample rate di input corrente
uint32_t audio_ms_to_samples_local(float delayMs) {
    uint32_t sr = g_audioConfig.detectedInputSR;
    if (sr == 0) sr = AUDIO_SAMPLE_RATE_DEFAULT;
    float samplesF = delayMs * (sr / 1000.0f);
    if (samplesF < 0.0f) return 0;
    return (uint32_t)samplesF;
}
