#include "relay_control.h"
#include "config.h"

static bool s_relayState  = false;
static bool s_strobeState = false;

// Soglie isteresi per il trigger strobo audio-reactive
// ON quando bassLevel > kStrobeThreshOn, OFF quando bassLevel < kStrobeThreshOff
static const float kStrobeThreshOn  = 0.7f;   // 70% della scala 0.0-1.0
static const float kStrobeThreshOff = 0.5f;   // 50% (isteresi 20%)

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
    if (!s_strobeState && bassLevel > kStrobeThreshOn) {
        strobe_set(true);
    } else if (s_strobeState && bassLevel < kStrobeThreshOff) {
        strobe_set(false);
    }
    return s_strobeState;
}
