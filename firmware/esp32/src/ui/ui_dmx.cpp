#include "ui_dmx.h"
#include "../dmx512.h"

static lv_obj_t* s_fixtureList = nullptr;
static lv_obj_t* s_masterDimmerLabel = nullptr;
static lv_obj_t* s_sceneDropdown = nullptr;
static lv_obj_t* s_arLabel = nullptr;

// Callback master dimmer
static void cb_master_dimmer(lv_event_t* e) {
    lv_obj_t* slider = lv_event_get_target(e);
    int val = lv_slider_get_value(slider);
    char buf[32];
    snprintf(buf, sizeof(buf), "Master Dimmer: %d%%", val);
    lv_label_set_text(s_masterDimmerLabel, buf);
    dmx_set_master_dimmer((uint8_t)(val * 255 / 100));
}

// Callback BLACKOUT
static void cb_blackout(lv_event_t* e) {
    (void)e;
    dmx_blackout();
    Serial.println("[UI_DMX] BLACKOUT attivato");
}

// Callback slider R (stub – da collegare alla fixture selezionata)
static void cb_slider_r(lv_event_t* e) {
    lv_obj_t* slider = lv_event_get_target(e);
    (void)slider;
    // TODO: applicare colore alla fixture selezionata
}

void ui_dmx_create(lv_obj_t* parent) {
    // Titolo
    lv_obj_t* title = lv_label_create(parent);
    lv_label_set_text(title, "CONTROLLO LUCI DMX");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 8);

    // Master dimmer
    s_masterDimmerLabel = lv_label_create(parent);
    lv_label_set_text(s_masterDimmerLabel, "Master Dimmer: 100%");
    lv_obj_align(s_masterDimmerLabel, LV_ALIGN_TOP_LEFT, 10, 44);

    lv_obj_t* dimmerSlider = lv_slider_create(parent);
    lv_slider_set_range(dimmerSlider, 0, 100);
    lv_slider_set_value(dimmerSlider, 100, LV_ANIM_OFF);
    lv_obj_set_size(dimmerSlider, 300, 20);
    lv_obj_align(dimmerSlider, LV_ALIGN_TOP_LEFT, 10, 68);
    lv_obj_add_event_cb(dimmerSlider, cb_master_dimmer, LV_EVENT_VALUE_CHANGED, NULL);

    // Bottone BLACKOUT
    lv_obj_t* btnBlackout = lv_btn_create(parent);
    lv_obj_align(btnBlackout, LV_ALIGN_TOP_LEFT, 330, 60);
    lv_obj_set_size(btnBlackout, 140, 36);
    lv_obj_set_style_bg_color(btnBlackout, lv_color_hex(0xCC0000), 0);
    lv_obj_add_event_cb(btnBlackout, cb_blackout, LV_EVENT_CLICKED, NULL);
    lv_obj_t* lblBO = lv_label_create(btnBlackout);
    lv_label_set_text(lblBO, LV_SYMBOL_WARNING " BLACKOUT");
    lv_obj_center(lblBO);

    // Indicatore audio-reactive
    s_arLabel = lv_label_create(parent);
    lv_label_set_text(s_arLabel, LV_SYMBOL_AUDIO " Audio-Reactive: OFF");
    lv_obj_align(s_arLabel, LV_ALIGN_TOP_LEFT, 490, 72);

    // Selettore scena
    lv_obj_t* sceneLabel = lv_label_create(parent);
    lv_label_set_text(sceneLabel, "Scena attiva:");
    lv_obj_align(sceneLabel, LV_ALIGN_TOP_LEFT, 10, 102);

    s_sceneDropdown = lv_dropdown_create(parent);
    lv_dropdown_set_options(s_sceneDropdown, "Nessuna\nColori Statici\nStrobo\nChase SX-DX\nRainbow");
    lv_obj_set_size(s_sceneDropdown, 250, 36);
    lv_obj_align(s_sceneDropdown, LV_ALIGN_TOP_LEFT, 120, 98);

    // Slider colore rapido (R, G, B)
    lv_obj_t* colorLabel = lv_label_create(parent);
    lv_label_set_text(colorLabel, "Colore fixture selezionata:");
    lv_obj_align(colorLabel, LV_ALIGN_TOP_LEFT, 10, 150);

    // Slider R
    lv_obj_t* lblR = lv_label_create(parent);
    lv_label_set_text(lblR, "R");
    lv_obj_align(lblR, LV_ALIGN_TOP_LEFT, 10, 182);
    lv_obj_t* sliderR = lv_slider_create(parent);
    lv_slider_set_range(sliderR, 0, 255);
    lv_slider_set_value(sliderR, 255, LV_ANIM_OFF);
    lv_obj_set_size(sliderR, 220, 16);
    lv_obj_align(sliderR, LV_ALIGN_TOP_LEFT, 28, 184);
    lv_obj_add_event_cb(sliderR, cb_slider_r, LV_EVENT_VALUE_CHANGED, NULL);

    // Slider G
    lv_obj_t* lblG = lv_label_create(parent);
    lv_label_set_text(lblG, "G");
    lv_obj_align(lblG, LV_ALIGN_TOP_LEFT, 10, 208);
    lv_obj_t* sliderG = lv_slider_create(parent);
    lv_slider_set_range(sliderG, 0, 255);
    lv_slider_set_value(sliderG, 0, LV_ANIM_OFF);
    lv_obj_set_size(sliderG, 220, 16);
    lv_obj_align(sliderG, LV_ALIGN_TOP_LEFT, 28, 210);
    (void)sliderG;

    // Slider B
    lv_obj_t* lblB = lv_label_create(parent);
    lv_label_set_text(lblB, "B");
    lv_obj_align(lblB, LV_ALIGN_TOP_LEFT, 10, 234);
    lv_obj_t* sliderB = lv_slider_create(parent);
    lv_slider_set_range(sliderB, 0, 255);
    lv_slider_set_value(sliderB, 0, LV_ANIM_OFF);
    lv_obj_set_size(sliderB, 220, 16);
    lv_obj_align(sliderB, LV_ALIGN_TOP_LEFT, 28, 236);
    (void)sliderB;

    // Lista fixture
    lv_obj_t* fixtureLabel = lv_label_create(parent);
    lv_label_set_text(fixtureLabel, "Fixture:");
    lv_obj_align(fixtureLabel, LV_ALIGN_TOP_LEFT, 270, 150);

    s_fixtureList = lv_list_create(parent);
    lv_obj_set_size(s_fixtureList, 500, 270);
    lv_obj_align(s_fixtureList, LV_ALIGN_TOP_LEFT, 270, 172);

    ui_dmx_update_fixtures();
}

void ui_dmx_update_fixtures() {
    if (!s_fixtureList) return;
    lv_obj_clean(s_fixtureList);

    auto fixtures = dmx_get_all_fixtures();
    if (fixtures.empty()) {
        lv_list_add_btn(s_fixtureList, LV_SYMBOL_WARNING, "Nessuna fixture configurata");
        return;
    }
    for (const auto* f : fixtures) {
        char buf[64];
        snprintf(buf, sizeof(buf), "%s [%s] @%d", f->name, f->type, f->startAddress);
        lv_list_add_btn(s_fixtureList, LV_SYMBOL_EYE_OPEN, buf);
    }
}
