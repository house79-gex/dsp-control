#pragma once
#include <Arduino.h>

// I2S Slave RX + FFT audio-reactive (6 bande)

void audio_slave_init();
void audio_slave_process_fft();
const float* audio_slave_get_fft_bins();   // Puntatore a array di FFT_NUM_BANDS float
float audio_slave_get_peak_db();
