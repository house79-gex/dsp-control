#include "dsp_control.h"
#include <Arduino.h>

// ======= STUB DSP CQ260D =======
// Tutti i metodi logano l'intenzione ma non inviano dati reali.
// Il protocollo seriale CQ260D dovrà essere implementato quando
// la documentazione tecnica del modulo sarà disponibile.

void dsp_init() {
    // TODO: inizializzare comunicazione RS-485 con CQ260D
    // Probabilmente richiede sequenza di handshake proprietaria
    Serial.println("[DSP] Inizializzazione CQ260D (STUB)");
    Serial.println("[DSP] TODO: inviare sequenza init al CQ260D via RS-485");
}

void dsp_apply_preset(const std::string& presetName) {
    // TODO: tradurre nome preset in comandi binari CQ260D
    // Preset disponibili: "2WAY_STD", "3WAY_STD", "SUB_STD", "MONITOR", "OUTDOOR"
    Serial.printf("[DSP] Applicazione preset '%s' (STUB)\n", presetName.c_str());
    Serial.println("[DSP] TODO: inviare preset via RS-485 broadcast o unicast");
}

void dsp_set_speaker_gain(uint8_t id, float db) {
    // TODO: costruire pacchetto RS-485 per gain sul canale specificato
    // Valore tipico: da -60.0 dB a +12.0 dB
    Serial.printf("[DSP] Set gain cassa ID=%d → %.1f dB (STUB)\n", id, db);
}

void dsp_set_speaker_hpf(uint8_t id, float freqHz) {
    // TODO: impostare frequenza High-Pass Filter sul CQ260D
    // Range tipico: 20 Hz – 2000 Hz, pendenza configurabile (12/24 dB/oct)
    Serial.printf("[DSP] Set HPF cassa ID=%d → %.0f Hz (STUB)\n", id, freqHz);
}

void dsp_set_speaker_lpf(uint8_t id, float freqHz) {
    // TODO: impostare frequenza Low-Pass Filter sul CQ260D
    // Range tipico: 200 Hz – 20000 Hz, pendenza configurabile (12/24 dB/oct)
    Serial.printf("[DSP] Set LPF cassa ID=%d → %.0f Hz (STUB)\n", id, freqHz);
}
