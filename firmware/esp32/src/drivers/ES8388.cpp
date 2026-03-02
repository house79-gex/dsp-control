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

    // Sequenza di inizializzazione registri ES8388
    writeReg(0x00, 0x80);  // Control1: VMIDSEL – abilita riferimento VMID
    writeReg(0x01, 0x58);  // Control2: power up ADC e DAC
    writeReg(0x02, 0xF3);  // ChipPower: abilita tutti i blocchi analogici
    writeReg(0x04, 0x00);  // ADC Power: on
    writeReg(0x05, 0x00);  // DAC Power: on
    writeReg(0x06, 0xC0);  // ChipLowPower: disabilita low-power
    writeReg(0x08, 0x00);  // MasterMode: modalità slave (MCLK/BCLK/LRCK da ESP32)
    writeReg(0x09, 0xC0);  // ADC Left Volume: 0 dB
    writeReg(0x0A, 0xC0);  // ADC Right Volume: 0 dB
    writeReg(0x17, 0x18);  // SDP_IN (ADCCONTROL3): I2S 16-bit
    writeReg(0x18, 0x02);  // SDP_OUT (DACCONTROL1): I2S 16-bit, LRCLK polarity
    writeReg(0x1A, 0x00);  // DAC Left Volume: 0 dB
    writeReg(0x1B, 0x00);  // DAC Right Volume: 0 dB
    writeReg(0x27, 0xB8);  // LOUT1VOL: volume massimo uscita analogica
    writeReg(0x2A, 0xB8);  // ROUT1VOL: volume massimo uscita analogica

    Serial.println("[ES8388] Codec inizializzato");
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

    // ADCCONTROL5 (0x0D): ADC oversampling speed
    // Bit[3]: 0 = single-speed (≤48 kHz), 1 = double-speed (>48 kHz)
    // Bit[2:0]: ADC LR Clock divider ratio (000 = 256/Fs, default)
    uint8_t adcCtl5 = readReg(0x0D);
    uint8_t speedBit = (sampleRate > 48000) ? (1 << 3) : 0;
    writeReg(0x0D, (adcCtl5 & ~(1 << 3)) | speedBit);

    // DACCONTROL2 (0x19): DAC oversampling speed
    // Bit[3]: 0 = single-speed (≤48 kHz), 1 = double-speed (>48 kHz)
    uint8_t dacCtl2 = readReg(0x19);
    writeReg(0x19, (dacCtl2 & ~(1 << 3)) | speedBit);

    // Per 44.1 kHz: ESP32 deve generare MCLK = 11.2896 MHz (256 × 44100)
    // Per 48 kHz:   ESP32 deve generare MCLK = 12.288 MHz  (256 × 48000)
    // ADCCONTROL4 (0x0C): ADC Left/Right polarity e input select (non modificato qui)
    // Registro 0x06 (CHIPPOWER2 / LOUT2VOL in alcune versioni): già configurato in begin()
    Serial.printf("[ES8388] Sample rate: %u Hz (slave, MCLK=256fs, MCLK=%.4f MHz)\n",
                  sampleRate, (sampleRate * 256) / 1000000.0f);
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
    // ES8388 SDP register bit format (0x17 ADCCONTROL3, 0x18 DACCONTROL1):
    // Bit 7:   DAT_LRP
    // Bit 6:   LRP (LRC polarity)
    // Bits 5:4 WL[1:0] word length: 00=24bit, 01=20bit, 10=18bit, 11=16bit
    // Bits 3:2 FMT[1:0] format:     00=I2S, 01=left-justified, 10=right, 11=DSP
    // Bit 1:   reserved
    // Bit 0:   input/output select
    uint8_t wl;
    switch (bits) {
        case 32: wl = 0b00; break;  // 32-bit treated as 24-bit (ES8388 max=24)
        case 24: wl = 0b00; break;
        case 20: wl = 0b01; break;
        case 18: wl = 0b10; break;
        default: wl = 0b11; break;  // 16-bit
    }
    // Word length is in bits [5:4]; preserve other bits
    uint8_t sdpIn  = readReg(0x17);
    uint8_t sdpOut = readReg(0x18);
    writeReg(0x17, (sdpIn  & 0xCF) | (wl << 4));
    writeReg(0x18, (sdpOut & 0xCF) | (wl << 4));
    Serial.printf("[ES8388] Bit depth impostato a %d bit (wl=0x%02X, reg17=0x%02X reg18=0x%02X)\n",
                  bits, wl, readReg(0x17), readReg(0x18));
}

void ES8388::setVolume(uint8_t volume) {
    // Registro DAC digitale 0x1A (L) e 0x1B (R): 0x00=0dB, 0xFF=-96dB (inverso)
    // Map volume 0-100 → registro 255-0
    uint8_t regVal = (uint8_t)(255 * (100 - volume) / 100);
    writeReg(0x1A, regVal);  // Left DAC volume
    writeReg(0x1B, regVal);  // Right DAC volume
    Serial.printf("[ES8388] Volume impostato a %d%% (reg: 0x%02X)\n", volume, regVal);
}

void ES8388::stop() {
    // Power down ADC: registro 0x03 (ADCPOWER) – coerente con startCapture()
    writeReg(0x03, 0xFF);  // ADCPOWER: spegni tutti i blocchi ADC
    // Power down DAC e uscite analogiche: registro 0x04 (DACPOWER) – coerente con startPlayback()
    writeReg(0x04, 0xC0);  // DACPOWER: spegni DAC L/R
    // Disabilita alimentazione chip
    writeReg(0x02, 0xFF);  // ChipPower: power down
    Serial.println("[ES8388] Codec fermato (power down)");
}

size_t ES8388::writeSamples(const int16_t* buffer, size_t count, TickType_t timeout) {
    // Scrive campioni stereo interleaved al DAC via I2S DMA.
    // count = numero campioni per canale; buffer ha count*2 elementi (L,R alternati).
    size_t bytesWritten = 0;
    esp_err_t err = i2s_write(I2S_NUM_0, buffer,
                              count * 2 * sizeof(int16_t),
                              &bytesWritten, timeout);
    if (err != ESP_OK) {
        Serial.printf("[ES8388] writeSamples i2s_write err=%d\n", err);
        return 0;
    }
    return bytesWritten / (2 * sizeof(int16_t));
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
