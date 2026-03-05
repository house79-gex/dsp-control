#pragma once
#include <Arduino.h>

// ======= Sistema Audio Adattivo Multi-Sample-Rate =======
// Gestisce la configurazione audio dinamica per sorgenti con diversi sample rate.
// Il DSP CQ260D lavora SEMPRE a 48kHz fisso (da datasheet).
// L'input può variare: 32k, 44.1k, 48k, 96k Hz.

enum class AudioSampleRate : uint32_t {
    SR_32000 = 32000,
    SR_44100 = 44100,
    SR_48000 = 48000,
    SR_96000 = 96000,
    SR_AUTO  = 0     // Auto-detect via I2S timing
};

struct AudioConfig {
    AudioSampleRate inputSampleRate;     // Sample rate sorgente rilevata
    AudioSampleRate processSampleRate;   // Sempre SR_48000 per DSP CQ260D
    bool            enableSRC;           // Abilita Sample Rate Converter
    float           inputGainDb;         // Gain ingresso ES8388 (dB)
    uint32_t        detectedInputSR;     // SR rilevato in Hz (0 se non rilevato)
    bool            srcActive;           // SRC attualmente in uso
};

// Istanza globale configurazione audio
extern AudioConfig g_audioConfig;

// Inizializza il sistema audio adattivo (chiama all'avvio dopo Wire.begin())
void audio_config_init();

// Rileva sample rate input analizzando timing I2S (ritorna SR in Hz, 0 se fallisce)
uint32_t audio_detect_sample_rate();

// Abilita/disabilita Sample Rate Converter
void audio_set_src_enabled(bool enable);

// ——— Conversione delay adattiva ———

// Per DSP CQ260D: SEMPRE 48kHz (da datasheet, valore fisso hardware)
// Ritorna campioni con saturazione a UINT16_MAX
uint16_t audio_ms_to_samples_dsp(float delayMs);

// Per buffer locale: usa il sample rate di input corrente
// Ritorna campioni come uint32_t per evitare overflow con SR elevati
uint32_t audio_ms_to_samples_local(float delayMs);
