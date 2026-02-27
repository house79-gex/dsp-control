#include "led_ring.h"
#include <Adafruit_NeoPixel.h>

// ======= Oggetti NeoPixel =======
static Adafruit_NeoPixel s_ringVol(LED_RING_COUNT, LED_RING_VOL_PIN, NEO_GRB + NEO_KHZ800);
static Adafruit_NeoPixel s_ringBal(LED_RING_COUNT, LED_RING_BAL_PIN, NEO_GRB + NEO_KHZ800);

// Stato encoder e valori correnti
static int s_volume  = 80;   // 0-100
static int s_balance = 0;    // -50..+50

// Stato encoder (per decodifica incrementale)
static volatile int s_encVolDelta  = 0;
static volatile int s_encBalDelta  = 0;
static int8_t s_prevVolAB = 0;
static int8_t s_prevBalAB = 0;

// Effetto pulse
static bool     s_pulseActive = false;
static uint8_t  s_pulseR = 0, s_pulseG = 0, s_pulseB = 0;
static uint32_t s_pulseStart = 0;
static const uint32_t PULSE_DURATION_MS = 300;

// Converte percentuale (0-100) in colore RGB (verde→giallo→rosso)
static uint32_t volume_to_color(int pct, const Adafruit_NeoPixel& ring) {
    if (pct <= 50) {
        // Verde → Giallo
        uint8_t r = (uint8_t)(pct * 2 * 255 / 100);
        return ring.Color(r, 255, 0);
    } else {
        // Giallo → Rosso
        uint8_t g = (uint8_t)((100 - pct) * 2 * 255 / 100);
        return ring.Color(255, g, 0);
    }
}

void led_ring_init() {
    s_ringVol.begin();
    s_ringVol.setBrightness(80);
    s_ringVol.clear();
    s_ringVol.show();

    s_ringBal.begin();
    s_ringBal.setBrightness(80);
    s_ringBal.clear();
    s_ringBal.show();

    Serial.println("[LED_RING] LED ring inizializzati");
}

void led_ring_set_volume(int percent) {
    s_volume = constrain(percent, 0, 100);
    s_ringVol.clear();

    // Numero di LED accesi proporzionale al volume
    int numLit = (s_volume * LED_RING_COUNT) / 100;
    uint32_t col = volume_to_color(s_volume, s_ringVol);
    for (int i = 0; i < numLit; i++) {
        s_ringVol.setPixelColor(i, col);
    }
    s_ringVol.show();
}

void led_ring_set_balance(int balance) {
    s_balance = constrain(balance, -50, 50);
    s_ringBal.clear();

    // Centro = LED_RING_COUNT/2, sinistra = meno, destra = più
    int center = LED_RING_COUNT / 2;
    int deviation = (balance * center) / 50;  // Numero LED da accendere a sx o dx

    if (deviation == 0) {
        // Centro: accendi LED centrali
        s_ringBal.setPixelColor(center - 1, s_ringBal.Color(0, 200, 255));
        s_ringBal.setPixelColor(center,     s_ringBal.Color(0, 200, 255));
    } else if (deviation < 0) {
        // Sinistra: accendi LED a sinistra del centro
        for (int i = center + deviation; i < center; i++) {
            if (i >= 0) s_ringBal.setPixelColor(i, s_ringBal.Color(100, 100, 255));
        }
    } else {
        // Destra: accendi LED a destra del centro
        for (int i = center; i < center + deviation; i++) {
            if (i < LED_RING_COUNT) s_ringBal.setPixelColor(i, s_ringBal.Color(255, 100, 100));
        }
    }
    s_ringBal.show();
}

void led_ring_effect_pulse(uint8_t r, uint8_t g, uint8_t b) {
    s_pulseR = r; s_pulseG = g; s_pulseB = b;
    s_pulseStart = millis();
    s_pulseActive = true;
}

void led_ring_update() {
    if (!s_pulseActive) return;

    uint32_t elapsed = millis() - s_pulseStart;
    if (elapsed >= PULSE_DURATION_MS) {
        s_pulseActive = false;
        // Ripristina visualizzazione normale
        led_ring_set_volume(s_volume);
        led_ring_set_balance(s_balance);
        return;
    }

    // Effetto fade: luminosità massima a metà, zero agli estremi
    float t = (float)elapsed / PULSE_DURATION_MS;
    float brightness = (t < 0.5f) ? (t * 2.0f) : ((1.0f - t) * 2.0f);
    uint8_t br = (uint8_t)(brightness * 255);

    for (int i = 0; i < LED_RING_COUNT; i++) {
        s_ringVol.setPixelColor(i, s_ringVol.Color(
            (s_pulseR * br) >> 8,
            (s_pulseG * br) >> 8,
            (s_pulseB * br) >> 8));
    }
    s_ringVol.show();
}

// ======= Encoder rotativi =======

void encoder_init() {
    pinMode(ENCODER_VOL_A, INPUT_PULLUP);
    pinMode(ENCODER_VOL_B, INPUT_PULLUP);
    pinMode(ENCODER_BAL_A, INPUT_PULLUP);
    pinMode(ENCODER_BAL_B, INPUT_PULLUP);
    s_prevVolAB = (digitalRead(ENCODER_VOL_A) << 1) | digitalRead(ENCODER_VOL_B);
    s_prevBalAB = (digitalRead(ENCODER_BAL_A) << 1) | digitalRead(ENCODER_BAL_B);
    Serial.println("[ENCODER] Encoder rotativi inizializzati");
}

// Tabella di transizione per decodifica encoder quadraturale
static const int8_t ENC_TABLE[16] = {
    0, -1,  1,  0,
    1,  0,  0, -1,
   -1,  0,  0,  1,
    0,  1, -1,  0
};

void encoder_tick() {
    // Encoder volume
    int8_t volAB = (digitalRead(ENCODER_VOL_A) << 1) | digitalRead(ENCODER_VOL_B);
    int8_t volIdx = (s_prevVolAB << 2) | volAB;
    s_encVolDelta += ENC_TABLE[volIdx & 0x0F];
    s_prevVolAB = volAB;

    // Encoder balance
    int8_t balAB = (digitalRead(ENCODER_BAL_A) << 1) | digitalRead(ENCODER_BAL_B);
    int8_t balIdx = (s_prevBalAB << 2) | balAB;
    s_encBalDelta += ENC_TABLE[balIdx & 0x0F];
    s_prevBalAB = balAB;

    // Applica delta (ogni 4 passi = 1 click)
    if (s_encVolDelta >= 4) {
        s_volume = constrain(s_volume + 1, 0, 100);
        s_encVolDelta -= 4;
        led_ring_set_volume(s_volume);
    } else if (s_encVolDelta <= -4) {
        s_volume = constrain(s_volume - 1, 0, 100);
        s_encVolDelta += 4;
        led_ring_set_volume(s_volume);
    }

    if (s_encBalDelta >= 4) {
        s_balance = constrain(s_balance + 1, -50, 50);
        s_encBalDelta -= 4;
        led_ring_set_balance(s_balance);
    } else if (s_encBalDelta <= -4) {
        s_balance = constrain(s_balance - 1, -50, 50);
        s_encBalDelta += 4;
        led_ring_set_balance(s_balance);
    }
}

int encoder_get_volume()  { return s_volume; }
int encoder_get_balance() { return s_balance; }
