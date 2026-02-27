#include "audio_reactive.h"
#include "dmx512.h"
#include <Arduino.h>
#include <math.h>
#include <string.h>
#include <vector>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

// ======= Variabili di stato =======

static bool    s_enabled         = false;
static uint8_t s_activeScenario  = 0;
static float   s_bandLevels[AUDIO_NUM_BANDS] = { 0 };
static float   s_spectrum[AUDIO_FFT_SIZE / 2] = { 0 };

// Scenari audio-reactive
static std::vector<AudioReactiveScenario> s_scenarios;

// Buffer audio per FFT
static float   s_fftBuffer[AUDIO_FFT_SIZE];
static float   s_fftWindow[AUDIO_FFT_SIZE];  // Finestra di Hann

// Coda audio per comunicazione tra task I2S e task FFT
static QueueHandle_t s_audioQueue = nullptr;
#define AUDIO_QUEUE_SIZE 4

// ======= Scenari predefiniti di fabbrica ======= 

static void init_factory_scenarios() {
    // Frequenze bande standard
    const float bandFreqs[AUDIO_NUM_BANDS][2] = {
        { 20.0f,    60.0f   },  // Sub Bass
        { 60.0f,   250.0f   },  // Bass
        { 250.0f,  500.0f   },  // Low Mid
        { 500.0f,  2000.0f  },  // Mid
        { 2000.0f, 6000.0f  },  // High Mid
        { 6000.0f, 20000.0f },  // Treble
    };

    // Colori default per ogni banda
    const uint8_t bandColors[AUDIO_NUM_BANDS][3] = {
        { 200, 0,   0   },  // Sub Bass → rosso
        { 255, 60,  0   },  // Bass → arancio
        { 200, 150, 0   },  // Low Mid → giallo
        { 0,   200, 100 },  // Mid → verde
        { 0,   100, 255 },  // High Mid → blu
        { 200, 0,   200 },  // Treble → viola
    };

    // ——— Scenario 1: BASS_PULSE ———
    AudioReactiveScenario bassPulse = {};
    bassPulse.id = 1;
    strncpy(bassPulse.name, "BASS_PULSE", sizeof(bassPulse.name) - 1);
    for (int i = 0; i < AUDIO_NUM_BANDS; i++) {
        bassPulse.bands[i].minFreq     = bandFreqs[i][0];
        bassPulse.bands[i].maxFreq     = bandFreqs[i][1];
        bassPulse.bands[i].colorR      = (i == BAND_BASS || i == BAND_SUB_BASS) ? 255 : 0;
        bassPulse.bands[i].colorG      = 0;
        bassPulse.bands[i].colorB      = 0;
        bassPulse.bands[i].sensitivity = (i == BAND_BASS) ? 3.0f : 0.5f;
        bassPulse.bands[i].attackMs    = 5.0f;
        bassPulse.bands[i].decayMs     = 200.0f;
        strncpy(bassPulse.bands[i].blendMode, "override", 15);
    }
    bassPulse.isFactory = true;
    s_scenarios.push_back(bassPulse);

    // ——— Scenario 2: FULL_SPECTRUM ———
    AudioReactiveScenario fullSpectrum = {};
    fullSpectrum.id = 2;
    strncpy(fullSpectrum.name, "FULL_SPECTRUM", sizeof(fullSpectrum.name) - 1);
    for (int i = 0; i < AUDIO_NUM_BANDS; i++) {
        fullSpectrum.bands[i].minFreq     = bandFreqs[i][0];
        fullSpectrum.bands[i].maxFreq     = bandFreqs[i][1];
        fullSpectrum.bands[i].colorR      = bandColors[i][0];
        fullSpectrum.bands[i].colorG      = bandColors[i][1];
        fullSpectrum.bands[i].colorB      = bandColors[i][2];
        fullSpectrum.bands[i].sensitivity = 2.0f;
        fullSpectrum.bands[i].attackMs    = 10.0f;
        fullSpectrum.bands[i].decayMs     = 150.0f;
        strncpy(fullSpectrum.bands[i].blendMode, "add", 15);
    }
    fullSpectrum.isFactory = true;
    s_scenarios.push_back(fullSpectrum);

    // ——— Scenario 3: KICK_FLASH ———
    AudioReactiveScenario kickFlash = {};
    kickFlash.id = 3;
    strncpy(kickFlash.name, "KICK_FLASH", sizeof(kickFlash.name) - 1);
    for (int i = 0; i < AUDIO_NUM_BANDS; i++) {
        kickFlash.bands[i].minFreq     = bandFreqs[i][0];
        kickFlash.bands[i].maxFreq     = bandFreqs[i][1];
        kickFlash.bands[i].colorR      = 255;
        kickFlash.bands[i].colorG      = 255;
        kickFlash.bands[i].colorB      = 255;
        kickFlash.bands[i].sensitivity = (i == BAND_BASS) ? 5.0f : 0.1f;
        kickFlash.bands[i].attackMs    = 2.0f;
        kickFlash.bands[i].decayMs     = 80.0f;
        strncpy(kickFlash.bands[i].blendMode, "override", 15);
    }
    kickFlash.isFactory = true;
    s_scenarios.push_back(kickFlash);

    // ——— Scenario 4: DJ_PARTY ———
    AudioReactiveScenario djParty = {};
    djParty.id = 4;
    strncpy(djParty.name, "DJ_PARTY", sizeof(djParty.name) - 1);
    for (int i = 0; i < AUDIO_NUM_BANDS; i++) {
        djParty.bands[i].minFreq     = bandFreqs[i][0];
        djParty.bands[i].maxFreq     = bandFreqs[i][1];
        djParty.bands[i].attackMs    = 5.0f;
        djParty.bands[i].decayMs     = 100.0f;
        djParty.bands[i].sensitivity = 2.5f;
        strncpy(djParty.bands[i].blendMode, "add", 15);
    }
    // Sub bass + bass → rosso
    djParty.bands[BAND_SUB_BASS].colorR = 255; djParty.bands[BAND_SUB_BASS].colorG = 0;   djParty.bands[BAND_SUB_BASS].colorB = 0;
    djParty.bands[BAND_BASS].colorR     = 255; djParty.bands[BAND_BASS].colorG     = 30;  djParty.bands[BAND_BASS].colorB     = 0;
    // Mid → blu
    djParty.bands[BAND_MID].colorR      = 0;   djParty.bands[BAND_MID].colorG      = 50;  djParty.bands[BAND_MID].colorB      = 255;
    // Alti → bianco
    djParty.bands[BAND_HIGH_MID].colorR = 255; djParty.bands[BAND_HIGH_MID].colorG = 255; djParty.bands[BAND_HIGH_MID].colorB = 255;
    djParty.bands[BAND_TREBLE].colorR   = 255; djParty.bands[BAND_TREBLE].colorG   = 255; djParty.bands[BAND_TREBLE].colorB   = 255;
    djParty.isFactory = true;
    s_scenarios.push_back(djParty);

    // ——— Scenario 5: CONCERT_SYNC ———
    AudioReactiveScenario concertSync = {};
    concertSync.id = 5;
    strncpy(concertSync.name, "CONCERT_SYNC", sizeof(concertSync.name) - 1);
    for (int i = 0; i < AUDIO_NUM_BANDS; i++) {
        concertSync.bands[i].minFreq     = bandFreqs[i][0];
        concertSync.bands[i].maxFreq     = bandFreqs[i][1];
        concertSync.bands[i].sensitivity = 2.0f;
        concertSync.bands[i].attackMs    = 8.0f;
        concertSync.bands[i].decayMs     = 200.0f;
        strncpy(concertSync.bands[i].blendMode, "override", 15);
    }
    concertSync.bands[BAND_SUB_BASS].colorR = 200; concertSync.bands[BAND_SUB_BASS].colorG = 0;  concertSync.bands[BAND_SUB_BASS].colorB = 0;
    concertSync.bands[BAND_BASS].colorR     = 200; concertSync.bands[BAND_BASS].colorG     = 0;  concertSync.bands[BAND_BASS].colorB     = 0;
    concertSync.bands[BAND_LOW_MID].colorR  = 150; concertSync.bands[BAND_LOW_MID].colorG  = 0;  concertSync.bands[BAND_LOW_MID].colorB  = 200;
    concertSync.bands[BAND_MID].colorR      = 100; concertSync.bands[BAND_MID].colorG      = 0;  concertSync.bands[BAND_MID].colorB      = 180;
    concertSync.bands[BAND_HIGH_MID].colorR = 0;   concertSync.bands[BAND_HIGH_MID].colorG = 200; concertSync.bands[BAND_HIGH_MID].colorB = 200;
    concertSync.bands[BAND_TREBLE].colorR   = 255; concertSync.bands[BAND_TREBLE].colorG   = 255; concertSync.bands[BAND_TREBLE].colorB   = 255;
    concertSync.isFactory = true;
    s_scenarios.push_back(concertSync);

    // ——— Scenario 6: AMBIENT_FLOW ———
    AudioReactiveScenario ambientFlow = {};
    ambientFlow.id = 6;
    strncpy(ambientFlow.name, "AMBIENT_FLOW", sizeof(ambientFlow.name) - 1);
    for (int i = 0; i < AUDIO_NUM_BANDS; i++) {
        ambientFlow.bands[i].minFreq     = bandFreqs[i][0];
        ambientFlow.bands[i].maxFreq     = bandFreqs[i][1];
        ambientFlow.bands[i].colorR      = bandColors[i][0];
        ambientFlow.bands[i].colorG      = bandColors[i][1];
        ambientFlow.bands[i].colorB      = bandColors[i][2];
        ambientFlow.bands[i].sensitivity = 1.0f;
        ambientFlow.bands[i].attackMs    = 50.0f;   // Reazione lenta
        ambientFlow.bands[i].decayMs     = 500.0f;  // Decay lungo
        strncpy(ambientFlow.bands[i].blendMode, "add", 15);
    }
    ambientFlow.isFactory = true;
    s_scenarios.push_back(ambientFlow);

    // ——— Scenario 7: VOCAL_WASH ———
    AudioReactiveScenario vocalWash = {};
    vocalWash.id = 7;
    strncpy(vocalWash.name, "VOCAL_WASH", sizeof(vocalWash.name) - 1);
    for (int i = 0; i < AUDIO_NUM_BANDS; i++) {
        vocalWash.bands[i].minFreq     = bandFreqs[i][0];
        vocalWash.bands[i].maxFreq     = bandFreqs[i][1];
        vocalWash.bands[i].sensitivity = (i == BAND_MID) ? 3.0f : 0.2f;
        vocalWash.bands[i].attackMs    = 20.0f;
        vocalWash.bands[i].decayMs     = 300.0f;
        strncpy(vocalWash.bands[i].blendMode, "multiply", 15);
    }
    vocalWash.bands[BAND_MID].colorR = 255; vocalWash.bands[BAND_MID].colorG = 200; vocalWash.bands[BAND_MID].colorB = 100;
    vocalWash.isFactory = true;
    s_scenarios.push_back(vocalWash);

    // ——— Scenario 8: BASS_COLOR_CHASE ———
    AudioReactiveScenario bassChase = {};
    bassChase.id = 8;
    strncpy(bassChase.name, "BASS_COLOR_CHASE", sizeof(bassChase.name) - 1);
    for (int i = 0; i < AUDIO_NUM_BANDS; i++) {
        bassChase.bands[i].minFreq     = bandFreqs[i][0];
        bassChase.bands[i].maxFreq     = bandFreqs[i][1];
        bassChase.bands[i].sensitivity = (i <= BAND_BASS) ? 4.0f : 0.1f;
        bassChase.bands[i].attackMs    = 3.0f;
        bassChase.bands[i].decayMs     = 120.0f;
        strncpy(bassChase.bands[i].blendMode, "override", 15);
    }
    bassChase.bands[BAND_BASS].colorR = 255; bassChase.bands[BAND_BASS].colorG = 0; bassChase.bands[BAND_BASS].colorB = 100;
    bassChase.isFactory = true;
    s_scenarios.push_back(bassChase);
}

