#include "rs485_rx.h"
#include "config.h"
#include <Arduino.h>

static uint32_t s_framesForwarded = 0;

void rs485_rx_init() {
    // Inizializza UART1 per RS-485
    Serial1.begin(57600, SERIAL_8N1, RS485_RX_PIN, RS485_TX_PIN);
    pinMode(RS485_DE_PIN, OUTPUT);
    digitalWrite(RS485_DE_PIN, LOW);  // Ricezione di default
    Serial.println("[RS485_RX] UART RS-485 inizializzato");
}

void rs485_rx_forward(const uint8_t* data, uint8_t len) {
    if (!data || len == 0) return;

    // Abilita trasmissione
    digitalWrite(RS485_DE_PIN, HIGH);
    delayMicroseconds(10);

    Serial1.write(data, len);
    Serial1.flush();

    // Disabilita trasmissione (torna in ricezione)
    delayMicroseconds(10);
    digitalWrite(RS485_DE_PIN, LOW);

    s_framesForwarded++;
}

uint32_t rs485_rx_get_frames_forwarded() { return s_framesForwarded; }
