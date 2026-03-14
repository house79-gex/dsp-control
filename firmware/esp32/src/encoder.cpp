#include "encoder.h"
#include "config.h"
#include "led_ring.h"
#if USE_SLAVE_PERIPHERALS
#include "ipc_master.h"
#include "storage.h"
#endif
static bool s_useSlaveEnc = false;

// ===== Stato encoder volume =====
static volatile int32_t s_encVolISRDelta = 0;
static volatile uint8_t s_encVolLastState = 0;
static volatile bool    s_volBtnISRState = false;
static volatile bool    s_volBtnChanged = false;
static volatile uint32_t s_volBtnDebounce = 0;
static int s_volValue = 80;  // 0-100 (default 80%)
static bool s_volBtnPressed = false;

// ===== Stato encoder balance =====
static volatile int32_t s_encBalISRDelta = 0;
static volatile uint8_t s_encBalLastState = 0;
static volatile bool    s_balBtnISRState = false;
static volatile bool    s_balBtnChanged = false;
static volatile uint32_t s_balBtnDebounce = 0;
static int s_balValue = 0;  // -50..+50 (default centro)
static bool s_balBtnPressed = false;

// ===== Callback =====
static EncoderCallback s_volChangeCb = nullptr;
static ButtonCallback  s_volButtonCb = nullptr;
static EncoderCallback s_balChangeCb = nullptr;
static ButtonCallback  s_balButtonCb = nullptr;

// ===== Tabella transizione quadraturale (DRAM per accesso rapido da ISR) =====
// State machine 2-bit: AB (00, 01, 10, 11)
// CW:  00 → 01 → 11 → 10 → 00
// CCW: 00 → 10 → 11 → 01 → 00
static const DRAM_ATTR int8_t ENC_TABLE[16] = {
    0, -1,  1,  0,
    1,  0,  0, -1,
   -1,  0,  0,  1,
    0,  1, -1,  0
};

// ===== ISR Encoder Volume =====
static void IRAM_ATTR isr_vol_encoder() {
    uint8_t clk = (uint8_t)digitalRead(ENCODER_VOL_A);
    uint8_t dt  = (uint8_t)digitalRead(ENCODER_VOL_B);
    uint8_t state = (clk << 1) | dt;
    uint8_t idx   = (s_encVolLastState << 2) | state;
    s_encVolISRDelta += ENC_TABLE[idx & 0x0F];
    s_encVolLastState = state;
}

// ===== ISR Pulsante Volume (con debounce software) =====
static void IRAM_ATTR isr_vol_button() {
    uint32_t now = millis();
    if (now - s_volBtnDebounce < 50) return;  // Debounce 50ms
    s_volBtnDebounce = now;
    s_volBtnISRState = (digitalRead(ENCODER_VOL_BTN) == LOW);  // Pullup: LOW=pressed
    s_volBtnChanged = true;
}

// ===== ISR Encoder Balance =====
static void IRAM_ATTR isr_bal_encoder() {
    uint8_t clk = (uint8_t)digitalRead(ENCODER_BAL_A);
    uint8_t dt  = (uint8_t)digitalRead(ENCODER_BAL_B);
    uint8_t state = (clk << 1) | dt;
    uint8_t idx   = (s_encBalLastState << 2) | state;
    s_encBalISRDelta += ENC_TABLE[idx & 0x0F];
    s_encBalLastState = state;
}

// ===== ISR Pulsante Balance =====
static void IRAM_ATTR isr_bal_button() {
    uint32_t now = millis();
    if (now - s_balBtnDebounce < 50) return;
    s_balBtnDebounce = now;
    s_balBtnISRState = (digitalRead(ENCODER_BAL_BTN) == LOW);
    s_balBtnChanged = true;
}

// ===== Init =====
void encoder_init() {
#if USE_SLAVE_PERIPHERALS
    s_useSlaveEnc = storage_ipc_slave_available();
    if (s_useSlaveEnc) {
        led_ring_init();
        ipc_set_led_ring((uint8_t)s_volValue, (int8_t)s_balValue);
        Serial.println("[ENCODER] Periferiche su Slave (IPC)");
        return;
    }
#endif
    pinMode(ENCODER_VOL_A,   INPUT_PULLUP);
    pinMode(ENCODER_VOL_B,   INPUT_PULLUP);
    pinMode(ENCODER_VOL_BTN, INPUT_PULLUP);

    // Configura GPIO encoder balance
    pinMode(ENCODER_BAL_A,   INPUT_PULLUP);
    pinMode(ENCODER_BAL_B,   INPUT_PULLUP);
    pinMode(ENCODER_BAL_BTN, INPUT_PULLUP);

    // Leggi stato iniziale encoder (evita falso delta al primo interrupt)
    s_encVolLastState = (uint8_t)((digitalRead(ENCODER_VOL_A) << 1) | digitalRead(ENCODER_VOL_B));
    s_encBalLastState = (uint8_t)((digitalRead(ENCODER_BAL_A) << 1) | digitalRead(ENCODER_BAL_B));

    // Attach interrupt su entrambi i fronti di CLK e DT (quadraturale)
    attachInterrupt(digitalPinToInterrupt(ENCODER_VOL_A),   isr_vol_encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_VOL_B),   isr_vol_encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_VOL_BTN), isr_vol_button,  CHANGE);

    attachInterrupt(digitalPinToInterrupt(ENCODER_BAL_A),   isr_bal_encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_BAL_B),   isr_bal_encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_BAL_BTN), isr_bal_button,  CHANGE);

    // Init LED rings con valori default
    led_ring_init();
    led_ring_set_volume(s_volValue);
    led_ring_set_balance(s_balValue);

    Serial.println("[ENCODER] Encoder rotativi inizializzati (quadraturale 4-step)");
}