// ======= FFT semplificata (algoritmo DFT per bande) =======
// Nota: per produzione usare dsps_fft2r_fc32 da ESP-DSP library
// Questa implementazione usa DFT a banda limitata per efficienza su ESP32

static float compute_band_energy(const float* samples, size_t numSamples,
                                  float minFreq, float maxFreq, float sampleRate) {
    // Calcola energia in una banda di frequenza usando DFT selettiva
    // Per performance su ESP32 usiamo solo le frequenze di interesse
    float energy = 0.0f;
    int minBin = (int)(minFreq * (float)numSamples / sampleRate);
    int maxBin = (int)(maxFreq * (float)numSamples / sampleRate);
    if (minBin < 1) minBin = 1;
    if (maxBin > (int)(numSamples / 2)) maxBin = numSamples / 2;

    // Accumula energia (somma magnitudini)
    for (int k = minBin; k <= maxBin; k++) {
        float cosSum = 0.0f, sinSum = 0.0f;
        // DFT per frequenza k (campiona ogni 4 per efficienza)
        for (size_t n = 0; n < numSamples; n += 4) {
            float angle = -2.0f * M_PI * k * n / (float)numSamples;
            cosSum += samples[n] * cosf(angle);
            sinSum += samples[n] * sinf(angle);
        }
        energy += sqrtf(cosSum * cosSum + sinSum * sinSum);
    }
    return energy / (float)(maxBin - minBin + 1);
}

