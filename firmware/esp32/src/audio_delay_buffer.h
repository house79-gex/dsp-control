#pragma once
#include <stdint.h>
#include <stddef.h>

// ======= Audio Delay Buffer =======
// Buffer circolare per delay software, allocato in PSRAM.
// Uso: sincronizzare output DAC ES8388 (amplificatori senza DSP) con il resto
// dell'impianto gestito dal DSP CQ260D.
//
// Specifiche:
//   - Max delay: 100 ms @ 48kHz stereo = ~19.2 KB
//   - Buffer in PSRAM (8MB disponibili su ESP32-S3)
//   - Campioni stereo interleaved int16_t

class AudioDelayBuffer {
public:
    // Inizializza il buffer in PSRAM
    // sampleRate: Hz (tipicamente 48000)
    // maxDelayMs: massimo delay supportato in ms
    // Ritorna true se allocazione PSRAM riuscita
    bool init(uint32_t sampleRate, float maxDelayMs);

    // Libera la PSRAM
    void deinit();

    // Imposta delay corrente (0 a maxDelayMs)
    void setDelayMs(float delayMs);

    // Scrive frameCount frame stereo (interleaved) nel buffer
    void write(const int16_t* frames, size_t frameCount);

    // Legge frameCount frame stereo con il delay impostato
    // output: buffer di uscita (almeno frameCount*2 int16_t)
    void read(int16_t* output, size_t frameCount);

    float    getDelayMs()   const { return m_delayMs; }
    uint32_t getSampleRate()const { return m_sampleRate; }
    bool     isInitialized()const { return m_buffer != nullptr; }

private:
    int16_t* m_buffer       = nullptr;
    size_t   m_bufferFrames = 0;    // Capacità totale in frame stereo
    size_t   m_writePtr     = 0;    // Puntatore scrittura (indice frame)
    size_t   m_delayFrames  = 0;    // Ritardo in frame
    uint32_t m_sampleRate   = 48000;
    float    m_delayMs      = 0.0f;
    float    m_maxDelayMs   = 0.0f;
};

// Istanza globale
extern AudioDelayBuffer g_audioDelay;
