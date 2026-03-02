#include "ES8388.h"
#include "../../config.h"

// Istanza globale singleton
ES8388 es8388;

// ======= Comunicazione I2C =======

bool ES8388::writeReg(uint8_t reg, uint8_t val) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->write(val);
    return _wire->endTransmission() == 0;
}

uint8_t ES8388::readReg(uint8_t reg) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->endTransmission(false);
    _wire->requestFrom(_addr, (uint8_t)1);
    return _wire->available() ? _wire->read() : 0xFF;
}

bool ES8388::isConnected() {
    _wire->beginTransmission(_addr);
    return _wire->endTransmission() == 0;
}

// ======= Configurazione I2S =======

void ES8388::configureI2S() {
    if (_i2sInitialized) {
        i2s_driver_uninstall(_i2sPort);
        _i2sInitialized = false;
    }

    i2s_config_t cfg = {
        .mode                 = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
        .sample_rate          = _sampleRate,
        .bits_per_sample      = (_bits == 24) ? I2S_BITS_PER_SAMPLE_24BIT
                                              : I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format       = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count        = 8,
        .dma_buf_len          = 64,
        .use_apll             = true,
        .tx_desc_auto_clear   = true,
        .fixed_mclk           = 0,
    };

    i2s_pin_config_t pins = {
        .mck_io_num   = I2S_PIN_NO_CHANGE,
        .bck_io_num   = I2S_SCK,
        .ws_io_num    = I2S_WS,
        .data_out_num = I2S_SD_OUT,
        .data_in_num  = I2S_SD_IN,
    };

    esp_err_t err = i2s_driver_install(_i2sPort, &cfg, 0, nullptr);
    if (err != ESP_OK) {
        Serial.printf("[ES8388] Errore init I2S: %d\n", err);
        return;
    }
    i2s_set_pin(_i2sPort, &pins);
    _i2sInitialized = true;
    Serial.printf("[ES8388] I2S configurato: %lu Hz %d bit\n",
                  (unsigned long)_sampleRate, _bits);
}

// ======= Inizializzazione codec =======

bool ES8388::begin(TwoWire* wire, uint8_t addr) {
    _wire = wire;
    _addr = addr;

    if (!isConnected()) {
        Serial.printf("[ES8388] Codec non trovato su I2C addr 0x%02X\n", addr);
        return false;
    }
    Serial.printf("[ES8388] Codec trovato su I2C addr 0x%02X\n", addr);

    // Reset software
    writeReg(0x00, 0x80);
    delay(10);
    writeReg(0x00, 0x00);

    // Configurazione iniziale
    writeReg(ES8388_REG_MASTERMODE,  0x00);  // slave mode
    writeReg(ES8388_REG_CHIPPOWER,   0x00);  // chip power on
    writeReg(ES8388_REG_DACPOWER,    0x00);  // DAC power down (ancora)
    writeReg(ES8388_REG_ADCPOWER,    0xFF);  // ADC power down (ancora)

    // Configurazione ADC
    writeReg(ES8388_REG_ADCCONTROL1,  0x00);  // MicBias off, LIN1/RIN1
    writeReg(ES8388_REG_ADCCONTROL4,  0x0D);  // 16bit I2S
    writeReg(ES8388_REG_ADCCONTROL5,  0x02);  // ratio 256
    writeReg(ES8388_REG_ADCCONTROL8,  0x10);  // volume L
    writeReg(ES8388_REG_ADCCONTROL9,  0x10);  // volume R
    writeReg(ES8388_REG_ADCCONTROL10, 0x32);  // ALC off
    writeReg(ES8388_REG_ADCCONTROL11, 0xC0);
    writeReg(ES8388_REG_ADCCONTROL12, 0x05);
    writeReg(ES8388_REG_ADCCONTROL13, 0x06);
    writeReg(ES8388_REG_ADCCONTROL14, 0x42);  // HPF on

    // Configurazione DAC
    writeReg(ES8388_REG_DACCONTROL1,  0x18);  // 16bit I2S
    writeReg(ES8388_REG_DACCONTROL2,  0x02);  // ratio 256
    writeReg(ES8388_REG_DACCONTROL3,  0x00);  // no mute
    writeReg(ES8388_REG_DACCONTROL4,  0x00);  // volume L = 0dB
    writeReg(ES8388_REG_DACCONTROL5,  0x00);  // volume R = 0dB
    writeReg(ES8388_REG_DACCONTROL7,  0x00);
    writeReg(ES8388_REG_DACCONTROL17, 0xB8);  // MixerLeft
    writeReg(ES8388_REG_DACCONTROL20, 0xB8);  // MixerRight
    writeReg(ES8388_REG_DACCONTROL21, 0x80);  // slave mode
    writeReg(ES8388_REG_DACCONTROL23, 0x00);  // mono mix off
    writeReg(ES8388_REG_DACCONTROL24, 0x1E);  // LOUT1
    writeReg(ES8388_REG_DACCONTROL25, 0x1E);  // ROUT1
    writeReg(ES8388_REG_DACCONTROL26, 0x1E);  // LOUT2
    writeReg(ES8388_REG_DACCONTROL27, 0x1E);  // ROUT2

    configureI2S();

    Serial.println("[ES8388] Inizializzazione completata");
    return true;
}