// ======= Inizializzazione =======

void audio_reactive_init() {
    // Crea coda audio per comunicazione tra task
    s_audioQueue = xQueueCreate(AUDIO_QUEUE_SIZE, sizeof(int16_t*));

    // Precalcola finestra di Hann per FFT
    for (int i = 0; i < AUDIO_FFT_SIZE; i++) {
        s_fftWindow[i] = 0.5f * (1.0f - cosf(2.0f * M_PI * i / (float)(AUDIO_FFT_SIZE - 1)));
    }

    // Inizializza scenari di fabbrica
    init_factory_scenarios();

    Serial.println("[AUDIO_REACT] Modulo audio-reactive inizializzato");
}

void audio_reactive_enable(bool enabled, uint8_t scenarioId) {
    s_enabled = enabled;
    if (enabled && scenarioId > 0) {
        s_activeScenario = scenarioId;
    }
    Serial.printf("[AUDIO_REACT] %s, scenario=%d\n",
                  enabled ? "ABILITATO" : "DISABILITATO", s_activeScenario);
}

bool audio_reactive_is_enabled() {
    return s_enabled;
}

void audio_reactive_set_scenario(uint8_t scenarioId) {
    s_activeScenario = scenarioId;
}

uint8_t audio_reactive_get_scenario() {
    return s_activeScenario;
}

