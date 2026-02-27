#pragma once
#include <Arduino.h>

enum class AudioMode {
    MixerPassThrough,  // Relay NC: XLR IN → XLR OUT diretto (ES8388 ADC attivo in ascolto)
    TestTone           // Relay NA: DAC ES8388 → XLR OUT
};

// Struttura risultato FFT (32 bande logaritmiche 20Hz-20kHz)
struct FftResult {
    float bands[32];       // Livelli per banda (dB, normalizzati 0.0-1.0)
    uint32_t timestamp;    // Timestamp millis()
};

void audio_init();
void setAudioMode(AudioMode mode);
AudioMode getAudioMode();
void audio_generate_test_tone(float frequencyHz = 1000.0f, float amplitude = 0.5f);

// FFT reale (sostituisce stub)
void audio_fft_process();

// VU meter
float audio_get_peak_db();
void audio_get_channel_levels(float& left, float& right);
const FftResult& audio_get_fft_result();