// ======= Configurazione runtime =======

void ES8388::setSampleRate(uint32_t rate) {
    _sampleRate = rate;
    if (_i2sInitialized) {
        i2s_set_sample_rates(_i2sPort, rate);
    }
}

void ES8388::setBitsPerSample(uint8_t bits) {
    _bits = bits;
    // Reinizializza I2S se necessario
    if (_i2sInitialized) {
        configureI2S();
    }
}

void ES8388::setVolume(uint8_t volumePercent) {
    // Mappa 0-100% su 0-33 (registro DAC volume: 0=max, 33=min)
    uint8_t regVal = (uint8_t)(33 - (volumePercent * 33 / 100));
    writeReg(ES8388_REG_DACCONTROL24, regVal);
    writeReg(ES8388_REG_DACCONTROL25, regVal);
    writeReg(ES8388_REG_DACCONTROL26, regVal);
    writeReg(ES8388_REG_DACCONTROL27, regVal);
}

void ES8388::setInputGain(uint8_t gainPercent) {
    // Mappa 0-100% su 0-7 (3 bit PGA gain)
    uint8_t gain = gainPercent * 7 / 100;
    writeReg(ES8388_REG_ADCCONTROL8, (gain << 4) | gain);
    writeReg(ES8388_REG_ADCCONTROL9, (gain << 4) | gain);
}

void ES8388::setMute(bool muted) {
    writeReg(ES8388_REG_DACCONTROL3, muted ? 0x04 : 0x00);
}

// ======= Modalità operative =======

void ES8388::startCapture() {
    // ADC power up, DAC power down
    writeReg(ES8388_REG_ADCPOWER, 0x00);   // ADC L+R power on
    writeReg(ES8388_REG_DACPOWER, 0xC0);   // DAC power down
    Serial.println("[ES8388] Modalità cattura (ADC)");
}

void ES8388::startPlayback() {
    // DAC power up, ADC power down
    writeReg(ES8388_REG_DACPOWER, 0x3C);   // DAC L+R + OUT1 + OUT2 power on
    writeReg(ES8388_REG_ADCPOWER, 0xFF);   // ADC power down
    Serial.println("[ES8388] Modalità playback (DAC)");
}

// ======= I/O campioni =======

size_t ES8388::readSamples(int16_t* buf, size_t len) {
    if (!_i2sInitialized) return 0;
    size_t bytesRead = 0;
    size_t bytesToRead = len * sizeof(int16_t) * 2;  // stereo
    i2s_read(_i2sPort, buf, bytesToRead, &bytesRead, pdMS_TO_TICKS(10));
    return bytesRead / (sizeof(int16_t) * 2);
}

size_t ES8388::writeSamples(const int16_t* buf, size_t len) {
    if (!_i2sInitialized) return 0;
    size_t bytesWritten = 0;
    size_t bytesToWrite = len * sizeof(int16_t) * 2;  // stereo
    i2s_write(_i2sPort, buf, bytesToWrite, &bytesWritten, pdMS_TO_TICKS(10));
    return bytesWritten / (sizeof(int16_t) * 2);
}
