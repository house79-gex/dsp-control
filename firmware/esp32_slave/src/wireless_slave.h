#pragma once
#include <Arduino.h>

// ESP-NOW wireless TX (casse remote)
// Usa antenna IPEX esterna per range 150-300m

struct WirelessAudioFrame {
    uint8_t  seqNum;         // Numero sequenza (wrapping 0-255)
    float    fftBins[6];     // FFT 6 bande
    float    peakDb;         // Peak dBFS
    uint8_t  dmxFps;         // DMX frame rate corrente
};

void wireless_slave_init();
void wireless_slave_send_frame(const WirelessAudioFrame& frame);
uint32_t wireless_slave_get_tx_count();
int8_t wireless_slave_get_rssi();
