#pragma once
#include <Arduino.h>
#include <cstdint>

// ======= Modulo RX – Forward RS-485 verso CQ260D =======
// Riceve frame RS-485 wireless e li invia al DSP via UART

// Inizializza UART RS-485 + DE pin
void rs485_rx_init();

// Invia frame RS-485 al CQ260D
// data: payload, len: lunghezza in byte
void rs485_rx_forward(const uint8_t* data, uint8_t len);

// Statistiche
uint32_t rs485_rx_get_frames_forwarded();