void audio_reactive_process(const int16_t* audioData, size_t numSamples) {
    if (!s_enabled || s_activeScenario == 0) return;

    AudioReactiveScenario* scenario = audio_reactive_get_scenario_data(s_activeScenario);
    if (!scenario) return;

    // Limita numero campioni al buffer FFT
    size_t n = numSamples < AUDIO_FFT_SIZE ? numSamples : AUDIO_FFT_SIZE;

    // Converti PCM 16-bit in float normalizzato + applica finestra di Hann
    for (size_t i = 0; i < n; i++) {
        s_fftBuffer[i] = (float)audioData[i] / 32768.0f * s_fftWindow[i];
    }

    // Calcola energia per ogni banda di frequenza
    for (int band = 0; band < AUDIO_NUM_BANDS; band++) {
        BandConfig& bc = scenario->bands[band];
        float energy = compute_band_energy(s_fftBuffer, n, bc.minFreq, bc.maxFreq,
                                            (float)AUDIO_SAMPLE_RATE);
        // Scala con sensibilità
        energy *= bc.sensitivity;
        energy = fminf(energy, 1.0f);

        // Aggiorna livello grezzo
        bc.currentLevel = energy;

        // Smoothing attack/decay
        float dt = 25.0f; // 25ms per frame (40Hz)
        float attackCoeff  = expf(-dt / bc.attackMs);
        float decayCoeff   = expf(-dt / bc.decayMs);
        float coeff = (energy > bc.smoothedLevel) ? attackCoeff : decayCoeff;
        bc.smoothedLevel = bc.smoothedLevel * coeff + energy * (1.0f - coeff);
        s_bandLevels[band] = bc.smoothedLevel;
    }

    // Applica effetti DMX in base ai livelli per banda
    auto fixtures = dmx_get_all_fixtures();
    if (fixtures.empty()) return;

    for (int band = 0; band < AUDIO_NUM_BANDS; band++) {
        BandConfig& bc = scenario->bands[band];
        float level = bc.smoothedLevel;
        if (level < 0.02f) continue;  // Soglia minima per evitare rumore

        uint8_t r = (uint8_t)(bc.colorR * level);
        uint8_t g = (uint8_t)(bc.colorG * level);
        uint8_t blue = (uint8_t)(bc.colorB * level);

        // Applica a fixture target o a tutte se non specificato
        if (bc.numTargets > 0) {
            for (uint8_t i = 0; i < bc.numTargets; i++) {
                if (strcmp(bc.blendMode, "override") == 0) {
                    dmx_set_fixture_color(bc.targetFixtureIds[i], r, g, blue);
                } else if (strcmp(bc.blendMode, "add") == 0) {
                    DmxFixture* fx = dmx_get_fixture(bc.targetFixtureIds[i]);
                    if (fx && fx->numChannels >= 3) {
                        uint8_t nr = (uint8_t)fminf(fx->channels[0] + r, 255.0f);
                        uint8_t ng = (uint8_t)fminf(fx->channels[1] + g, 255.0f);
                        uint8_t newBlue = (uint8_t)fminf(fx->channels[2] + blue, 255.0f);
                        dmx_set_fixture_color(bc.targetFixtureIds[i], nr, ng, newBlue);
                    }
                }
            }
        } else {
            // Applica a tutte le fixture (distribuite per banda)
            size_t numFx = fixtures.size();
            size_t bandSize = (numFx + AUDIO_NUM_BANDS - 1) / AUDIO_NUM_BANDS;
            size_t startFx = band * bandSize;
            size_t endFx = fminf((float)(startFx + bandSize), (float)numFx);
            for (size_t i = startFx; i < endFx; i++) {
                dmx_set_fixture_color(fixtures[i]->id, r, g, blue);
            }
        }
    }
}

