#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <freertos/FreeRTOS.h>

// ======= ES8388 Codec Driver =======
// Controllo via I2C per configurazione ADC gain, sample rate e bit depth
// Datasheet: ES8388 by Everest Semiconductor

class ES8388 {
public:
    ES8388() : _i2cAddr(0x10), _wire(nullptr) {}

    // Inizializza il driver I2C. Restituisce true se il chip risponde.
    bool begin(TwoWire* wire, uint8_t i2cAddr);

    // ——— Configurazione ADC ———

    // Imposta guadagno ingresso ADC (PGA).
    // Registro 0x09 (L) e 0x0A (R): range -96 dB … +24 dB, step 0.5 dB.
    void  setADCGain(float gainDb);
    float getADCGain();

    // ——— Configurazione codec ———

    void setSampleRate(uint32_t sampleRate);
    void setBitsPerSample(uint8_t bits);

    // ——— Controllo alimentazione ———

    // Attiva il percorso ADC (microfono/line-in → I2S).
    void startCapture();

    // Attiva il percorso DAC (I2S → uscita analogica).
    void startPlayback();

    // ——— I/O campioni via I2S DMA ———

    // Legge campioni PCM dall'ADC via I2S DMA.
    // buffer: array destinazione int32_t, count: numero campioni, timeout: ticks FreeRTOS.
    // Restituisce il numero di campioni effettivamente letti.
    size_t readSamples(int32_t* buffer, size_t count, TickType_t timeout);

private:
    uint8_t    _i2cAddr;
    TwoWire*   _wire;

    void    writeReg(uint8_t reg, uint8_t value);
    uint8_t readReg(uint8_t reg);
};
