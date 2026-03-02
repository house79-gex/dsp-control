#pragma once
#include <Arduino.h>
#include <lvgl.h>

// ======= Driver Display SPI + Touch Capacitivo =======
// Supporta display 5" 800x480 (ILI9488 / ST7796) via LovyanGFX
// Touch capacitivo GT911 / FT5x06 via I2C

// Inizializza display SPI, backlight e rotazione
void display_init();

// Callback flush LVGL (da registrare come dispDrv.flush_cb)
void display_flush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p);

// Callback lettura touch LVGL (da registrare come indevDrv.read_cb)
void touch_read(lv_indev_drv_t* indev, lv_indev_data_t* data);

// Imposta luminosità backlight (0-255)
void display_set_backlight(uint8_t brightness);
