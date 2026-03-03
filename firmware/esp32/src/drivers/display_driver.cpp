#include "display_driver.h"
#include "../../config.h"
#include <Arduino.h>

// ======= Driver Display + Touch via LovyanGFX =======
//
// Supporta display ILI9488 / ST7796 800x480 SPI
// Touch capacitivo GT911 / FT5x06 I2C
//
// Touch auto-detection: I2C scan su 0x38 (FT5x06) e 0x5D/0x14 (GT911)
// Attivare con USE_LOVYAN_GFX in platformio.ini build_flags.

#ifdef USE_LOVYAN_GFX
#include <LovyanGFX.hpp>
#include <Wire.h>

// ——— Auto-detection touch controller ———
// Ritorna: 0=nessuno, 1=FT5x06 (0x38), 2=GT911 (0x5D), 3=GT911 (0x14)
static uint8_t s_touchType = 0;  // rilevato in display_init()

static uint8_t detect_touch_controller() {
    Wire.begin(TOUCH_SDA, TOUCH_SCL);
    Wire.setClock(400000);

    // Prova FT5x06 @ 0x38
    Wire.beginTransmission(0x38);
    if (Wire.endTransmission() == 0) {
        Serial.println("[TOUCH] Rilevato FT5x06 @ 0x38");
        return 1;
    }
    // Prova GT911 @ 0x5D
    Wire.beginTransmission(0x5D);
    if (Wire.endTransmission() == 0) {
        Serial.println("[TOUCH] Rilevato GT911 @ 0x5D");
        return 2;
    }
    // Prova GT911 @ 0x14
    Wire.beginTransmission(0x14);
    if (Wire.endTransmission() == 0) {
        Serial.println("[TOUCH] Rilevato GT911 @ 0x14");
        return 3;
    }
    Serial.printf("[TOUCH] Nessun controller rilevato, fallback FT5x06 @ 0x38\n");
    return 1;  // fallback FT5x06
}

// ——— Classi LGFX separate per FT5x06 e GT911 ———

class LGFX_FT5x06 : public lgfx::LGFX_Device {
    lgfx::Panel_ILI9488   _panel_instance;
    lgfx::Bus_SPI         _bus_instance;
    lgfx::Light_PWM       _light_instance;
    lgfx::Touch_FT5x06    _touch_instance;
public:
    LGFX_FT5x06() {
        {
            auto cfg = _bus_instance.config();
            cfg.spi_host    = SPI2_HOST;
            cfg.spi_mode    = 0;
            cfg.freq_write  = 40000000;
            cfg.freq_read   = 16000000;
            cfg.spi_3wire   = false;
            cfg.use_lock    = true;
            cfg.dma_channel = SPI_DMA_CH_AUTO;
            cfg.pin_sclk    = LCD_SCLK;
            cfg.pin_mosi    = LCD_MOSI;
            cfg.pin_miso    = LCD_MISO;
            cfg.pin_dc      = LCD_DC;
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }
        {
            auto cfg = _panel_instance.config();
            cfg.pin_cs           = LCD_CS;
            cfg.pin_rst          = LCD_RST;
            cfg.pin_busy         = -1;
            cfg.panel_width      = 480;
            cfg.panel_height     = 800;
            cfg.offset_x         = 0;
            cfg.offset_y         = 0;
            cfg.offset_rotation  = 1;
            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits  = 1;
            cfg.readable         = true;
            cfg.invert           = false;
            cfg.rgb_order        = false;
            cfg.dlen_16bit       = false;
            cfg.bus_shared       = false;
            _panel_instance.config(cfg);
        }
        {
            auto cfg = _light_instance.config();
            cfg.pin_bl      = LCD_BL;
            cfg.invert      = false;
            cfg.freq        = 44100;
            cfg.pwm_channel = 7;
            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }
        {
            auto cfg = _touch_instance.config();
            cfg.x_min           = 0;
            cfg.x_max           = 799;
            cfg.y_min           = 0;
            cfg.y_max           = 479;
            cfg.pin_int         = -1;
            cfg.bus_shared      = true;
            cfg.offset_rotation = 0;
            cfg.i2c_port        = 0;
            cfg.i2c_addr        = 0x38;
            cfg.pin_sda         = TOUCH_SDA;
            cfg.pin_scl         = TOUCH_SCL;
            cfg.freq            = 400000;
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }
        setPanel(&_panel_instance);
    }
};

