#pragma once
#include <stdint.h>
#include <stddef.h>

// ======= Audio Protection Pipeline =======
// Pipeline di protezione audio applicata prima dell'uscita DAC/DSP.
//
// Catena:
//   Input PCM → DC Blocking Filter → HPF 30Hz → Brick-Wall Limiter → Soft Clip → Output
//
// DC Blocking: IIR HPF 1Hz (rimuove offset DC)
// HPF 30Hz:    Butterworth 1° ordine (protegge da infrasuoni)
// Limiter:     Brick-wall -1 dBFS, attack 0.5ms, release 50ms
// Soft Clip:   tanh normalizzato per sicurezza finale

class AudioProtection {
public:
    // Inizializza con sample rate (Hz)
    void init(uint32_t sampleRate);

    // Imposta soglia limiter in dBFS (es. -1.0f)
    void setLimiterThreshold(float thresholdDbfs);

    // Processa buffer stereo interleaved (frameCount frame L+R)
    // Modifica il buffer in-place
    void process(int16_t* buffer, size_t frameCount);

    // Numero di eventi di clip rilevati dall'ultimo reset
    uint32_t getClipCount() const { return m_clipCount; }

    // Azzera contatore clip
    void resetClipCount() { m_clipCount = 0; }

    bool isInitialized() const { return m_initialized; }

private:
    // DC Blocking (IIR HPF ~1Hz): y[n] = x[n] - x[n-1] + alpha * y[n-1]
    float applyDCBlock(float sample, float& prevIn, float& prevOut);

    // HPF 30Hz (Butterworth 1° ordine)
    float applyHPF30(float sample, float& state);

    // Limiter con envelope follower
    float applyLimiter(float sample);

    // Soft clip tanh
    static float applySoftClip(float sample);

    uint32_t m_sampleRate     = 48000;
    float    m_thresholdLin   = 0.891f;   // -1 dBFS ≈ 10^(-1/20)
    float    m_limiterGain    = 1.0f;
    float    m_attackCoeff    = 0.0f;
    float    m_releaseCoeff   = 0.0f;

    // DC blocking state (L, R)
    float    m_dcPrevInL  = 0.0f, m_dcPrevOutL = 0.0f;
    float    m_dcPrevInR  = 0.0f, m_dcPrevOutR = 0.0f;

    // HPF 30Hz state (L, R)
    float    m_hpfStateL  = 0.0f;
    float    m_hpfStateR  = 0.0f;
    float    m_hpfA1      = 0.0f;  // Coefficiente filtro
    float    m_hpfB0      = 0.0f;
    float    m_hpfB1      = 0.0f;

    uint32_t m_clipCount  = 0;
    bool     m_initialized = false;
};

// Istanza globale
extern AudioProtection g_audioProtection;
