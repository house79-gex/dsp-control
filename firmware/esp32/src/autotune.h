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
