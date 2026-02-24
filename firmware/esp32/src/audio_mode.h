#pragma once
#include <Arduino.h>

enum class AudioMode {
    MixerPassThrough,  // Relay NC: XLR IN → XLR OUT diretto (ES8388 ADC attivo in ascolto)
    TestTone           // Relay NA: DAC ES8388 → XLR OUT
};

void audio_init();
void setAudioMode(AudioMode mode);
AudioMode getAudioMode();
void audio_generate_test_tone(float frequencyHz = 1000.0f, float amplitude = 0.5f);
// TODO: stub FFT - da implementare con dsps_fft2r_fc32 (ESP-DSP library)
void audio_fft_process_stub();
