#pragma once
#include <Arduino.h>

void slave_peripherals_init();
void slave_peripherals_poll();
void slave_peripherals_set_led(int volPct, int bal);
void slave_peripherals_set_relay(bool on);
void slave_peripherals_get_state(int8_t& volDelta, int8_t& balDelta, uint8_t& buttons);
