#include "dmx512_slave.h"
#include "config.h"
#include <HardwareSerial.h>

// Buffer DMX (512 canali + 1 start code)
static uint8_t  s_dmxBuf[DMX_UNIVERSE_SIZE + 1] = {};  // [0] = start code (0x00)
static portMUX_TYPE s_dmxMux = portMUX_INITIALIZER_UNLOCKED;

// Statistiche
static uint32_t s_frameCount = 0;
static uint32_t s_lastFpsTs  = 0;
static uint32_t s_fps        = 0;

// Scene predefinite (placeholder – customizzabile via IPC CMD_STORAGE_LOAD_SCENE)
#define NUM_BUILTIN_SCENES  4
static const uint8_t kScenes[NUM_BUILTIN_SCENES][8] = {
    { 0,   0,   0,   0,   0,   0,   0,   0   },   // Scena 0: Spento
    { 255, 0,   0,   255, 0,   0,   0,   255 },    // Scena 1: Rosso
    { 0,   255, 0,   255, 0,   0,   255, 0   },    // Scena 2: Verde
    { 0,   0,   255, 255, 255, 255, 255, 255 },    // Scena 3: Blu full
};

static HardwareSerial s_dmxSerial(DMX_UART_NUM);

void dmx_slave_init() {
    // DE sempre HIGH (trasmissione)
    pinMode(DMX_DE, OUTPUT);
    digitalWrite(DMX_DE, HIGH);

    // UART configurato a 250000 baud, 8N2 (standard DMX512)
    s_dmxSerial.begin(250000, SERIAL_8N2, -1, DMX_TX);
    s_dmxBuf[0] = 0x00;  // Start code DMX

    Serial.printf("[DMX_SLAVE] Inizializzato (TX=GPIO%d, DE=GPIO%d, %d canali @ %d Hz)\n",
                  DMX_TX, DMX_DE, DMX_UNIVERSE_SIZE, DMX_UPDATE_RATE_HZ);
}

void dmx_slave_set_channel(uint16_t channel, uint8_t value) {
    if (channel < 1 || channel > DMX_UNIVERSE_SIZE) return;
    portENTER_CRITICAL(&s_dmxMux);
    s_dmxBuf[channel] = value;
    portEXIT_CRITICAL(&s_dmxMux);
}

void dmx_slave_set_channels(uint8_t startCh, const uint8_t* values, uint8_t count) {
    if (startCh < 1 || values == nullptr) return;
    portENTER_CRITICAL(&s_dmxMux);
    for (int i = 0; i < count && (startCh + i) <= DMX_UNIVERSE_SIZE; i++) {
        s_dmxBuf[startCh + i] = values[i];
    }
    portEXIT_CRITICAL(&s_dmxMux);
}

void dmx_slave_set_scene(uint8_t sceneIndex) {
    if (sceneIndex >= NUM_BUILTIN_SCENES) {
        Serial.printf("[DMX_SLAVE] Scena %d non trovata\n", sceneIndex);
        return;
    }
    portENTER_CRITICAL(&s_dmxMux);
    for (int i = 0; i < 8; i++) {
        s_dmxBuf[1 + i] = kScenes[sceneIndex][i];
    }
    portEXIT_CRITICAL(&s_dmxMux);
    Serial.printf("[DMX_SLAVE] Scena %d applicata\n", sceneIndex);
}

uint32_t dmx_slave_get_fps() {
    return s_fps;
}

// Task FreeRTOS: invia frame DMX a DMX_UPDATE_RATE_HZ
void dmx_slave_task(void* pvParameters) {
    const TickType_t period = pdMS_TO_TICKS(1000 / DMX_UPDATE_RATE_HZ);
    TickType_t lastWake = xTaskGetTickCount();

    while (true) {
        // Copia buffer DMX con lock
        uint8_t txBuf[DMX_UNIVERSE_SIZE + 1];
        portENTER_CRITICAL(&s_dmxMux);
        memcpy(txBuf, s_dmxBuf, sizeof(txBuf));
        portEXIT_CRITICAL(&s_dmxMux);

        // Break (88µs) + Mark After Break (8µs) + Start Code + 512 slot
        s_dmxSerial.flush();
        s_dmxSerial.end();
        // Genera Break: 88µs @ LOW (250kbaud = 4µs/bit → 22 bit LOW)
        pinMode(DMX_TX, OUTPUT);
        digitalWrite(DMX_TX, LOW);
        delayMicroseconds(88);
        digitalWrite(DMX_TX, HIGH);
        delayMicroseconds(8);
        // Reinizializza UART e invia frame
        s_dmxSerial.begin(250000, SERIAL_8N2, -1, DMX_TX);
        s_dmxSerial.write(txBuf, DMX_UNIVERSE_SIZE + 1);

        // Calcolo FPS
        s_frameCount++;
        uint32_t now = millis();
        if (now - s_lastFpsTs >= 1000) {
            s_fps = s_frameCount;
            s_frameCount = 0;
            s_lastFpsTs  = now;
        }

        vTaskDelayUntil(&lastWake, period);
    }
}
