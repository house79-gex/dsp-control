#include "autotune.h"
#include "dsp_control.h"
#include "audio_mode.h"
#include "rs485.h"
#include <Arduino.h>
#include <math.h>
#include <string.h>

// ======= Variabili di stato =======

static AutotuneStatus s_status = {};
static bool s_initialized = false;

// Frequenze di analisi (octave spacing da 20Hz a 20kHz, 64 punti)
static const float ANALYSIS_FREQS[64] = {
    20.0f,    22.4f,   25.1f,   28.2f,   31.6f,   35.5f,   39.8f,   44.7f,
    50.1f,    56.2f,   63.1f,   70.8f,   79.4f,   89.1f,  100.0f,  112.2f,
   125.9f,   141.3f,  158.5f,  177.8f,  199.5f,  223.9f,  251.2f,  281.8f,
   316.2f,   354.8f,  398.1f,  446.7f,  501.2f,  562.3f,  630.9f,  707.9f,
   794.3f,   891.3f, 1000.0f, 1122.0f, 1258.9f, 1412.5f, 1584.9f, 1778.3f,
  1995.3f,  2238.7f, 2511.9f, 2818.4f, 3162.3f, 3548.1f, 3981.1f, 4466.8f,
  5011.9f,  5623.4f, 6309.6f, 7079.5f, 7943.3f, 8912.5f, 10000.0f,11220.2f,
 12589.3f, 14125.4f,15848.9f,17782.8f,19952.6f, 20000.0f, 20000.0f, 20000.0f
};

// ======= Funzioni interne =======

static void set_status_message(const char* msg) {
    strncpy(s_status.statusMessage, msg, sizeof(s_status.statusMessage) - 1);
    Serial.printf("[AUTOTUNE] %s\n", msg);
}

// Genera risposta di test simulata con caratteristiche tipiche di una cassa PA
// In produzione questa funzione viene sostituita dalla lettura reale ADC ES8388
static void simulate_frequency_response(AutotuneResult* result) {
    for (int i = 0; i < 64; i++) {
        float freq = ANALYSIS_FREQS[i];
        float magnitude = 0.0f;

        // Simula risposta tipica 2-vie PA: rolloff grave < 80Hz, picco 1kHz
        if (freq < 80.0f) {
            // HPF naturale a 80Hz (-12dB/oct sotto)
            magnitude = 20.0f * log10f(freq / 80.0f) * 2.0f;
        } else if (freq > 12000.0f) {
            // Rolloff HF sopra 12kHz
            magnitude = -3.0f * log10f(freq / 12000.0f);
        } else {
            magnitude = 0.0f;
        }

        // Aggiungi leggera variazione casuale ±2dB (realistica)
        float variation = ((float)esp_random() / (float)UINT32_MAX) * 4.0f - 2.0f;
        magnitude += variation;

        result->responseFreq[i]      = freq;
        result->responseMagnitude[i] = magnitude;
    }
}

// Calcola la correzione EQ ottimale (EQ inverso della risposta)
static void calculate_eq_correction(AutotuneResult* result) {
    // Campiona la risposta a 28 frequenze per le bande EQ uscita
    const float eqFreqs[28] = {
        63.0f, 80.0f, 100.0f, 125.0f, 160.0f, 200.0f, 250.0f,
        315.0f, 400.0f, 500.0f, 630.0f, 800.0f, 1000.0f, 1250.0f,
        1600.0f, 2000.0f, 2500.0f, 3150.0f, 4000.0f, 5000.0f,
        6300.0f, 8000.0f, 10000.0f, 12500.0f, 16000.0f,
        20000.0f, 20000.0f, 20000.0f
    };

    for (int i = 0; i < 28; i++) {
        result->correctionFreq[i] = eqFreqs[i];

        // Trova la risposta misurata più vicina a questa frequenza
        float closestMag = 0.0f;
        float minDiff = 1e9f;
        for (int j = 0; j < 64; j++) {
            float diff = fabsf(result->responseFreq[j] - eqFreqs[i]);
            if (diff < minDiff) {
                minDiff = diff;
                closestMag = result->responseMagnitude[j];
            }
        }

        // La correzione è l'inverso della risposta, limitata a ±12dB
        float correction = -closestMag;
        result->correctionGain[i] = fmaxf(-12.0f, fminf(12.0f, correction));
    }
}

