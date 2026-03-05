#include "audio_src.h"
#include <Arduino.h>

// Istanza globale
AudioSRC g_audioSrc;

// ——— Inizializzazione ———

bool AudioSRC::init(uint32_t inputSR, uint32_t outputSR) {
    if (inputSR == 0 || outputSR == 0) {
        Serial.println("[SRC] Errore: sample rate non valido");
        return false;
    }
    m_inputSR     = inputSR;
    m_outputSR    = outputSR;
    m_ratio       = (float)outputSR / (float)inputSR;
    m_phase       = 0.0f;
    m_prevL       = 0;
    m_prevR       = 0;
    m_initialized = true;

    Serial.printf("[SRC] Init: %u Hz → %u Hz (ratio=%.6f)\n",
                  inputSR, outputSR, m_ratio);
    return true;
}

// ——— Elaborazione ———

// Interpolazione lineare stereo interleaved.
// Per ogni campione di uscita, calcola la posizione frazionaria nell'input
// e interpola tra i due campioni adiacenti.
size_t AudioSRC::process(const int16_t* input, size_t inputFrames,
                         int16_t* output, size_t outputCapacity) {
    if (!m_initialized || inputFrames == 0 || outputCapacity == 0) return 0;

    size_t outIdx = 0;

    while (outIdx < outputCapacity) {
        // Posizione nel buffer di input (in frame)
        float inPos = m_phase / m_ratio;

        // Frame intero nel buffer di input
        size_t inFrame = (size_t)inPos;
        float  frac    = inPos - (float)inFrame;

        // Campioni del frame corrente (inFrame) e successivo (inFrame+1)
        int16_t curL, curR, nextL, nextR;

        if (inFrame == 0) {
            // Primo frame: usa campione precedente come "frame-1"
            curL  = m_prevL;
            curR  = m_prevR;
            nextL = (inFrame < inputFrames) ? input[inFrame * 2]     : m_prevL;
            nextR = (inFrame < inputFrames) ? input[inFrame * 2 + 1] : m_prevR;
        } else if (inFrame < inputFrames) {
            curL  = input[(inFrame - 1) * 2];
            curR  = input[(inFrame - 1) * 2 + 1];
            nextL = (inFrame < inputFrames) ? input[inFrame * 2]     : curL;
            nextR = (inFrame < inputFrames) ? input[inFrame * 2 + 1] : curR;
        } else {
            // Esaurito l'input disponibile
            break;
        }

        // Interpolazione lineare
        output[outIdx * 2]     = (int16_t)(curL + frac * (nextL - curL));
        output[outIdx * 2 + 1] = (int16_t)(curR + frac * (nextR - curR));
        outIdx++;

        // Avanza la fase di output di 1 unità (un frame di output)
        m_phase += 1.0f;

        // Controlla se abbiamo consumato tutto l'input disponibile
        float nextInPos = m_phase / m_ratio;
        if ((size_t)nextInPos >= inputFrames + 1) break;
    }

    // Salva l'ultimo campione input come storia per il prossimo blocco
    if (inputFrames > 0) {
        m_prevL = input[(inputFrames - 1) * 2];
        m_prevR = input[(inputFrames - 1) * 2 + 1];
    }

    // Aggiusta la fase: sottrai i frame input consumati (convertiti in fase output)
    float inputConsumed = (float)inputFrames;
    m_phase -= inputConsumed * m_ratio;
    if (m_phase < 0.0f) m_phase = 0.0f;

    return outIdx;
}

// ——— Reset ———

void AudioSRC::reset() {
    m_phase  = 0.0f;
    m_prevL  = 0;
    m_prevR  = 0;
}
