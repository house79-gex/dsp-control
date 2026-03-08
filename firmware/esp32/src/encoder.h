#pragma once
#include <Arduino.h>

// ===== Encoder Rotativi con Decodifica Quadraturale =====

// Callback per eventi encoder
typedef void (*EncoderCallback)(int8_t delta);  // delta: -1 (CCW), +1 (CW)
typedef void (*ButtonCallback)(bool pressed);   // pressed: true=pressed, false=released

// Inizializzazione encoder
void encoder_init();

// Tick periodico encoder (chiamare ogni loop per processare delta e aggiornare LED ring)
void encoder_tick();

// Registra callback per volume encoder
void encoder_vol_on_change(EncoderCallback cb);
void encoder_vol_on_button(ButtonCallback cb);

// Registra callback per balance encoder
void encoder_bal_on_change(EncoderCallback cb);
void encoder_bal_on_button(ButtonCallback cb);

// Getter valori correnti
int encoder_get_volume();    // 0-100
int encoder_get_balance();   // -50..+50

// Setter valori (per sync con UI LVGL)
void encoder_set_volume(int vol);
void encoder_set_balance(int bal);
