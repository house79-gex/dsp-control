#include "ui_dsp_advanced.h"
#include "../dsp_control.h"
#include "../audio_mode.h"
#include "../autotune.h"

static lv_obj_t* s_speakerDropdown = nullptr;
static lv_obj_t* s_gainLabel = nullptr;
static lv_obj_t* s_delayLabel = nullptr;
static lv_obj_t* s_presetDropdown = nullptr;
static lv_obj_t* s_vuLeft = nullptr;
static lv_obj_t* s_vuRight = nullptr;
static lv_timer_t* s_vuTimer = nullptr;

// Callback gain
static void cb_gain(lv_event_t* e) {
    lv_obj_t* slider = lv_event_get_target(e);
    int val = lv_slider_get_value(slider);
    char buf[32];
    snprintf(buf, sizeof(buf), "Gain: %+d dB", val);
    lv_label_set_text(s_gainLabel, buf);
    // TODO: applicare al device selezionato
}

// Callback delay
static void cb_delay(lv_event_t* e) {
    lv_obj_t* slider = lv_event_get_target(e);
    int val = lv_slider_get_value(slider);
    char buf[32];
    snprintf(buf, sizeof(buf), "Delay: %d ms", val);
    lv_label_set_text(s_delayLabel, buf);
    // TODO: applicare al device selezionato
}

// Callback AutoTune
static void cb_autotune(lv_event_t* e) {
    (void)e;
    autotune_start(AutotuneMode::Single, 1);
    Serial.println("[UI_DSP] AutoTune avviato");
}

// Timer VU meter (ogni 100ms)
static void vu_timer_cb(lv_timer_t* timer) {
    (void)timer;
    float left, right;
    audio_get_channel_levels(left, right);
    ui_dsp_advanced_update_vu(left * 100.0f, right * 100.0f);
}

