#pragma once
#include <Arduino.h>
#include <cstdint>

// ======= Modulo RX – Output DMX512 =======
// Ricostruisce buffer DMX 512 canali dai chunk wireless
// Genera frame DMX512 completo (BREAK + MAB + START_CODE + 512ch) @ 40Hz

#define DMX_NUM_CHANNELS  512

// Inizializza UART2 per DMX512 output
void dmx_rx_init();

// Aggiunge chunk DMX al buffer locale
// startCh: canale iniziale (0-511), numCh: numero canali nel chunk
void dmx_rx_apply_chunk(uint16_t startCh, const uint8_t* dmxData, uint8_t numCh);

// Task FreeRTOS generazione DMX (da registrare in main)
// Genera frame DMX512 ogni 25ms (40Hz)
void dmx_rx_task(void* param);

// Statistiche
uint32_t dmx_rx_get_frames_sent();
