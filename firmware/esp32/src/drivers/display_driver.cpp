#include "display_driver.h"
#include "../../config.h"
#include <Arduino.h>

// ======= Driver Display + Touch via LovyanGFX =======
//
// Richiede libreria: lovyan03/LovyanGFX@^1.1.9
// Supporta display ILI9488 / ST7796 800x480 SPI
// Touch capacitivo GT911 / FT5x06 I2C
//
// Per abilitare il driver reale decommentare #define USE_LOVYAN_GFX
// e aggiungere in platformio.ini:
//   lib_deps = ... lovyan03/LovyanGFX@^1.1.9
//
// #define USE_LOVYAN_GFX

#ifdef USE_LOVYAN_GFX
#include <LovyanGFX.hpp>

// ——— Configurazione LovyanGFX ———
class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_ILI9488   _panel_instance;
    lgfx::Bus_SPI         _bus_instance;
    lgfx::Light_PWM       _light_instance;
    lgfx::Touch_FT5x06    _touch_instance;

public:
    LGFX() {
        {
            auto cfg = _bus_instance.config();
            cfg.spi_host   = SPI2_HOST;
            cfg.spi_mode   = 0;
            cfg.freq_write = 40000000;
            cfg.freq_read  = 16000000;
            cfg.spi_3wire  = false;
            cfg.use_lock   = true;
            cfg.dma_channel = SPI_DMA_CH_AUTO;
            cfg.pin_sclk   = LCD_SCLK;
            cfg.pin_mosi   = LCD_MOSI;
            cfg.pin_miso   = LCD_MISO;
            cfg.pin_dc     = LCD_DC;
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }
        {
            auto cfg = _panel_instance.config();
            cfg.pin_cs         = LCD_CS;
            cfg.pin_rst        = LCD_RST;
            cfg.pin_busy       = -1;
            cfg.panel_width    = 480;
            cfg.panel_height   = 800;
            cfg.offset_x       = 0;
            cfg.offset_y       = 0;
            cfg.offset_rotation = 1;  // 0=portrait, 1=landscape
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
            cfg.pin_bl       = LCD_BL;
            cfg.invert       = false;
            cfg.freq         = 44100;
            cfg.pwm_channel  = 7;
            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }
        {
            auto cfg = _touch_instance.config();
            cfg.x_min      = 0;
            cfg.x_max      = 799;
            cfg.y_min      = 0;
            cfg.y_max      = 479;
            cfg.pin_int    = -1;
            cfg.bus_shared = true;
            cfg.offset_rotation = 0;
            cfg.i2c_port   = 0;
            // Auto-detect: FT5x06 usa 0x38, GT911 usa 0x5D o 0x14
            // Modificare in base al touch controller del display in uso:
            cfg.i2c_addr   = TOUCH_I2C_ADDR;
            cfg.pin_sda    = TOUCH_SDA;
            cfg.pin_scl    = TOUCH_SCL;
            cfg.freq       = 400000;
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }
        setPanel(&_panel_instance);
    }
};

static LGFX s_lcd;

void display_init() {
    s_lcd.init();
    s_lcd.setRotation(1);  // Landscape 800x480
    s_lcd.setBrightness(200);
    s_lcd.fillScreen(TFT_BLACK);
    Serial.println("[DISPLAY] LovyanGFX inizializzato (800x480)");
}

void display_flush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p) {
    int32_t w = area->x2 - area->x1 + 1;
    int32_t h = area->y2 - area->y1 + 1;
    s_lcd.startWrite();
    s_lcd.setAddrWindow(area->x1, area->y1, w, h);
    s_lcd.writePixels((lgfx::rgb565_t*)color_p, w * h);
    s_lcd.endWrite();
    lv_disp_flush_ready(disp);
}

void touch_read(lv_indev_drv_t* indev, lv_indev_data_t* data) {
    uint16_t tx = 0, ty = 0;
    if (s_lcd.getTouch(&tx, &ty)) {
        data->point.x = tx;
        data->point.y = ty;
        data->state   = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

void display_set_backlight(uint8_t brightness) {
    s_lcd.setBrightness(brightness);
}

#else  // USE_LOVYAN_GFX non definito: stub per compatibilità

void display_init() {
    Serial.println("[DISPLAY] Driver stub (abilitare USE_LOVYAN_GFX + LovyanGFX lib)");
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
