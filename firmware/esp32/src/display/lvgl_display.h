#pragma once

// ======= Configurazione Display UEDX80480050E-WB-A 5" =======
// Pannello 800×480 RGB 24-bit con touch GT911 via I2C.
// Riferimento: docs/UEDX80480050E-WB-A-V3.3-SPEC.pdf
//
// Driver: LovyanGFX (Panel_RGB + Bus_RGB + Touch_GT911)
// Libreria: lovyan03/LovyanGFX@^1.1.16

#ifdef USE_LOVYAN_GFX
#include <LovyanGFX.hpp>
#include "../config.h"

// ——— Configurazione LovyanGFX per UEDX80480050E-WB-A ———
class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_RGB   _panel_instance;
    lgfx::Bus_RGB     _bus_instance;
    lgfx::Light_PWM   _light_instance;
    lgfx::Touch_GT911 _touch_instance;

public:
    LGFX();
};

// Istanza globale del display
extern LGFX g_display;

#endif  // USE_LOVYAN_GFX

// ——— API display indipendente da LovyanGFX ———

// Inizializza display e touch
bool display_init();

// Imposta luminosità backlight (0-255)
void display_set_brightness(uint8_t brightness);

// Ritorna true se il touch è premuto; popola x, y con le coordinate
bool display_get_touch(uint16_t& x, uint16_t& y);
