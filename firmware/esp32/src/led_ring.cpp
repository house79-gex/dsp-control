#include "led_ring.h"
#include "config.h"
#include <Adafruit_NeoPixel.h>

// ===== Oggetti NeoPixel =====
static Adafruit_NeoPixel s_ringVol(LED_RING_COUNT, LED_RING_VOL_PIN, NEO_GRB + NEO_KHZ800);
static Adafruit_NeoPixel s_ringBal(LED_RING_COUNT, LED_RING_BAL_PIN, NEO_GRB + NEO_KHZ800);

// Stato corrente e target (per smooth transition)
static int  s_volume        = 80;
static int  s_balance       = 0;
static int  s_volumeTarget  = 80;
static int  s_balanceTarget = 0;
static bool s_forceRedraw   = false;  // Forza ridisegno dopo effetto pulse

static const float SMOOTH_FACTOR = 0.3f;  // 0.0-1.0 (più alto = più veloce)

// Effetto pulse (per feedback click encoder button)
static bool     s_pulseActive = false;
static uint8_t  s_pulseR = 0, s_pulseG = 0, s_pulseB = 0;
static uint32_t s_pulseStart = 0;
static const uint32_t PULSE_DURATION_MS = 200;

// ===== Utilità colore volume (verde→giallo→rosso) =====
static uint32_t volume_to_color(int pct, const Adafruit_NeoPixel& ring) {
    if (pct <= 50) {
        uint8_t r = (uint8_t)(pct * 2 * 255 / 100);
        return ring.Color(r, 255, 0);  // Verde → Giallo
    } else {
        uint8_t g = (uint8_t)((100 - pct) * 2 * 255 / 100);
        return ring.Color(255, g, 0);  // Giallo → Rosso
    }
}

// ===== Init =====
void led_ring_init() {
    s_ringVol.begin();
    s_ringVol.setBrightness(LED_RING_BRIGHTNESS);
    s_ringVol.clear();
    s_ringVol.show();

    s_ringBal.begin();
    s_ringBal.setBrightness(LED_RING_BRIGHTNESS);
    s_ringBal.clear();
    s_ringBal.show();

    Serial.printf("[LED_RING] LED ring WS2812B inizializzati (%d LED, brightness %d)\n",
                  LED_RING_COUNT, LED_RING_BRIGHTNESS);
}

// ===== Set Volume (con smooth transition) =====
void led_ring_set_volume(int percent) {
    s_volumeTarget = constrain(percent, 0, 100);
}

// ===== Set Balance (con smooth transition) =====
void led_ring_set_balance(int balance) {
    s_balanceTarget = constrain(balance, -50, 50);
}

// ===== Update Loop (chiamare ogni loop ~5-20ms) =====
void led_ring_update() {
    // Effetto pulse (ha priorità sulla visualizzazione normale)
    if (s_pulseActive) {
        uint32_t elapsed = millis() - s_pulseStart;
        if (elapsed >= PULSE_DURATION_MS) {
            s_pulseActive = false;
            s_forceRedraw = true;  // Ripristina visualizzazione normale al prossimo update
        } else {
            uint8_t fade = (uint8_t)(255 * (PULSE_DURATION_MS - elapsed) / PULSE_DURATION_MS);
            for (int i = 0; i < LED_RING_COUNT; i++) {
                s_ringVol.setPixelColor(i, s_ringVol.Color(
                    (uint8_t)((s_pulseR * fade) / 255),
                    (uint8_t)((s_pulseG * fade) / 255),
                    (uint8_t)((s_pulseB * fade) / 255)));
                s_ringBal.setPixelColor(i, s_ringBal.Color(
                    (uint8_t)((s_pulseR * fade) / 255),
                    (uint8_t)((s_pulseG * fade) / 255),
                    (uint8_t)((s_pulseB * fade) / 255)));
            }
            s_ringVol.show();
            s_ringBal.show();
            return;
        }
    }

    // Smooth transition volume
    if (s_forceRedraw || s_volume != s_volumeTarget) {
        if (!s_forceRedraw) {
            float delta = (float)(s_volumeTarget - s_volume) * SMOOTH_FACTOR;
            s_volume += (int)delta;
            if (abs(s_volume - s_volumeTarget) < 2) s_volume = s_volumeTarget;
        }

        s_ringVol.clear();
        int numLit = (s_volume * LED_RING_COUNT) / 100;
        uint32_t col = volume_to_color(s_volume, s_ringVol);
        for (int i = 0; i < numLit; i++) {
            s_ringVol.setPixelColor(i, col);
        }
        s_ringVol.show();
    }

    // Smooth transition balance
    if (s_forceRedraw || s_balance != s_balanceTarget) {
        if (!s_forceRedraw) {
            float delta = (float)(s_balanceTarget - s_balance) * SMOOTH_FACTOR;
            s_balance += (int)delta;
            if (abs(s_balance - s_balanceTarget) < 2) s_balance = s_balanceTarget;
        }

        s_ringBal.clear();
        int center    = LED_RING_COUNT / 2;
        int deviation = (s_balance * center) / 50;

        if (deviation == 0) {
            // Centro: 2 LED centrali ciano
            s_ringBal.setPixelColor(center - 1, s_ringBal.Color(0, 200, 255));
            s_ringBal.setPixelColor(center,     s_ringBal.Color(0, 200, 255));
        } else if (deviation < 0) {
            // Sinistra: LED blu progressivi
            for (int i = center + deviation; i < center; i++) {
                if (i >= 0) {
                    uint8_t brightness = (uint8_t)(255 * (center - i) / (-deviation));
                    s_ringBal.setPixelColor(i, s_ringBal.Color(0, brightness, 255));
                }
            }
        } else {
            // Destra: LED arancione progressivi
            for (int i = center; i < center + deviation; i++) {
                if (i < LED_RING_COUNT) {
                    uint8_t brightness = (uint8_t)(255 * (i - center + 1) / deviation);
                    s_ringBal.setPixelColor(i, s_ringBal.Color(255, brightness / 2, 0));
                }
            }
        }
        s_ringBal.show();
    }
    s_forceRedraw = false;
}

// ===== Effetto Pulse (per feedback visivo click encoder button) =====
void led_ring_effect_pulse(uint8_t r, uint8_t g, uint8_t b) {
    s_pulseActive = true;
    s_pulseR = r;
    s_pulseG = g;
    s_pulseB = b;
    s_pulseStart = millis();
}
