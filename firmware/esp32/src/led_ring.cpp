#include "led_ring.h"
#include <Adafruit_NeoPixel.h>

// ======= Oggetti NeoPixel =======
static Adafruit_NeoPixel s_ringVol(LED_RING_COUNT, LED_RING_VOL_PIN, NEO_GRB + NEO_KHZ800);
static Adafruit_NeoPixel s_ringBal(LED_RING_COUNT, LED_RING_BAL_PIN, NEO_GRB + NEO_KHZ800);

// Stato encoder e valori correnti
static int s_volume  = 80;   // 0-100
static int s_balance = 0;    // -50..+50

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

// Stato ISR encoder (IRAM per accesso rapido da interrupt)
static volatile int32_t s_encVolISRDelta = 0;
static volatile int32_t s_encBalISRDelta = 0;
static volatile uint8_t s_encVolLastState = 0;
static volatile uint8_t s_encBalLastState = 0;

// Tabella di transizione per decodifica encoder quadraturale (DRAM per accesso da ISR)
static const DRAM_ATTR int8_t ENC_TABLE[16] = {
    0, -1,  1,  0,
    1,  0,  0, -1,
   -1,  0,  0,  1,
    0,  1, -1,  0
};

void IRAM_ATTR encoder_vol_isr() {
    uint8_t clk = digitalRead(ENCODER_VOL_A);
    uint8_t dt  = digitalRead(ENCODER_VOL_B);
    uint8_t state = (clk << 1) | dt;
    uint8_t idx   = (s_encVolLastState << 2) | state;
    s_encVolISRDelta += ENC_TABLE[idx & 0x0F];
    s_encVolLastState = state;
}

void IRAM_ATTR encoder_bal_isr() {
    uint8_t clk = digitalRead(ENCODER_BAL_A);
    uint8_t dt  = digitalRead(ENCODER_BAL_B);
    uint8_t state = (clk << 1) | dt;
    uint8_t idx   = (s_encBalLastState << 2) | state;
    s_encBalISRDelta += ENC_TABLE[idx & 0x0F];
    s_encBalLastState = state;
}

void encoder_init() {
    pinMode(ENCODER_VOL_A, INPUT_PULLUP);
    pinMode(ENCODER_VOL_B, INPUT_PULLUP);
    pinMode(ENCODER_BAL_A, INPUT_PULLUP);
    pinMode(ENCODER_BAL_B, INPUT_PULLUP);

    // Stato iniziale encoder
    s_encVolLastState = (uint8_t)((digitalRead(ENCODER_VOL_A) << 1) | digitalRead(ENCODER_VOL_B));
    s_encBalLastState = (uint8_t)((digitalRead(ENCODER_BAL_A) << 1) | digitalRead(ENCODER_BAL_B));

    // Attacca interrupt su entrambi i fronti di CLK e DT
    attachInterrupt(digitalPinToInterrupt(ENCODER_VOL_A), encoder_vol_isr, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_VOL_B), encoder_vol_isr, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_BAL_A), encoder_bal_isr, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_BAL_B), encoder_bal_isr, CHANGE);

    Serial.println("[ENCODER] Encoder rotativi inizializzati (interrupt-based)");
}

void encoder_tick() {
    // Leggi delta ISR atomicamente e azzera
    noInterrupts();
    int32_t volDelta = s_encVolISRDelta;
    int32_t balDelta = s_encBalISRDelta;
    s_encVolISRDelta = 0;
    s_encBalISRDelta = 0;
    interrupts();

    // Applica delta volume (ogni 4 passi quadraturali = 1 click fisico)
    if (abs(volDelta) >= 4) {
        int steps = (int)(volDelta / 4);
        s_volume = constrain(s_volume + steps, 0, 100);
        led_ring_set_volume(s_volume);
        // Ri-accumula i passi rimanenti (modulo 4)
        noInterrupts();
        s_encVolISRDelta += (volDelta % 4);
        interrupts();
    }

    // Applica delta balance
    if (abs(balDelta) >= 4) {
        int steps = (int)(balDelta / 4);
        s_balance = constrain(s_balance + steps, -50, 50);
        led_ring_set_balance(s_balance);
        noInterrupts();
        s_encBalISRDelta += (balDelta % 4);
        interrupts();
    }
}

int encoder_get_volume()  { return s_volume; }
int encoder_get_balance() { return s_balance; }
