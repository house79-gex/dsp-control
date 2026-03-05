#pragma once
#include <Arduino.h>
#include <stdint.h>
#include <stddef.h>

// ======= Sample Rate Converter (SRC) =======
// Converte audio PCM da un sample rate arbitrario a 48kHz per il DSP CQ260D.
// Algoritmo: interpolazione lineare (latenza < 0.1 ms, bassa complessità CPU).
// Processo campioni stereo interleaved (L, R, L, R, ...).

class AudioSRC {
public:
    // Inizializza il convertitore
    // inputSR:  sample rate sorgente (es. 44100)
    // outputSR: sample rate destinazione (sempre 48000 per DSP)
    // Ritorna true se inizializzazione riuscita
    bool init(uint32_t inputSR, uint32_t outputSR);

    // Processa un blocco di campioni
    // input:        buffer di ingresso stereo interleaved (L,R,L,R,...)
    // inputFrames:  numero di frame (coppie L+R) in input
    // output:       buffer di uscita stereo interleaved (almeno outputCapacity frames)
    // outputCapacity: dimensione massima buffer output in frame
    // Ritorna: numero di frame prodotti
    size_t process(const int16_t* input, size_t inputFrames,
                   int16_t* output, size_t outputCapacity);

    // Reset stato interno (storia campioni)
    void reset();

    // Accessori
    bool     isInitialized() const { return m_initialized; }
    uint32_t getInputSR()    const { return m_inputSR; }
    uint32_t getOutputSR()   const { return m_outputSR; }
    float    getRatio()      const { return m_ratio; }

private:
    uint32_t m_inputSR      = 0;
    uint32_t m_outputSR     = 0;
    float    m_ratio        = 1.0f;   // outputSR / inputSR
    float    m_phase        = 0.0f;   // Posizione interpolazione (0.0 – 1.0)
    int16_t  m_prevL        = 0;      // Campione L precedente per interpolazione
    int16_t  m_prevR        = 0;      // Campione R precedente per interpolazione
    bool     m_initialized  = false;
};

// Istanza globale
extern AudioSRC g_audioSrc;
