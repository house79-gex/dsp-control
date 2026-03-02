#include "ES8388.h"
#include <driver/i2s.h>

// ======= ES8388 Registro ADC Volume =======
// Registro 0x09 = Left ADC Volume
// Registro 0x0A = Right ADC Volume
// Range: 0x00 = mute, 0xC0 = 0 dB, 0xFF = +24 dB
// Formula: regValue = (gainDb + 96.0) / 0.5
// Inversa: gainDb = regValue * 0.5 - 96.0

bool ES8388::begin(TwoWire* wire, uint8_t i2cAddr) {
    _wire    = wire;
    _i2cAddr = i2cAddr;

    // Verifica presenza chip sul bus I2C
    _wire->beginTransmission(_i2cAddr);
    uint8_t err = _wire->endTransmission();
    if (err != 0) {
        Serial.printf("[ES8388] Chip non trovato su I2C addr 0x%02X (err=%d)\n", _i2cAddr, err);
        return false;
    }
    Serial.printf("[ES8388] Chip trovato su I2C addr 0x%02X\n", _i2cAddr);
    return true;
}

void ES8388::setADCGain(float gainDb) {
    // ES8388 ADC PGA (Programmable Gain Amplifier)
    // Registri 0x09 (Left ADC Volume) e 0x0A (Right ADC Volume)
    // Range: -96 dB a +24 dB in step di 0.5 dB
    gainDb = constrain(gainDb, -96.0f, 24.0f);
    uint8_t regValue = (uint8_t)((gainDb + 96.0f) / 0.5f);

    writeReg(0x09, regValue);  // Left ADC volume
    writeReg(0x0A, regValue);  // Right ADC volume

    Serial.printf("[ES8388] ADC gain impostato a %.1f dB (reg: 0x%02X)\n", gainDb, regValue);
}

float ES8388::getADCGain() {
    uint8_t regValue = readReg(0x09);  // Left ADC volume
    return (regValue * 0.5f) - 96.0f;
}

void ES8388::setSampleRate(uint32_t sampleRate) {
    // ES8388 registro 0x08 (MASTERMODE): modalità slave (ESP32 è master I2S)
    // Bit [5:3] = 000 → MCLKDIV = 256×fs (default, valido per 44100 e 48000 Hz)
    // Bit [0]   = 0   → slave mode (MCLK, BCLK, LRCK ricevuti dall'esterno)
    writeReg(0x08, 0x00);

    // ADC oversampling speed: single-speed per ≤48 kHz, double-speed per >48 kHz
    // Registro 0x0D (ADCCONTROL5), bit[3]: 0=single-speed, 1=double-speed
    uint8_t adcCtl5 = readReg(0x0D);
    uint8_t speedBit = (sampleRate > 48000) ? (1 << 3) : 0;
    writeReg(0x0D, (adcCtl5 & ~(1 << 3)) | speedBit);

    Serial.printf("[ES8388] Sample rate: %u Hz (slave, MCLK=256fs)\n", sampleRate);
}

void ES8388::startCapture() {
    // Sequenza di alimentazione ADC secondo datasheet ES8388:
    // 1. Registro 0x00 (CONTROL1): abilita riferimento analogico e alimentazione ADC
    //    bit[4]=1 (analog power on), bit[3]=1 (ADC digital power on)
    uint8_t ctrl1 = readReg(0x00);
    writeReg(0x00, ctrl1 | 0x18);

    // 2. Registro 0x03 (ADCPOWER): alimenta tutti i componenti ADC (0x00 = tutto on)
    writeReg(0x03, 0x00);

    // 3. Registro 0x0B (ADCCONTROL3): stereo ADC, modalità normale
    writeReg(0x0B, 0x02);

    Serial.println("[ES8388] ADC capture avviato");
}

void ES8388::startPlayback() {
    // Sequenza di alimentazione DAC secondo datasheet ES8388:
    // 1. Registro 0x01 (CONTROL2): alimenta il DAC digitale (clear bit[2])
    uint8_t ctrl2 = readReg(0x01);
    writeReg(0x01, ctrl2 & ~(1 << 2));

    // 2. Registro 0x04 (DACPOWER): alimenta DAC L/R e driver di uscita
    //    bit[5]=ROUT2EN, bit[4]=LOUT2EN, bit[3]=ROUT1EN, bit[2]=LOUT1EN,
    //    bit[1]=DACRPWR, bit[0]=DACLPWR
    writeReg(0x04, 0x3F);

    Serial.println("[ES8388] DAC playback avviato");
}

size_t ES8388::readSamples(int32_t* buffer, size_t count, TickType_t timeout) {
    // Usare con I2S configurato a 32-bit (o 24-bit padded a 32-bit).
    // Per configurazioni 16-bit usare i2s_read direttamente con buffer int16_t.
    size_t bytesRead = 0;
    esp_err_t err = i2s_read(I2S_NUM_0, buffer, count * sizeof(int32_t), &bytesRead, timeout);
    if (err != ESP_OK) {
        Serial.printf("[ES8388] readSamples i2s_read err=%d\n", err);
        return 0;
    }
    return bytesRead / sizeof(int32_t);
}

void ES8388::setBitsPerSample(uint8_t bits) {
    // ES8388 registro 0x17 (SDP_IN) e 0x18 (SDP_OUT): word length
    // 00 = 24-bit, 01 = 20-bit, 10 = 18-bit, 11 = 16-bit
    uint8_t wl;
    switch (bits) {
        case 24: wl = 0b00; break;
        case 20: wl = 0b01; break;
        case 18: wl = 0b10; break;
        default: wl = 0b11; break;  // 16-bit
    }
    // Aggiorna word length in SDP_IN (reg 0x17) bits [1:0]
    uint8_t sdpIn  = readReg(0x17);
    uint8_t sdpOut = readReg(0x18);
    writeReg(0x17, (sdpIn  & 0xFC) | wl);
    writeReg(0x18, (sdpOut & 0xFC) | wl);
    Serial.printf("[ES8388] Bit depth impostato a %d bit (wl=0x%02X)\n", bits, wl);
}

// ——— Helper I2C privati ———

void ES8388::writeReg(uint8_t reg, uint8_t value) {
    if (!_wire) return;
    _wire->beginTransmission(_i2cAddr);
    _wire->write(reg);
    _wire->write(value);
    uint8_t err = _wire->endTransmission();
    if (err != 0) {
        Serial.printf("[ES8388] writeReg(0x%02X, 0x%02X) err=%d\n", reg, value, err);
    }
}

uint8_t ES8388::readReg(uint8_t reg) {
    if (!_wire) return 0;
    _wire->beginTransmission(_i2cAddr);
    _wire->write(reg);
    if (_wire->endTransmission(false) != 0) return 0;
    _wire->requestFrom((uint8_t)_i2cAddr, (uint8_t)1);
    return _wire->available() ? _wire->read() : 0;
}
