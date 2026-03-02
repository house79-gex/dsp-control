#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <driver/i2s.h>
#include <cstdint>

// ======= Driver ES8388 – Codec Audio I2C + I2S =======
// Supporta cattura (ADC) e playback (DAC) via I2S DMA

// Indirizzo I2C default ES8388
#define ES8388_ADDR_DEFAULT  0x10

// Registri ES8388 (principali)
#define ES8388_REG_CHIPPOWER     0x02
#define ES8388_REG_ADCPOWER      0x03
#define ES8388_REG_DACPOWER      0x04
#define ES8388_REG_MASTERMODE    0x08
#define ES8388_REG_ADCCONTROL1   0x09
#define ES8388_REG_ADCCONTROL4   0x0C
#define ES8388_REG_ADCCONTROL5   0x0D
#define ES8388_REG_ADCCONTROL8   0x10
#define ES8388_REG_ADCCONTROL9   0x11
#define ES8388_REG_ADCCONTROL10  0x12
#define ES8388_REG_ADCCONTROL11  0x13
#define ES8388_REG_ADCCONTROL12  0x14
#define ES8388_REG_ADCCONTROL13  0x15
#define ES8388_REG_ADCCONTROL14  0x16
#define ES8388_REG_DACCONTROL1   0x17
#define ES8388_REG_DACCONTROL2   0x18
#define ES8388_REG_DACCONTROL3   0x19
#define ES8388_REG_DACCONTROL4   0x1A
#define ES8388_REG_DACCONTROL5   0x1B
#define ES8388_REG_DACCONTROL7   0x1D
#define ES8388_REG_DACCONTROL17  0x27
#define ES8388_REG_DACCONTROL20  0x2A
#define ES8388_REG_DACCONTROL21  0x2B
#define ES8388_REG_DACCONTROL23  0x2D
#define ES8388_REG_DACCONTROL24  0x2E
#define ES8388_REG_DACCONTROL25  0x2F
#define ES8388_REG_DACCONTROL26  0x30
#define ES8388_REG_DACCONTROL27  0x31

class ES8388 {
public:
    // Inizializzazione codec via I2C
    // wire: istanza TwoWire (Wire o Wire1)
    // addr: indirizzo I2C (default 0x10)
    bool begin(TwoWire* wire = &Wire, uint8_t addr = ES8388_ADDR_DEFAULT);

    // Configura sample rate (es. 48000 Hz)
    void setSampleRate(uint32_t rate);

    // Configura bit depth (16 o 24 bit)
    void setBitsPerSample(uint8_t bits);

    // Imposta volume output DAC (0-100%)
    void setVolume(uint8_t volumePercent);

    // Imposta gain ADC (0-100%)
    void setInputGain(uint8_t gainPercent);

    // Avvia modalità cattura (ADC attivo, DAC passivo)
    void startCapture();

    // Avvia modalità playback (DAC attivo, ADC passivo)
    void startPlayback();

    // Legge campioni stereo dal buffer DMA I2S (ADC)
    // buf: buffer destinazione, len: numero campioni stereo richiesti
    // Ritorna numero campioni effettivamente letti
    size_t readSamples(int16_t* buf, size_t len);

    // Scrive campioni stereo nel buffer DMA I2S (DAC)
    // buf: buffer sorgente, len: numero campioni stereo
    // Ritorna numero campioni effettivamente scritti
    size_t writeSamples(const int16_t* buf, size_t len);

    // Mute/unmute output DAC
    void setMute(bool muted);

    // Verifica se codec risponde su I2C
    bool isConnected();

private:
    TwoWire* _wire     = nullptr;
    uint8_t  _addr     = ES8388_ADDR_DEFAULT;
    uint32_t _sampleRate = 48000;
    uint8_t  _bits       = 16;
    bool     _i2sInitialized = false;
    i2s_port_t _i2sPort = I2S_NUM_0;

    bool writeReg(uint8_t reg, uint8_t val);
    uint8_t readReg(uint8_t reg);
    void configureI2S();
    void powerDown();
    void powerUp();
};

// Istanza globale singleton
extern ES8388 es8388;