void ui_dsp_advanced_create(lv_obj_t* parent) {
    // Titolo
    lv_obj_t* title = lv_label_create(parent);
    lv_label_set_text(title, "DSP AVANZATO");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 8);

    // Selettore cassa/gruppo
    lv_obj_t* lblSpeaker = lv_label_create(parent);
    lv_label_set_text(lblSpeaker, "Cassa/Gruppo:");
    lv_obj_align(lblSpeaker, LV_ALIGN_TOP_LEFT, 10, 44);
    s_speakerDropdown = lv_dropdown_create(parent);
    lv_dropdown_set_options(s_speakerDropdown, "Tutti\nSX\nDX\nSUB\nCassa 1\nCassa 2");
    lv_obj_set_size(s_speakerDropdown, 200, 36);
    lv_obj_align(s_speakerDropdown, LV_ALIGN_TOP_LEFT, 130, 40);

    // Gain
    s_gainLabel = lv_label_create(parent);
    lv_label_set_text(s_gainLabel, "Gain: 0 dB");
    lv_obj_align(s_gainLabel, LV_ALIGN_TOP_LEFT, 10, 90);
    lv_obj_t* gainSlider = lv_slider_create(parent);
    lv_slider_set_range(gainSlider, -60, 12);
    lv_slider_set_value(gainSlider, 0, LV_ANIM_OFF);
    lv_obj_set_size(gainSlider, 320, 20);
    lv_obj_align(gainSlider, LV_ALIGN_TOP_LEFT, 10, 114);
    lv_obj_add_event_cb(gainSlider, cb_gain, LV_EVENT_VALUE_CHANGED, NULL);

    // EQ semplificato (Bass/Mid/Treble)
    lv_obj_t* lblEq = lv_label_create(parent);
    lv_label_set_text(lblEq, "EQ: Bass");
    lv_obj_align(lblEq, LV_ALIGN_TOP_LEFT, 10, 146);
    lv_obj_t* bassSlider = lv_slider_create(parent);
    lv_slider_set_range(bassSlider, -12, 12);
    lv_slider_set_value(bassSlider, 0, LV_ANIM_OFF);
    lv_obj_set_size(bassSlider, 100, 16);
    lv_obj_align(bassSlider, LV_ALIGN_TOP_LEFT, 10, 166);
    (void)bassSlider;

    lv_obj_t* lblMid = lv_label_create(parent);
    lv_label_set_text(lblMid, "Mid");
    lv_obj_align(lblMid, LV_ALIGN_TOP_LEFT, 120, 146);
    lv_obj_t* midSlider = lv_slider_create(parent);
    lv_slider_set_range(midSlider, -12, 12);
    lv_slider_set_value(midSlider, 0, LV_ANIM_OFF);
    lv_obj_set_size(midSlider, 100, 16);
    lv_obj_align(midSlider, LV_ALIGN_TOP_LEFT, 120, 166);
    (void)midSlider;

    lv_obj_t* lblTre = lv_label_create(parent);
    lv_label_set_text(lblTre, "Treble");
    lv_obj_align(lblTre, LV_ALIGN_TOP_LEFT, 230, 146);
    lv_obj_t* treSlider = lv_slider_create(parent);
    lv_slider_set_range(treSlider, -12, 12);
    lv_slider_set_value(treSlider, 0, LV_ANIM_OFF);
    lv_obj_set_size(treSlider, 100, 16);
    lv_obj_align(treSlider, LV_ALIGN_TOP_LEFT, 230, 166);
    (void)treSlider;

    // Delay
    s_delayLabel = lv_label_create(parent);
    lv_label_set_text(s_delayLabel, "Delay: 0 ms");
    lv_obj_align(s_delayLabel, LV_ALIGN_TOP_LEFT, 10, 196);
    lv_obj_t* delaySlider = lv_slider_create(parent);
    lv_slider_set_range(delaySlider, 0, 500);
    lv_slider_set_value(delaySlider, 0, LV_ANIM_OFF);
    lv_obj_set_size(delaySlider, 320, 20);
    lv_obj_align(delaySlider, LV_ALIGN_TOP_LEFT, 10, 220);
    lv_obj_add_event_cb(delaySlider, cb_delay, LV_EVENT_VALUE_CHANGED, NULL);

    // Selettore preset
    lv_obj_t* lblPreset = lv_label_create(parent);
    lv_label_set_text(lblPreset, "Preset:");
    lv_obj_align(lblPreset, LV_ALIGN_TOP_LEFT, 10, 254);
    s_presetDropdown = lv_dropdown_create(parent);
    lv_dropdown_set_options(s_presetDropdown, "2WAY_STD\n3WAY_STD\nSUB_STD\nFLAT\nAUTOTUNE");
    lv_obj_set_size(s_presetDropdown, 200, 36);
    lv_obj_align(s_presetDropdown, LV_ALIGN_TOP_LEFT, 80, 250);

    // Bottone AutoTune
    lv_obj_t* btnAt = lv_btn_create(parent);
    lv_obj_align(btnAt, LV_ALIGN_TOP_LEFT, 295, 250);
    lv_obj_set_size(btnAt, 130, 36);
    lv_obj_set_style_bg_color(btnAt, lv_color_hex(0xe94560), 0);
    lv_obj_add_event_cb(btnAt, cb_autotune, LV_EVENT_CLICKED, NULL);
    lv_obj_t* lblAt = lv_label_create(btnAt);
    lv_label_set_text(lblAt, LV_SYMBOL_SETTINGS " AutoTune");
    lv_obj_center(lblAt);

    // VU meter (barre verticali L/R)
    lv_obj_t* lblVu = lv_label_create(parent);
    lv_label_set_text(lblVu, "VU Meter:");
    lv_obj_align(lblVu, LV_ALIGN_TOP_LEFT, 450, 44);

    // Barra VU sinistra
    lv_obj_t* bgL = lv_obj_create(parent);
    lv_obj_set_size(bgL, 30, 300);
    lv_obj_align(bgL, LV_ALIGN_TOP_LEFT, 450, 68);
    lv_obj_set_style_bg_color(bgL, lv_color_hex(0x0f3460), 0);
    lv_obj_set_style_border_width(bgL, 0, 0);
    lv_obj_set_style_pad_all(bgL, 0, 0);

    s_vuLeft = lv_bar_create(bgL);
    lv_obj_set_size(s_vuLeft, 30, 300);
    lv_bar_set_range(s_vuLeft, 0, 100);
    lv_bar_set_value(s_vuLeft, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(s_vuLeft, lv_color_hex(0x00e676), LV_PART_INDICATOR);
    lv_obj_align(s_vuLeft, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    lv_obj_t* lblL = lv_label_create(parent);
    lv_label_set_text(lblL, "L");
    lv_obj_align(lblL, LV_ALIGN_TOP_LEFT, 455, 374);

    // Barra VU destra
    lv_obj_t* bgR = lv_obj_create(parent);
    lv_obj_set_size(bgR, 30, 300);
    lv_obj_align(bgR, LV_ALIGN_TOP_LEFT, 495, 68);
    lv_obj_set_style_bg_color(bgR, lv_color_hex(0x0f3460), 0);
    lv_obj_set_style_border_width(bgR, 0, 0);
    lv_obj_set_style_pad_all(bgR, 0, 0);

    s_vuRight = lv_bar_create(bgR);
    lv_obj_set_size(s_vuRight, 30, 300);
    lv_bar_set_range(s_vuRight, 0, 100);
    lv_bar_set_value(s_vuRight, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(s_vuRight, lv_color_hex(0x00e676), LV_PART_INDICATOR);
    lv_obj_align(s_vuRight, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    lv_obj_t* lblR = lv_label_create(parent);
    lv_label_set_text(lblR, "R");
    lv_obj_align(lblR, LV_ALIGN_TOP_LEFT, 500, 374);

    // Timer VU meter (ogni 100ms)
    s_vuTimer = lv_timer_create(vu_timer_cb, 100, NULL);
}

void ui_dsp_advanced_update_vu(float leftPct, float rightPct) {
    if (s_vuLeft)  lv_bar_set_value(s_vuLeft,  (int)leftPct,  LV_ANIM_ON);
    if (s_vuRight) lv_bar_set_value(s_vuRight, (int)rightPct, LV_ANIM_ON);
}
