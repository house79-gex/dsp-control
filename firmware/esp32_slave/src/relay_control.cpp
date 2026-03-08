#include "relay_control.h"
#include "config.h"

static bool s_relayState  = false;
static bool s_strobeState = false;

void relay_control_init() {
    pinMode(RELAY_GPIO,  OUTPUT);
    pinMode(STROBE_GPIO, OUTPUT);
    digitalWrite(RELAY_GPIO,  LOW);
    digitalWrite(STROBE_GPIO, LOW);
    Serial.printf("[RELAY] Inizializzato (RELAY=GPIO%d, STROBE=GPIO%d)\n",
                  RELAY_GPIO, STROBE_GPIO);
}

void relay_set(bool on) {
    s_relayState = on;
    digitalWrite(RELAY_GPIO, on ? HIGH : LOW);
}

void strobe_set(bool on) {
    s_strobeState = on;
    digitalWrite(STROBE_GPIO, on ? HIGH : LOW);
}

bool relay_get_state() {
    return s_relayState;
}

bool strobe_get_state() {
    return s_strobeState;
}

bool strobe_trigger_from_bass(float bassLevel) {
    // Isteresi: ON a 0.7, OFF a 0.5 (evita flicker)
    const float kThreshOn  = 0.7f;
    const float kThreshOff = 0.5f;

    if (!s_strobeState && bassLevel > kThreshOn) {
        strobe_set(true);
    } else if (s_strobeState && bassLevel < kThreshOff) {
        strobe_set(false);
    }
    return s_strobeState;
}
