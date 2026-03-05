#include "lvgl_display.h"
#include <Arduino.h>

// ======= Display UEDX80480050E-WB-A – Implementazione =======
// Configurazione LovyanGFX per pannello 800×480 RGB con touch GT911.
// Riferimento: docs/UEDX80480050E-WB-A-V3.3-SPEC.pdf pagine 6-9.

#ifdef USE_LOVYAN_GFX

// Istanza globale
LGFX g_display;

// ——— Costruttore: configurazione pannello ———
LGFX::LGFX() {
    // ——— Bus RGB ———
    {
        auto cfg = _bus_instance.config();

        cfg.panel = &_panel_instance;

        // Pin dati RGB (dalla SPEC, interfaccia 40-pin)
        cfg.pin_d0  = LCD_B0;   // B0
        cfg.pin_d1  = LCD_B1;   // B1
        cfg.pin_d2  = LCD_B2;   // B2
        cfg.pin_d3  = LCD_B3;   // B3
        cfg.pin_d4  = LCD_B4;   // B4
        cfg.pin_d5  = LCD_G0;   // G0
        cfg.pin_d6  = LCD_G1;   // G1
        cfg.pin_d7  = LCD_G2;   // G2
        cfg.pin_d8  = LCD_G3;   // G3
        cfg.pin_d9  = LCD_G4;   // G4
        cfg.pin_d10 = LCD_G5;   // G5
        cfg.pin_d11 = LCD_R0;   // R0
        cfg.pin_d12 = LCD_R1;   // R1
        cfg.pin_d13 = LCD_R2;   // R2
        cfg.pin_d14 = LCD_R3;   // R3
        cfg.pin_d15 = LCD_R4;   // R4

        cfg.pin_henable = LCD_DE;     // Data Enable
        cfg.pin_vsync   = LCD_VSYNC;  // V-Sync
        cfg.pin_hsync   = LCD_HSYNC;  // H-Sync
        cfg.pin_pclk    = LCD_PCLK;   // Pixel Clock

        // Timing per 800×480 @ 30fps (SPEC pag.8)
        // Pixel clock: 20 MHz (tipico per questo pannello)
        cfg.freq_write = 20000000;

        // Horizontal timing (pixel)
        cfg.hsync_polarity    = 0;    // Active LOW
        cfg.hsync_front_porch = 8;
        cfg.hsync_pulse_width = 4;
        cfg.hsync_back_porch  = 8;

        // Vertical timing (linee)
        cfg.vsync_polarity    = 0;    // Active LOW
        cfg.vsync_front_porch = 8;
        cfg.vsync_pulse_width = 4;
        cfg.vsync_back_porch  = 8;

        cfg.pclk_active_neg = true;   // Clock sul fronte di discesa (SPEC pag.8)

        _bus_instance.config(cfg);
        _panel_instance.setBus(&_bus_instance);
    }

    // ——— Pannello RGB ———
    {
        auto cfg = _panel_instance.config();

        cfg.memory_width  = 800;
        cfg.memory_height = 480;
        cfg.panel_width   = 800;
        cfg.panel_height  = 480;
        cfg.offset_x      = 0;
        cfg.offset_y      = 0;

        _panel_instance.config(cfg);
    }

    // ——— Backlight PWM ———
    {
        auto cfg = _light_instance.config();
        cfg.pin_bl      = LCD_BL_EN;
        cfg.invert      = false;
        cfg.freq        = 44100;  // Hz PWM
        cfg.pwm_channel = 7;

        _light_instance.config(cfg);
        _panel_instance.setLight(&_light_instance);
    }

    // ——— Touch GT911 (I2C) ———
    {
        auto cfg = _touch_instance.config();

        cfg.x_min        = 0;
        cfg.x_max        = 799;
        cfg.y_min        = 0;
        cfg.y_max        = 479;
        cfg.pin_int      = TOUCH_INT;
        cfg.pin_rst      = TOUCH_RST;
        cfg.bus_shared   = false;    // I2C dedicato al touch
        cfg.offset_rotation = 0;

        // I2C per touch
        cfg.i2c_port = 1;           // I2C port 1 (separato dall'ES8388 su port 0)
        cfg.i2c_addr = TOUCH_I2C_ADDR;
        cfg.pin_sda  = TOUCH_SDA;
        cfg.pin_scl  = TOUCH_SCL;
        cfg.freq     = 400000;      // 400kHz Fast Mode

        _touch_instance.config(cfg);
        _panel_instance.setTouch(&_touch_instance);
    }

    setPanel(&_panel_instance);
}

#endif  // USE_LOVYAN_GFX

// ——— API display ———

bool display_init() {
#ifdef USE_LOVYAN_GFX
    if (!g_display.init()) {
        Serial.println("[DISPLAY] Errore init LovyanGFX");
        return false;
    }
    g_display.setRotation(0);
    g_display.setBrightness(128);  // 50% default
    Serial.println("[DISPLAY] UEDX80480050E-WB-A inizializzato (800×480 RGB)");
    return true;
#else
    Serial.println("[DISPLAY] LovyanGFX non disponibile (USE_LOVYAN_GFX non definito)");
    return false;
#endif
}

void display_set_brightness(uint8_t brightness) {
#ifdef USE_LOVYAN_GFX
    g_display.setBrightness(brightness);
#else
    // Fallback: PWM diretto sul pin backlight
    analogWrite(LCD_BL_EN, brightness);
#endif
}

bool display_get_touch(uint16_t& x, uint16_t& y) {
#ifdef USE_LOVYAN_GFX
    lgfx::touch_point_t tp;
    if (g_display.getTouch(&tp)) {
        x = (uint16_t)tp.x;
        y = (uint16_t)tp.y;
        return true;
    }
#endif
    return false;
}
