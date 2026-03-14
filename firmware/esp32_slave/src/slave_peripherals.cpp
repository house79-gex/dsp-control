#include "slave_peripherals.h"
#include "config.h"
#include <Adafruit_NeoPixel.h>

#if SLAVE_HAS_ENCODERS_LED

static Adafruit_NeoPixel s_ringVol(LED_RING_COUNT, LED_RING_VOL_PIN, NEO_GRB + NEO_KHZ800);
static Adafruit_NeoPixel s_ringBal(LED_RING_COUNT, LED_RING_BAL_PIN, NEO_GRB + NEO_KHZ800);

static int s_vol = 80, s_bal = 0;
static int8_t s_volDeltaSum = 0, s_balDeltaSum = 0;
static uint8_t s_btnState = 0;
static uint8_t s_lastVolAB = 0, s_lastBalAB = 0;

static const int8_t ENC_TABLE[16] = {
    0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0
};

static void poll_encoder(uint8_t clkPin, uint8_t dtPin, uint8_t& lastAB, int8_t& deltaSum) {
    uint8_t a = (uint8_t)digitalRead(clkPin);
    uint8_t b = (uint8_t)digitalRead(dtPin);
    uint8_t st = (a << 1) | b;
    uint8_t idx = (lastAB << 2) | st;
    deltaSum += ENC_TABLE[idx & 15];
    lastAB = st;
}

void slave_peripherals_init() {
    pinMode(ENCODER_VOL_A, INPUT_PULLUP);
    pinMode(ENCODER_VOL_B, INPUT_PULLUP);
    pinMode(ENCODER_VOL_BTN, INPUT_PULLUP);
    pinMode(ENCODER_BAL_A, INPUT_PULLUP);
    pinMode(ENCODER_BAL_B, INPUT_PULLUP);
    pinMode(ENCODER_BAL_BTN, INPUT_PULLUP);
    s_lastVolAB = ((uint8_t)digitalRead(ENCODER_VOL_A) << 1) | (uint8_t)digitalRead(ENCODER_VOL_B);
    s_lastBalAB = ((uint8_t)digitalRead(ENCODER_BAL_A) << 1) | (uint8_t)digitalRead(ENCODER_BAL_B);

    s_ringVol.begin();
    s_ringVol.setBrightness(LED_RING_BRIGHTNESS);
    s_ringBal.begin();
    s_ringBal.setBrightness(LED_RING_BRIGHTNESS);
    slave_peripherals_set_led(s_vol, s_bal);
    Serial.println("[SLAVE_PERIPH] Encoder + LED ring init");
}

void slave_peripherals_poll() {
    poll_encoder(ENCODER_VOL_A, ENCODER_VOL_B, s_lastVolAB, s_volDeltaSum);
    poll_encoder(ENCODER_BAL_A, ENCODER_BAL_B, s_lastBalAB, s_balDeltaSum);
    s_btnState = 0;
    if (digitalRead(ENCODER_VOL_BTN) == LOW) s_btnState |= 1;
    if (digitalRead(ENCODER_BAL_BTN) == LOW) s_btnState |= 2;
}

void slave_peripherals_set_led(int volPct, int bal) {
    s_vol = constrain(volPct, 0, 100);
    s_bal = constrain(bal, -50, 50);
    auto volColor = [&](int p) {
        if (p <= 50) return s_ringVol.Color((uint8_t)(p * 2 * 255 / 100), 255, 0);
        return s_ringVol.Color(255, (uint8_t)((100 - p) * 2 * 255 / 100), 0);
    };
    uint32_t c = volColor(s_vol);
    for (int i = 0; i < LED_RING_COUNT; i++) s_ringVol.setPixelColor(i, c);
    int nL = (LED_RING_COUNT / 2) * (50 - s_bal) / 50;
    int nR = (LED_RING_COUNT / 2) * (50 + s_bal) / 50;
    for (int i = 0; i < LED_RING_COUNT; i++) {
        if (i < LED_RING_COUNT / 2)
            s_ringBal.setPixelColor(i, i < nL ? s_ringBal.Color(0, 200, 255) : s_ringBal.Color(20, 20, 40));
        else
            s_ringBal.setPixelColor(i, (i - LED_RING_COUNT / 2) < nR ? s_ringBal.Color(255, 200, 0) : s_ringBal.Color(20, 20, 40));
    }
    s_ringVol.show();
    s_ringBal.show();
}

void slave_peripherals_set_relay(bool on) {
    digitalWrite(RELAY_GPIO, on ? HIGH : LOW);
}

void slave_peripherals_get_state(int8_t& volDelta, int8_t& balDelta, uint8_t& buttons) {
    volDelta = s_volDeltaSum;
    balDelta = s_balDeltaSum;
    buttons = s_btnState;
    s_volDeltaSum = 0;
    s_balDeltaSum = 0;
}

#else

void slave_peripherals_init() {}
void slave_peripherals_poll() {}
void slave_peripherals_set_led(int, int) {}
void slave_peripherals_set_relay(bool on) { digitalWrite(RELAY_GPIO, on ? HIGH : LOW); }
void slave_peripherals_get_state(int8_t& v, int8_t& b, uint8_t& btn) { v = b = 0; btn = 0; }

#endif