// ===== Tick – processa delta ISR e chiama callback (sicuro fuori da ISR) =====
void encoder_tick() {
#if USE_SLAVE_PERIPHERALS
    if (s_useSlaveEnc) {
        int8_t vd, bd;
        uint8_t btn;
        if (ipc_poll_inputs(vd, bd, btn)) {
            if (vd) {
                s_volValue = constrain(s_volValue + vd, 0, 100);
                ipc_set_led_ring((uint8_t)s_volValue, (int8_t)s_balValue);
                if (s_volChangeCb) s_volChangeCb(vd > 0 ? 1 : -1);
            }
            if (bd) {
                s_balValue = constrain(s_balValue + bd, -50, 50);
                ipc_set_led_ring((uint8_t)s_volValue, (int8_t)s_balValue);
                if (s_balChangeCb) s_balChangeCb(bd > 0 ? 1 : -1);
            }
            if (btn & 1) { if (s_volButtonCb) s_volButtonCb(true); }
            if (btn & 2) { encoder_set_balance(0); if (s_balButtonCb) s_balButtonCb(true); }
        }
        return;
    }
#endif
    noInterrupts();
    int32_t volDelta = s_encVolISRDelta;
    int32_t balDelta = s_encBalISRDelta;
    s_encVolISRDelta = 0;
    s_encBalISRDelta = 0;
    bool volBtnChanged = s_volBtnChanged;
    bool volBtnState   = s_volBtnISRState;
    s_volBtnChanged = false;
    bool balBtnChanged = s_balBtnChanged;
    bool balBtnState   = s_balBtnISRState;
    s_balBtnChanged = false;
    interrupts();

    // Applica delta volume (ogni 4 passi quadraturali = 1 click fisico)
    if (abs(volDelta) >= 4) {
        int steps = (int)(volDelta / 4);
        s_volValue = constrain(s_volValue + steps, 0, 100);
        led_ring_set_volume(s_volValue);
        if (s_volChangeCb) s_volChangeCb((int8_t)(steps > 0 ? 1 : -1));
        // Ri-accumula i passi rimanenti (modulo 4)
        noInterrupts();
        s_encVolISRDelta += (volDelta % 4);
        interrupts();
    }

    // Applica delta balance
    if (abs(balDelta) >= 4) {
        int steps = (int)(balDelta / 4);
        s_balValue = constrain(s_balValue + steps, -50, 50);
        led_ring_set_balance(s_balValue);
        if (s_balChangeCb) s_balChangeCb((int8_t)(steps > 0 ? 1 : -1));
        noInterrupts();
        s_encBalISRDelta += (balDelta % 4);
        interrupts();
    }

    // Processa pulsante volume
    if (volBtnChanged && volBtnState != s_volBtnPressed) {
        s_volBtnPressed = volBtnState;
        if (s_volButtonCb) s_volButtonCb(s_volBtnPressed);
    }

    // Processa pulsante balance
    if (balBtnChanged && balBtnState != s_balBtnPressed) {
        s_balBtnPressed = balBtnState;
        if (s_balButtonCb) s_balButtonCb(s_balBtnPressed);
    }

    // Aggiorna animazioni LED ring
    led_ring_update();
}

// ===== Callback registration =====
void encoder_vol_on_change(EncoderCallback cb) { s_volChangeCb = cb; }
void encoder_vol_on_button(ButtonCallback cb)  { s_volButtonCb = cb; }
void encoder_bal_on_change(EncoderCallback cb) { s_balChangeCb = cb; }
void encoder_bal_on_button(ButtonCallback cb)  { s_balButtonCb = cb; }

// ===== Getters/Setters =====
int encoder_get_volume()  { return s_volValue; }
int encoder_get_balance() { return s_balValue; }

void encoder_set_volume(int vol) {
    s_volValue = constrain(vol, 0, 100);
    led_ring_set_volume(s_volValue);
#if USE_SLAVE_PERIPHERALS
    if (s_useSlaveEnc) ipc_set_led_ring((uint8_t)s_volValue, (int8_t)s_balValue);
#endif
}

void encoder_set_balance(int bal) {
    s_balValue = constrain(bal, -50, 50);
    led_ring_set_balance(s_balValue);
#if USE_SLAVE_PERIPHERALS
    if (s_useSlaveEnc) ipc_set_led_ring((uint8_t)s_volValue, (int8_t)s_balValue);
#endif
}