// Stima il delay di allineamento dalla risposta in frequenza
// Usa la relazione tra sfasamento e delay di gruppo
static float estimate_delay_ms(const AutotuneResult* result) {
    // Stima semplificata: analizza la posizione del primo picco nella risposta
    // Un sistema più preciso userebbe la derivata della fase (group delay)
    float estimatedDelay = 0.0f;

    // Cerca il punto di -3dB nella zona grave per stimare la distanza
    for (int i = 0; i < 63; i++) {
        if (result->responseMagnitude[i] < -3.0f && result->responseFreq[i] > 200.0f) {
            // Stima delay dalla frequenza di transizione: d = 1/(2*pi*fc) * 1000 ms
            estimatedDelay = 1000.0f / (2.0f * M_PI * result->responseFreq[i]);
            break;
        }
    }

    // Limita il delay stimato a un range ragionevole (0-100ms)
    return fmaxf(0.0f, fminf(100.0f, estimatedDelay));
}

// ======= Inizializzazione =======

void autotune_init() {
    memset(&s_status, 0, sizeof(AutotuneStatus));
    s_status.state = AutotuneState::Idle;
    s_initialized  = true;
    Serial.println("[AUTOTUNE] Modulo autotune inizializzato");
}

// ======= Avvio procedura =======

bool autotune_start(AutotuneMode mode, uint8_t targetId) {
    if (!s_initialized) autotune_init();
    if (s_status.state != AutotuneState::Idle) {
        Serial.println("[AUTOTUNE] Procedura già in corso");
        return false;
    }

    s_status.mode            = mode;
    s_status.targetDeviceId  = targetId;
    s_status.currentDeviceIdx = 0;
    s_status.numResults      = 0;
    s_status.progress        = 0.0f;
    s_status.state           = AutotuneState::Muting;

    if (mode == AutotuneMode::Single) {
        s_status.totalDevices = 1;
        set_status_message("Avvio calibrazione singola cassa...");
    } else {
        s_status.totalDevices = 8; // Default: 8 casse (da parametrizzare)
        set_status_message("Avvio calibrazione impianto completo...");
    }

    return true;
}

// ======= Tick state machine =======

