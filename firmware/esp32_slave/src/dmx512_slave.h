#pragma once
#include <Arduino.h>

// DMX512 output via UART2
// Aggiornamento continuo a DMX_UPDATE_RATE_HZ (40 Hz)

void dmx_slave_init();
void dmx_slave_set_channel(uint16_t channel, uint8_t value);   // channel: 1-512
void dmx_slave_set_channels(uint8_t startCh, const uint8_t* values, uint8_t count);
void dmx_slave_set_scene(uint8_t sceneIndex);
void dmx_slave_task(void* pvParameters);  // Task FreeRTOS Core 1
uint32_t dmx_slave_get_fps();