class LGFX_GT911 : public lgfx::LGFX_Device {
    lgfx::Panel_ILI9488   _panel_instance;
    lgfx::Bus_SPI         _bus_instance;
    lgfx::Light_PWM       _light_instance;
    lgfx::Touch_GT911     _touch_instance;
public:
    LGFX_GT911(uint8_t addr = 0x5D) {
        {
            auto cfg = _bus_instance.config();
            cfg.spi_host    = SPI2_HOST;
            cfg.spi_mode    = 0;
            cfg.freq_write  = 40000000;
            cfg.freq_read   = 16000000;
            cfg.spi_3wire   = false;
            cfg.use_lock    = true;
            cfg.dma_channel = SPI_DMA_CH_AUTO;
            cfg.pin_sclk    = LCD_SCLK;
            cfg.pin_mosi    = LCD_MOSI;
            cfg.pin_miso    = LCD_MISO;
            cfg.pin_dc      = LCD_DC;
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }
        {
            auto cfg = _panel_instance.config();
            cfg.pin_cs           = LCD_CS;
            cfg.pin_rst          = LCD_RST;
            cfg.pin_busy         = -1;
            cfg.panel_width      = 480;
            cfg.panel_height     = 800;
            cfg.offset_x         = 0;
            cfg.offset_y         = 0;
            cfg.offset_rotation  = 1;
            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits  = 1;
            cfg.readable         = true;
            cfg.invert           = false;
            cfg.rgb_order        = false;
            cfg.dlen_16bit       = false;
            cfg.bus_shared       = false;
            _panel_instance.config(cfg);
        }
        {
            auto cfg = _light_instance.config();
            cfg.pin_bl      = LCD_BL;
            cfg.invert      = false;
            cfg.freq        = 44100;
            cfg.pwm_channel = 7;
            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }
        {
            auto cfg = _touch_instance.config();
            cfg.x_min           = 0;
            cfg.x_max           = 799;
            cfg.y_min           = 0;
            cfg.y_max           = 479;
            cfg.pin_int         = -1;
            cfg.bus_shared      = true;
            cfg.offset_rotation = 0;
            cfg.i2c_port        = 0;
            cfg.i2c_addr        = addr;
            cfg.pin_sda         = TOUCH_SDA;
            cfg.pin_scl         = TOUCH_SCL;
            cfg.freq            = 400000;
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }
        setPanel(&_panel_instance);
    }
};

// Puntatore generico al device LGFX attivo (FT5x06 o GT911)
static lgfx::LGFX_Device* s_lcd = nullptr;
static LGFX_FT5x06  s_lcd_ft5x06;
static LGFX_GT911   s_lcd_gt911_5d(0x5D);
static LGFX_GT911   s_lcd_gt911_14(0x14);

// Debounce touch: intervallo minimo tra eventi (ms)
#define TOUCH_DEBOUNCE_MS 50
static uint32_t s_lastTouchMs = 0;
static bool     s_lastTouchState = false;

void display_init() {
    s_touchType = detect_touch_controller();

    switch (s_touchType) {
        case 2:  s_lcd = &s_lcd_gt911_5d; break;
        case 3:  s_lcd = &s_lcd_gt911_14; break;
        default: s_lcd = &s_lcd_ft5x06;   break;
    }

    s_lcd->init();
    s_lcd->setRotation(1);   // Landscape 800x480
    s_lcd->setBrightness(200);
    s_lcd->fillScreen(TFT_BLACK);
    Serial.printf("[DISPLAY] LovyanGFX inizializzato (800x480, touch=%d)\n", s_touchType);
}

void display_flush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p) {
    if (!s_lcd) { lv_disp_flush_ready(disp); return; }
    int32_t w = area->x2 - area->x1 + 1;
    int32_t h = area->y2 - area->y1 + 1;
    s_lcd->startWrite();
    s_lcd->setAddrWindow(area->x1, area->y1, w, h);
    s_lcd->writePixels((lgfx::rgb565_t*)color_p, w * h);
    s_lcd->endWrite();
    lv_disp_flush_ready(disp);
}

void touch_read(lv_indev_drv_t* indev, lv_indev_data_t* data) {
    if (!s_lcd) { data->state = LV_INDEV_STATE_REL; return; }

    uint16_t tx = 0, ty = 0;
    bool pressed = s_lcd->getTouch(&tx, &ty);
    uint32_t now = millis();

    if (pressed) {
        // Applica debounce: ignora eventi troppo ravvicinati (< 50ms) solo al rising edge
        if (!s_lastTouchState || (now - s_lastTouchMs) >= TOUCH_DEBOUNCE_MS) {
            data->point.x   = tx;
            data->point.y   = ty;
            data->state     = LV_INDEV_STATE_PR;
            s_lastTouchMs   = now;
            s_lastTouchState = true;
        } else {
            // Ancora nel debounce: mantieni l'ultima posizione valida
            data->point.x = tx;
            data->point.y = ty;
            data->state   = LV_INDEV_STATE_PR;
        }
    } else {
        data->state      = LV_INDEV_STATE_REL;
        s_lastTouchState = false;
    }
}

void display_set_backlight(uint8_t brightness) {
    if (s_lcd) s_lcd->setBrightness(brightness);
}

#else  // USE_LOVYAN_GFX non definito: stub per compatibilità

void display_init() {
    Serial.println("[DISPLAY] Driver stub (abilitare USE_LOVYAN_GFX in platformio.ini)");
}

void display_flush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p) {
    lv_disp_flush_ready(disp);
}

void touch_read(lv_indev_drv_t* indev, lv_indev_data_t* data) {
    data->state = LV_INDEV_STATE_REL;
}

void display_set_backlight(uint8_t brightness) {
    (void)brightness;
}

#endif  // USE_LOVYAN_GFX
