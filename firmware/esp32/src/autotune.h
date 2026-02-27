#pragma once
#include <Arduino.h>
#include <cstdint>
#include <vector>

// ======= Sistema AutoTune – Calibrazione automatica impianto PA =======
// Genera segnali di test, analizza risposta in frequenza tramite ES8388 ADC,
// calcola e applica correzioni EQ e delay di allineamento al DSP CQ260D

// Stato della procedura autotune
enum class AutotuneState {
    Idle,           // Inattivo
    Muting,         // Mute di tutte le casse tranne quella in test
    Generating,     // Generazione sweep/noise
    Capturing,      // Cattura audio dal microfono
    Analyzing,      // Analisi FFT risposta in frequenza
    ApplyingEQ,     // Applicazione correzione EQ al DSP
    Complete,       // Procedura completata
    Error           // Errore durante la procedura
};

// Modalità autotune
enum class AutotuneMode {
    Single,         // Calibrazione singola cassa
    All             // Calibrazione impianto completo (tutte le casse)
};

// Sorgente microfono per autotune
enum class AutoTuneSource {
    LOCAL_MIC,           // Microfono locale (ES8388 ADC)
    REMOTE_SMARTPHONE,   // Microfono smartphone via WiFi
    USB_MEASUREMENT_MIC  // Microfono USB di misura calibrato (es. Dayton iMM-6C)
};

// Curva di calibrazione microfono di misura (max 128 punti)
struct MicCalibration {
    float freqHz[128];       // Frequenze
    float correctionDb[128]; // Correzione in dB per ogni frequenza
    uint8_t numPoints;       // Numero di punti nella curva
    char micName[32];        // Nome microfono (es. "iMM-6C SN12345")
    bool valid;              // Calibrazione caricata e valida
};

// Stato autotune remoto (smartphone)
struct RemoteFftData {
    float bands[64];      // Bande FFT inviate dallo smartphone
    uint32_t timestamp;   // Timestamp dati
    bool valid;           // Dati validi
};

// Risultato autotune per una cassa
struct AutotuneResult {
    uint8_t  deviceId;
    float    responseFreq[64];      // Frequenze di campionamento risposta
    float    responseMagnitude[64]; // Magnitudine risposta (dB)
    float    correctionFreq[28];    // Frequenze EQ di correzione
    float    correctionGain[28];    // Gain EQ di correzione (dB)
    float    estimatedDelayMs;      // Delay stimato (ms) per allineamento
    bool     valid;                 // Risultato valido
};

// Struttura stato procedura
struct AutotuneStatus {
    AutotuneState    state;
    AutotuneMode     mode;
    uint8_t          targetDeviceId;     // ID cassa in test (0=tutte)
    uint8_t          currentDeviceIdx;   // Indice cassa corrente (per mode All)
    uint8_t          totalDevices;       // Totale casse da calibrare
    float            progress;           // Progresso 0.0-1.0
    char             statusMessage[128]; // Messaggio stato per UI
    AutotuneResult   results[32];        // Risultati per cassa
    uint8_t          numResults;         // Numero risultati validi
};

// ——— API pubblica ———

// Inizializza il modulo autotune
void autotune_init();

// Avvia la procedura autotune
// mode: Single o All
// targetId: ID cassa (solo per Single)
bool autotune_start(AutotuneMode mode, uint8_t targetId = 0);

// Applica i risultati autotune al DSP
bool autotune_apply_results();

// Annulla la procedura in corso
void autotune_cancel();

// Restituisce lo stato corrente della procedura
const AutotuneStatus* autotune_get_status();

// Tick periodico – avanzamento state machine (chiamare ogni ~100ms)
void autotune_tick();

// AutoTune remoto (microfono smartphone o USB di misura)
bool autotune_start_remote(uint8_t targetId = 0);
bool autotune_start_usb_mic(uint8_t targetId = 0);
void autotune_upload_fft(const float* bands, uint8_t numBands);
bool autotune_is_remote_mode();
const RemoteFftData* autotune_get_remote_fft();

// API calibrazione microfono di misura
void autotune_load_calibration(const MicCalibration* cal);
const MicCalibration* autotune_get_calibration();
bool autotune_has_calibration();
