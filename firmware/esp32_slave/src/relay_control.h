#pragma once
#include <Arduino.h>

// Relay DPDT e uscita strobo
// GPIO diretti su ESP32 #2 (diverso da Master che usa TCA9535 expander)

void relay_control_init();
void relay_set(bool on);        // Relay generale (RELAY_GPIO)
void strobe_set(bool on);       // Strobo (STROBE_GPIO)
bool relay_get_state();
bool strobe_get_state();

// Strobo audio-reactive con isteresi
bool strobe_trigger_from_bass(float bassLevel);
