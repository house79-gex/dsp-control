#pragma once
#include <lvgl.h>

// Tab DSP avanzato sul display touch 5"
void ui_dsp_advanced_create(lv_obj_t* parent);
void ui_dsp_advanced_update_vu(float leftDb, float rightDb); // Aggiorna VU meter
