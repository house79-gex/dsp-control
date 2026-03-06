#include "audio_protection.h"
#include <Arduino.h>
#include <math.h>

// Istanza globale
AudioProtection g_audioProtection;

// ——— Inizializzazione ———

void AudioProtection::init(uint32_t sampleRate) {
    m_sampleRate = sampleRate;

    // Coefficienti limiter: attack 0.5ms, release 50ms
    float attackMs  = 0.5f;
    float releaseMs = 50.0f;
    m_attackCoeff   = expf(-1.0f / (sampleRate * attackMs  * 1e-3f));
    m_releaseCoeff  = expf(-1.0f / (sampleRate * releaseMs * 1e-3f));
    m_limiterGain   = 1.0f;

    // HPF Butterworth 1° ordine @ 30Hz
    // H(s) = s / (s + wc)  →  bilinear transform
    // wc = 2*pi*fc
    const float fc   = 30.0f;
    const float wc   = 2.0f * (float)M_PI * fc;
    const float T    = 1.0f / (float)sampleRate;
    const float k    = wc * T / 2.0f;
    // Pre-warped: wd = 2/T * tan(pi*fc/fs)
    const float wd   = (2.0f / T) * tanf((float)M_PI * fc / (float)sampleRate);
    const float norm = 1.0f / (wd + 2.0f / T);
    m_hpfB0 = (2.0f / T) * norm;
    m_hpfB1 = -(2.0f / T) * norm;
    m_hpfA1 = (wd - 2.0f / T) * norm;

    // Azzeramento stati
    m_dcPrevInL = m_dcPrevOutL = 0.0f;
    m_dcPrevInR = m_dcPrevOutR = 0.0f;
    m_hpfStateL = m_hpfStateR  = 0.0f;

    // Default -1 dBFS
    m_thresholdLin = powf(10.0f, -1.0f / 20.0f);
    m_clipCount    = 0;
    m_initialized  = true;

    Serial.printf("[PROT] Init: SR=%u Hz, limiter=%.4f lin (≈-1 dBFS)\n",
                  sampleRate, m_thresholdLin);
}

void AudioProtection::setLimiterThreshold(float thresholdDbfs) {
    m_thresholdLin = powf(10.0f, thresholdDbfs / 20.0f);
}

// ——— Filtri ———

// DC blocking IIR: y[n] = x[n] - x[n-1] + 0.9995 * y[n-1]
// Frequenza di taglio ≈ 1Hz a 48kHz
float AudioProtection::applyDCBlock(float x, float& prevIn, float& prevOut) {
    const float alpha = 0.9995f;
    float y = x - prevIn + alpha * prevOut;
    prevIn  = x;
    prevOut = y;
    return y;
}

// HPF Butterworth 1° ordine (df1)
float AudioProtection::applyHPF30(float x, float& state) {
    float y = m_hpfB0 * x + state;
    state = m_hpfB1 * x - m_hpfA1 * y;
    return y;
}

// Limiter envelope follower
float AudioProtection::applyLimiter(float sample) {
    float absVal = fabsf(sample);
    float targetGain = 1.0f;

    if (absVal > m_thresholdLin && absVal > 1e-9f) {
        targetGain = m_thresholdLin / absVal;
        m_clipCount++;
    }

    // Envelope follower (attack/release)
    if (targetGain < m_limiterGain) {
        m_limiterGain = m_attackCoeff  * m_limiterGain + (1.0f - m_attackCoeff)  * targetGain;
    } else {
        m_limiterGain = m_releaseCoeff * m_limiterGain + (1.0f - m_releaseCoeff) * targetGain;
    }

    return sample * m_limiterGain;
}

// Soft clipping tanh
float AudioProtection::applySoftClip(float sample) {
    // tanh limita dolcemente a ±1 con un margine di sicurezza
    return tanhf(sample);
}

// ——— Processo principale ———

void AudioProtection::process(int16_t* buffer, size_t frameCount) {
    if (!m_initialized || !buffer || frameCount == 0) return;

    const float inv32768 = 1.0f / 32768.0f;

    for (size_t i = 0; i < frameCount; i++) {
        float l = buffer[i * 2]     * inv32768;
        float r = buffer[i * 2 + 1] * inv32768;

        // 1. DC Blocking
        l = applyDCBlock(l, m_dcPrevInL, m_dcPrevOutL);
        r = applyDCBlock(r, m_dcPrevInR, m_dcPrevOutR);

        // 2. HPF 30Hz
        l = applyHPF30(l, m_hpfStateL);
        r = applyHPF30(r, m_hpfStateR);

        // 3. Limiter (condivide gain tra L e R per preservare la stereo image)
        float absMax = (fabsf(l) > fabsf(r)) ? fabsf(l) : fabsf(r);
        float targetGain = 1.0f;
        if (absMax > m_thresholdLin && absMax > 1e-9f) {
            targetGain = m_thresholdLin / absMax;
            m_clipCount++;
        }
        if (targetGain < m_limiterGain) {
            m_limiterGain = m_attackCoeff  * m_limiterGain + (1.0f - m_attackCoeff)  * targetGain;
        } else {
            m_limiterGain = m_releaseCoeff * m_limiterGain + (1.0f - m_releaseCoeff) * targetGain;
        }
        l *= m_limiterGain;
        r *= m_limiterGain;

        // 4. Soft clip finale
        l = applySoftClip(l);
        r = applySoftClip(r);

        // Converti a int16 con saturazione
        float lScaled = l * 32767.0f;
        float rScaled = r * 32767.0f;
        buffer[i * 2]     = (lScaled >  32767.0f) ?  32767 :
                             (lScaled < -32768.0f) ? -32768 : (int16_t)lScaled;
        buffer[i * 2 + 1] = (rScaled >  32767.0f) ?  32767 :
                             (rScaled < -32768.0f) ? -32768 : (int16_t)rScaled;
    }
}
