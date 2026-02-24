#include "ui_home.h"
#include "../rs485.h"

// Dati demo per popolamento lista casse
static const char* DEMO_SPEAKERS[] = {
    "SX1 – Cassa 2-vie", "DX1 – Cassa 2-vie", "SUB1 – Subwoofer"
};
static const int DEMO_COUNT = 3;

// Oggetti LVGL persistenti per aggiornamento dinamico
static lv_obj_t* s_speakerList = nullptr;
static lv_obj_t* s_masterLabel = nullptr;
static lv_obj_t* s_balanceLabel = nullptr;

// Callback slider volume master
static void cb_master_vol(lv_event_t* e) {
    lv_obj_t* slider = lv_event_get_target(e);
    int val = lv_slider_get_value(slider);
    char buf[24];
    snprintf(buf, sizeof(buf), "Volume: %d%%", val);
    lv_label_set_text(s_masterLabel, buf);
    // TODO: propagare valore via API / RS-485
}

// Callback slider balance
static void cb_balance(lv_event_t* e) {
    lv_obj_t* slider = lv_event_get_target(e);
    int val = lv_slider_get_value(slider);
    char buf[24];
    if (val == 0)       snprintf(buf, sizeof(buf), "Balance: Centro");
    else if (val < 0)   snprintf(buf, sizeof(buf), "Balance: SX %d", -val);
    else                snprintf(buf, sizeof(buf), "Balance: DX %d",  val);
    lv_label_set_text(s_balanceLabel, buf);
    // TODO: propagare balance SX/DX via RS-485
}

void ui_home_create(lv_obj_t* parent) {
    // Titolo
    lv_obj_t* title = lv_label_create(parent);
    lv_label_set_text(title, "SISTEMA AUDIO PA");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 8);

    // Slider volume master
    s_masterLabel = lv_label_create(parent);
    lv_label_set_text(s_masterLabel, "Volume: 80%");
    lv_obj_align(s_masterLabel, LV_ALIGN_TOP_LEFT, 10, 44);

    lv_obj_t* masterSlider = lv_slider_create(parent);
    lv_slider_set_range(masterSlider, 0, 100);
    lv_slider_set_value(masterSlider, 80, LV_ANIM_OFF);
    lv_obj_set_size(masterSlider, 380, 20);
    lv_obj_align(masterSlider, LV_ALIGN_TOP_LEFT, 10, 68);
    lv_obj_add_event_cb(masterSlider, cb_master_vol, LV_EVENT_VALUE_CHANGED, NULL);

    // Slider balance SX/DX
    s_balanceLabel = lv_label_create(parent);
    lv_label_set_text(s_balanceLabel, "Balance: Centro");
    lv_obj_align(s_balanceLabel, LV_ALIGN_TOP_LEFT, 10, 102);

    lv_obj_t* balSlider = lv_slider_create(parent);
    lv_slider_set_range(balSlider, -50, 50);
    lv_slider_set_value(balSlider, 0, LV_ANIM_OFF);
    lv_obj_set_size(balSlider, 380, 20);
    lv_obj_align(balSlider, LV_ALIGN_TOP_LEFT, 10, 126);
    lv_obj_add_event_cb(balSlider, cb_balance, LV_EVENT_VALUE_CHANGED, NULL);

    // Lista casse
    lv_obj_t* listLabel = lv_label_create(parent);
    lv_label_set_text(listLabel, "Casse connesse:");
    lv_obj_align(listLabel, LV_ALIGN_TOP_LEFT, 10, 162);

    s_speakerList = lv_list_create(parent);
    lv_obj_set_size(s_speakerList, 780, 260);
    lv_obj_align(s_speakerList, LV_ALIGN_TOP_LEFT, 10, 186);

    // Popola con dati demo
    ui_home_update_speakers();
}

void ui_home_update_speakers() {
    if (!s_speakerList) return;
    lv_obj_clean(s_speakerList);

    // TODO: usare g_assignments reali al posto dei dati demo
    for (int i = 0; i < DEMO_COUNT; i++) {
        lv_list_add_btn(s_speakerList, LV_SYMBOL_AUDIO, DEMO_SPEAKERS[i]);
    }
}
