#include "dmx_rx.h"
#include "config.h"
#include <Arduino.h>
#include <string.h>

// Timing DMX512 (ANSI E1.11)
#define DMX_BREAK_DURATION_US  92   // BREAK: >88µs LOW
#define DMX_MAB_DURATION_US    12   // MAB (Mark After Break): >8µs HIGH

// Buffer DMX 512 canali
static uint8_t  s_dmxBuf[DMX_NUM_CHANNELS] = {};
static uint32_t s_framesSent = 0;

void dmx_rx_init() {
    // UART2 @ 250000 baud, 8 bit, 2 stop bit per DMX512
    Serial2.begin(250000, SERIAL_8N2, -1, DMX_TX_PIN);
    pinMode(DMX_DE_PIN, OUTPUT);
    digitalWrite(DMX_DE_PIN, HIGH);  // Sempre in trasmissione
    memset(s_dmxBuf, 0, sizeof(s_dmxBuf));
    Serial.println("[DMX_RX] Output DMX512 inizializzato");
}

void dmx_rx_apply_chunk(uint16_t startCh, const uint8_t* dmxData, uint8_t numCh) {
    if (!dmxData || numCh == 0) return;
    for (uint8_t i = 0; i < numCh; i++) {
        uint16_t ch = startCh + i;
        if (ch < DMX_NUM_CHANNELS) {
            s_dmxBuf[ch] = dmxData[i];
        }
    }
}

// ——— Generazione frame DMX512 ———
// Struttura: BREAK (88µs LOW) + MAB (8µs HIGH) + START CODE (0x00) + 512 byte

static void send_dmx_frame() {
    // BREAK: pilota linea bassa per DMX_BREAK_DURATION_US µs
    Serial2.end();
    pinMode(DMX_TX_PIN, OUTPUT);
    digitalWrite(DMX_TX_PIN, LOW);
    delayMicroseconds(DMX_BREAK_DURATION_US);
    digitalWrite(DMX_TX_PIN, HIGH);
    delayMicroseconds(DMX_MAB_DURATION_US);      // MAB
    Serial2.begin(250000, SERIAL_8N2, -1, DMX_TX_PIN);

    // START CODE (0x00) + 512 byte dati
    Serial2.write(0x00);        // Start Code
    Serial2.write(s_dmxBuf, DMX_NUM_CHANNELS);
    Serial2.flush();

    s_framesSent++;
}

void dmx_rx_task(void* param) {
    Serial.println("[DMX_RX] Task DMX output avviato");
    TickType_t lastWake = xTaskGetTickCount();

    for (;;) {
        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(25));  // 40Hz
        send_dmx_frame();
    }
}

uint32_t dmx_rx_get_frames_sent() { return s_framesSent; }
