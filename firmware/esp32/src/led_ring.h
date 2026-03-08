#pragma once
#include <Arduino.h>
#include "config.h"

// ======= Modulo LED Ring WS2812B =======
// Gestisce anelli LED WS2812B attorno agli encoder rotativi
// Indipendente dal sistema DMX (indicatori locali sul pannello)

// Inizializzazione
void led_ring_init();

// LED Ring
void led_ring_set_volume(int percent);    // 0-100 → LED verde→giallo→rosso
void led_ring_set_balance(int balance);   // -50..+50 → LED SX o DX dal centro
void led_ring_update();                   // Aggiorna animazioni (chiamare nel loop)
void led_ring_effect_pulse(uint8_t r, uint8_t g, uint8_t b);  // Effetto pulse
