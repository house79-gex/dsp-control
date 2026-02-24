#pragma once
#include <string>
#include <cstdint>

// Modulo stub per integrazione DSP CQ260D via RS-485
// TODO: implementare protocollo reale CQ260D quando disponibile

void dsp_init();
void dsp_apply_preset(const std::string& presetName);
void dsp_set_speaker_gain(uint8_t id, float db);
void dsp_set_speaker_hpf(uint8_t id, float freqHz);
void dsp_set_speaker_lpf(uint8_t id, float freqHz);
