#include "audio_delay_buffer.h"
#include <Arduino.h>
#include <stdlib.h>
#include <string.h>

// Istanza globale
AudioDelayBuffer g_audioDelay;

// ——— Inizializzazione ———

bool AudioDelayBuffer::init(uint32_t sampleRate, float maxDelayMs) {
    m_sampleRate  = sampleRate;
    m_maxDelayMs  = maxDelayMs;
    m_delayMs     = 0.0f;
    m_delayFrames = 0;
    m_writePtr    = 0;

    // Numero di frame stereo necessari (aggiunge 1 frame di margine)
    m_bufferFrames = (size_t)(sampleRate * maxDelayMs / 1000.0f) + 1;

    // Allocazione in PSRAM con ps_malloc
    size_t bytes = m_bufferFrames * 2 * sizeof(int16_t);  // 2 campioni/frame
    m_buffer = (int16_t*)ps_malloc(bytes);
    if (!m_buffer) {
        // Fallback su heap normale se PSRAM non disponibile
        m_buffer = (int16_t*)malloc(bytes);
        if (!m_buffer) {
            Serial.printf("[DELAY] Errore allocazione %u byte\n", (unsigned)bytes);
            m_bufferFrames = 0;
            return false;
        }
        Serial.printf("[DELAY] Allocato %u byte su heap (PSRAM non disponibile)\n", (unsigned)bytes);
    } else {
        Serial.printf("[DELAY] Allocato %u byte in PSRAM\n", (unsigned)bytes);
    }

    memset(m_buffer, 0, bytes);
    Serial.printf("[DELAY] Init: SR=%u Hz, max=%.1f ms, %u frame\n",
                  sampleRate, maxDelayMs, (unsigned)m_bufferFrames);
    return true;
}

void AudioDelayBuffer::deinit() {
    if (m_buffer) {
        free(m_buffer);
        m_buffer = nullptr;
    }
    m_bufferFrames = 0;
}

// ——— Controllo delay ———

void AudioDelayBuffer::setDelayMs(float delayMs) {
    if (delayMs < 0.0f) delayMs = 0.0f;
    if (delayMs > m_maxDelayMs) delayMs = m_maxDelayMs;
    m_delayMs     = delayMs;
    m_delayFrames = (size_t)(m_sampleRate * delayMs / 1000.0f);
    if (m_delayFrames >= m_bufferFrames) m_delayFrames = m_bufferFrames - 1;
}

// ——— Write ———

void AudioDelayBuffer::write(const int16_t* frames, size_t frameCount) {
    if (!m_buffer || frameCount == 0) return;
    for (size_t i = 0; i < frameCount; i++) {
        m_buffer[m_writePtr * 2]     = frames[i * 2];
        m_buffer[m_writePtr * 2 + 1] = frames[i * 2 + 1];
        m_writePtr = (m_writePtr + 1) % m_bufferFrames;
    }
}

// ——— Read ———

// Legge con il delay configurato: ritorna i campioni scritti delayFrames fa
void AudioDelayBuffer::read(int16_t* output, size_t frameCount) {
    if (!m_buffer || !output || frameCount == 0) return;

    // Il puntatore di lettura è delayFrames indietro rispetto al write pointer
    size_t readPtr = (m_writePtr + m_bufferFrames - m_delayFrames) % m_bufferFrames;

    for (size_t i = 0; i < frameCount; i++) {
        output[i * 2]     = m_buffer[readPtr * 2];
        output[i * 2 + 1] = m_buffer[readPtr * 2 + 1];
        readPtr = (readPtr + 1) % m_bufferFrames;
    }
}