void autotune_tick() {
    if (!s_initialized || s_status.state == AutotuneState::Idle ||
        s_status.state == AutotuneState::Complete ||
        s_status.state == AutotuneState::Error) {
        return;
    }

    static uint32_t s_stateStart = 0;
    uint32_t now = millis();
    uint8_t currentDevId = (s_status.mode == AutotuneMode::Single) ?
                            s_status.targetDeviceId :
                            (s_status.currentDeviceIdx + 1);

    switch (s_status.state) {
        case AutotuneState::Muting: {
            // Muta tutte le casse tranne quella in test
            char msg[128];
            snprintf(msg, sizeof(msg), "Mute impianto – test cassa ID=%d...", currentDevId);
            set_status_message(msg);
            dsp_set_group_mute(DspGroupType::ALL, true);
            dsp_set_speaker_mute(currentDevId, false);
            s_stateStart = now;
            s_status.state = AutotuneState::Generating;
            s_status.progress = 0.1f;
            break;
        }

        case AutotuneState::Generating: {
            // Attendi 500ms dopo il mute, poi genera sweep
            if (now - s_stateStart < 500) break;
            char msg[128];
            snprintf(msg, sizeof(msg), "Generazione sweep 20Hz-20kHz per cassa %d...", currentDevId);
            set_status_message(msg);
            // Attiva modalità TestTone per generare segnale di test
            setAudioMode(AudioMode::TestTone);
            s_stateStart = now;
            s_status.state = AutotuneState::Capturing;
            s_status.progress = 0.3f;
            break;
        }

        case AutotuneState::Capturing: {
            // Attendi 2s di cattura audio
            if (now - s_stateStart < 2000) break;
            char msg[128];
            snprintf(msg, sizeof(msg), "Cattura risposta in frequenza cassa %d...", currentDevId);
            set_status_message(msg);
            setAudioMode(AudioMode::MixerPassThrough);
            s_stateStart = now;
            s_status.state = AutotuneState::Analyzing;
            s_status.progress = 0.6f;
            break;
        }

        case AutotuneState::Analyzing: {
            // Analisi FFT risposta
            if (now - s_stateStart < 500) break;
            char msg[128];
            snprintf(msg, sizeof(msg), "Analisi FFT e calcolo correzione EQ cassa %d...", currentDevId);
            set_status_message(msg);

            // Calcola risposta e correzione
            uint8_t resultIdx = s_status.numResults;
            s_status.results[resultIdx].deviceId = currentDevId;

            // Misura la risposta (in produzione: usa ADC ES8388 reale)
            simulate_frequency_response(&s_status.results[resultIdx]);
            calculate_eq_correction(&s_status.results[resultIdx]);
            s_status.results[resultIdx].estimatedDelayMs =
                estimate_delay_ms(&s_status.results[resultIdx]);
            s_status.results[resultIdx].valid = true;
            s_status.numResults++;

            s_status.state    = AutotuneState::ApplyingEQ;
            s_status.progress = 0.8f;
            s_stateStart = now;
            break;
        }

        case AutotuneState::ApplyingEQ: {
            // Attendi 200ms, poi applica se modalità Single o avanza a prossima cassa
            if (now - s_stateStart < 200) break;

            bool allDone = false;
            if (s_status.mode == AutotuneMode::Single) {
                allDone = true;
            } else {
                s_status.currentDeviceIdx++;
                if (s_status.currentDeviceIdx >= s_status.totalDevices) {
                    allDone = true;
                } else {
                    // Prossima cassa
                    s_status.state    = AutotuneState::Muting;
                    s_status.progress = (float)s_status.currentDeviceIdx / s_status.totalDevices;
                }
            }

            if (allDone) {
                // Ripristina mute
                dsp_set_group_mute(DspGroupType::ALL, false);
                s_status.state    = AutotuneState::Complete;
                s_status.progress = 1.0f;
                set_status_message("Calibrazione completata! Controlla i risultati e applica.");
            }
            break;
        }

        default:
            break;
    }
}

bool autotune_apply_results() {
    if (s_status.state != AutotuneState::Complete || s_status.numResults == 0) {
        Serial.println("[AUTOTUNE] Nessun risultato da applicare");
        return false;
    }

    Serial.printf("[AUTOTUNE] Applicazione risultati per %d casse...\n", s_status.numResults);

    for (uint8_t i = 0; i < s_status.numResults; i++) {
        AutotuneResult& r = s_status.results[i];
        if (!r.valid) continue;

        // Applica correzione EQ (28 bande)
        for (int band = 0; band < 28; band++) {
            if (fabsf(r.correctionGain[band]) > 0.5f) {  // Applica solo se > 0.5dB
                dsp_set_eq_band(r.deviceId, (uint8_t)band,
                                (uint16_t)r.correctionFreq[band],
                                r.correctionGain[band],
                                1.4f,    // Q=1.4 (valore standard)
                                0);      // Tipo: Parametric
            }
        }

        // Applica delay di allineamento
        if (r.estimatedDelayMs > 1.0f) {
            dsp_set_speaker_delay(r.deviceId, r.estimatedDelayMs);
        }

        Serial.printf("[AUTOTUNE] Cassa %d: EQ applicato, delay=%.1fms\n",
                      r.deviceId, r.estimatedDelayMs);
    }

    // Salva come preset "AUTOTUNE_RESULT"
    dsp_save_preset("AUTOTUNE_RESULT", "Risultato calibrazione automatica");

    set_status_message("Correzioni EQ e delay applicate al DSP");
    return true;
}

void autotune_cancel() {
    if (s_status.state != AutotuneState::Idle) {
        // Ripristina mute a OFF e modalità audio
        dsp_set_group_mute(DspGroupType::ALL, false);
        setAudioMode(AudioMode::MixerPassThrough);
        s_status.state = AutotuneState::Idle;
        set_status_message("Procedura autotune annullata");
    }
}

const AutotuneStatus* autotune_get_status() {
    return &s_status;
}
