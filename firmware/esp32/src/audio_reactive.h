#pragma once
#include <Arduino.h>
#include <cstdint>
#include <vector>
#include <string>

// ======= Sistema Audio-Reactive DMX =======
// Analisi FFT real-time dal segnale ES8388 ADC → controllo fixture DMX
// Suddivisione in 6 bande di frequenza configurabili

#define AUDIO_FFT_SIZE      512     // Dimensione FFT (potenza di 2)
#define AUDIO_SAMPLE_RATE   48000   // Sample rate ES8388
#define AUDIO_NUM_BANDS     6       // Numero bande frequenza

// ——— Bande di frequenza standard ———
#define BAND_SUB_BASS   0   // 20-60 Hz   → colori caldi/rossi
#define BAND_BASS       1   // 60-250 Hz  → pulsazione colore principale
#define BAND_LOW_MID    2   // 250-500 Hz → secondo layer colore
#define BAND_MID        3   // 500-2kHz   → effetti secondari
#define BAND_HIGH_MID   4   // 2k-6kHz    → flash/strobo
#define BAND_TREBLE     5   // 6k-20kHz   → sparkle/twinkle

// ——— Struttura configurazione banda ———
struct BandConfig {
    float    minFreq;             // Frequenza minima banda (Hz)
    float    maxFreq;             // Frequenza massima banda (Hz)
    uint8_t  targetFixtureIds[32]; // IDs fixture controllate da questa banda
    uint8_t  numTargets;          // Numero fixture target
    uint8_t  colorR;              // Colore base R
    uint8_t  colorG;              // Colore base G
    uint8_t  colorB;              // Colore base B
    float    sensitivity;         // Sensibilità/gain (0.1-10.0)
    float    attackMs;            // Velocità attack (ms)
    float    decayMs;             // Velocità decay (ms)
    char     blendMode[16];       // Modalità blend: "override", "add", "multiply"
    float    currentLevel;        // Livello corrente (0.0-1.0)
    float    smoothedLevel;       // Livello smoothed (attack/decay)
};

// ——— Struttura scenario audio-reactive ———
struct AudioReactiveScenario {
    uint8_t  id;
    char     name[32];
    BandConfig bands[AUDIO_NUM_BANDS];
    bool     isFactory;
};

// ——— API pubblica ———

// Inizializza il modulo audio-reactive
void audio_reactive_init();

// Abilita/disabilita audio-reactive
void audio_reactive_enable(bool enabled, uint8_t scenarioId = 0);
bool audio_reactive_is_enabled();

// Imposta lo scenario attivo
void audio_reactive_set_scenario(uint8_t scenarioId);
uint8_t audio_reactive_get_scenario();

// Aggiorna l'analisi FFT e applica effetti DMX (chiamare periodicamente)
// audioData: campioni audio PCM 16-bit, numSamples: numero campioni
void audio_reactive_process(const int16_t* audioData, size_t numSamples);

// Restituisce i livelli per banda (array di AUDIO_NUM_BANDS float 0.0-1.0)
const float* audio_reactive_get_band_levels();

// Restituisce i dati spettro FFT raw (AUDIO_FFT_SIZE/2 bin)
const float* audio_reactive_get_spectrum();

// Salva/recupera scenario
bool audio_reactive_save_scenario(const AudioReactiveScenario& scenario);
bool audio_reactive_delete_scenario(uint8_t scenarioId);
AudioReactiveScenario* audio_reactive_get_scenario_data(uint8_t scenarioId);
std::vector<AudioReactiveScenario*> audio_reactive_get_all_scenarios();

// Task FreeRTOS per elaborazione FFT (su Core 0)
void audio_fft_task(void* param);
