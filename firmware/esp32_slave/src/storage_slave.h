#pragma once
#include <Arduino.h>

// NVS storage per preset DSP, scene DMX e config WLED
// Questo ESP32 (#2 Slave) gestisce tutta la persistenza del sistema

void storage_slave_init();
bool storage_slave_save(const char* key, const uint8_t* data, size_t len);
bool storage_slave_load(const char* key, uint8_t* data, size_t maxLen, size_t& outLen);
void storage_slave_clear(const char* key);
void storage_slave_clear_all();
