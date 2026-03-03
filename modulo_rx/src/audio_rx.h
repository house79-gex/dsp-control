#pragma once
#include <Arduino.h>
#include <cstdint>

// ======= Modulo RX – Playback Audio ES8388 =======
// Riceve campioni audio wireless e li invia al codec ES8388 via I2S DAC

// Inizializza I2S + ES8388 in modalità playback
void audio_rx_init();

// Aggiunge campioni al buffer di playback
// samples: campioni int16 stereo (o mono a seconda di audioMode)
// numSamples: numero campioni (non byte)
// audioMode: 0=Mono, 1=Stereo, 2=Sub (mono da basse frequenze)
void audio_rx_push_samples(const int16_t* samples, uint8_t numSamples, uint8_t audioMode);

// Statistiche buffer
uint32_t audio_rx_get_underruns();
uint32_t audio_rx_get_frames_played();

// Task FreeRTOS playback audio (da registrare in main)
void audio_rx_task(void* param);
