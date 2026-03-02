#include "ES8388.h"

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
    // NOTA: questa funzione è uno stub informativo.
    // La configurazione precisa del sample rate dell'ES8388 dipende dal clock MCLK
    // fornito dall'MCU (tipicamente MCLK = 256× sample rate).
    // In un'implementazione completa con MCLK configurabile, calcolare i registri
    // di divisione ES8388 (MCLKDIV, BCLKDIV) in base al sample rate richiesto.
    // Con I2S_SAMPLE_RATE = 44100 il MCLK deve essere ~11.2896 MHz (256×44100).
    Serial.printf("[ES8388] Sample rate richiesto: %u Hz (configurare MCLK esternamente)\n",
                  sampleRate);
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