const float* audio_reactive_get_band_levels() {
    return s_bandLevels;
}

const float* audio_reactive_get_spectrum() {
    return s_spectrum;
}

bool audio_reactive_save_scenario(const AudioReactiveScenario& scenario) {
    for (auto& s : s_scenarios) {
        if (s.id == scenario.id && !s.isFactory) {
            s = scenario;
            return true;
        }
    }
    s_scenarios.push_back(scenario);
    return true;
}

bool audio_reactive_delete_scenario(uint8_t scenarioId) {
    for (auto it = s_scenarios.begin(); it != s_scenarios.end(); ++it) {
        if (it->id == scenarioId && !it->isFactory) {
            s_scenarios.erase(it);
            return true;
        }
    }
    return false;
}

AudioReactiveScenario* audio_reactive_get_scenario_data(uint8_t scenarioId) {
    for (auto& s : s_scenarios) {
        if (s.id == scenarioId) return &s;
    }
    return nullptr;
}

std::vector<AudioReactiveScenario*> audio_reactive_get_all_scenarios() {
    std::vector<AudioReactiveScenario*> result;
    for (auto& s : s_scenarios) result.push_back(&s);
    return result;
}

// Task FreeRTOS per elaborazione FFT su Core 0
void audio_fft_task(void* param) {
    Serial.println("[AUDIO_REACT] Task FFT avviato su Core 0");
    const TickType_t period = pdMS_TO_TICKS(25); // 40 Hz
    TickType_t lastWake = xTaskGetTickCount();

    for (;;) {
        // Il task processa l'audio se disponibile in coda
        // In produzione qui si leggerebbe dal buffer I2S dell'ES8388
        // Simulazione: genera silenzio se non c'è audio reale
        if (s_enabled) {
            // Nota: la lettura reale dell'ADC avviene in audio_mode.cpp
            // Questo task elabora i dati già campionati
        }
        vTaskDelayUntil(&lastWake, period);
    }
}
