#include "ui_wled.h"
#include "../wled_client.h"

static lv_obj_t* s_ctrlList        = nullptr;
static lv_obj_t* s_brightLabel     = nullptr;
static lv_obj_t* s_sceneDropdown   = nullptr;

// Callback master brightness
static void cb_brightness(lv_event_t* e) {
    lv_obj_t* slider = lv_event_get_target(e);
    int val = lv_slider_get_value(slider);
    char buf[48];
    snprintf(buf, sizeof(buf), "Luminosità master: %d%%", val);
    lv_label_set_text(s_brightLabel, buf);
    uint8_t bri = (uint8_t)(val * 255 / 100);
    uint8_t n = wled_get_controller_count();
    for (uint8_t i = 0; i < n; i++) {
        wled_client_set_brightness(i, ZONE_LOGO_LEFT,   bri);
        wled_client_set_brightness(i, ZONE_LOGO_RIGHT,  bri);
        wled_client_set_brightness(i, ZONE_FRONT_FRAME, bri);
    }
}

// Callback BLACKOUT
static void cb_blackout(lv_event_t* e) {
    (void)e;
    wled_client_blackout();
    Serial.println("[UI_WLED] BLACKOUT attivato");
}

// Callback SYNC
static void cb_sync(lv_event_t* e) {
    (void)e;
    wled_client_sync_all();
    Serial.println("[UI_WLED] SYNC tutti i controller");
}

// Callback DISCOVER
static void cb_discover(lv_event_t* e) {
    (void)e;
    wled_client_discover();
    ui_wled_update_controllers();
}

void ui_wled_create(lv_obj_t* parent) {
    // Titolo
    lv_obj_t* title = lv_label_create(parent);
    lv_label_set_text(title, "CONTROLLO NEON LED");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 8);

    // Master brightness
    s_brightLabel = lv_label_create(parent);
    lv_label_set_text(s_brightLabel, "Luminosità master: 100%");
    lv_obj_align(s_brightLabel, LV_ALIGN_TOP_LEFT, 10, 44);

    lv_obj_t* brightSlider = lv_slider_create(parent);
    lv_slider_set_range(brightSlider, 0, 100);
    lv_slider_set_value(brightSlider, 100, LV_ANIM_OFF);
    lv_obj_set_size(brightSlider, 300, 20);
    lv_obj_align(brightSlider, LV_ALIGN_TOP_LEFT, 10, 68);
    lv_obj_add_event_cb(brightSlider, cb_brightness, LV_EVENT_VALUE_CHANGED, NULL);

    // Bottone BLACKOUT
    lv_obj_t* btnBlackout = lv_btn_create(parent);
    lv_obj_align(btnBlackout, LV_ALIGN_TOP_LEFT, 330, 60);
    lv_obj_set_size(btnBlackout, 130, 36);
    lv_obj_set_style_bg_color(btnBlackout, lv_color_hex(0xCC0000), 0);
    lv_obj_add_event_cb(btnBlackout, cb_blackout, LV_EVENT_CLICKED, NULL);
    lv_obj_t* lblBO = lv_label_create(btnBlackout);
    lv_label_set_text(lblBO, LV_SYMBOL_WARNING " BLACKOUT");
    lv_obj_center(lblBO);

    // Bottone SYNC
    lv_obj_t* btnSync = lv_btn_create(parent);
    lv_obj_align(btnSync, LV_ALIGN_TOP_LEFT, 476, 60);
    lv_obj_set_size(btnSync, 110, 36);
    lv_obj_set_style_bg_color(btnSync, lv_color_hex(0x0055CC), 0);
    lv_obj_add_event_cb(btnSync, cb_sync, LV_EVENT_CLICKED, NULL);
    lv_obj_t* lblSync = lv_label_create(btnSync);
    lv_label_set_text(lblSync, LV_SYMBOL_REFRESH " SYNC");
    lv_obj_center(lblSync);

    // Selettore scena
    lv_obj_t* sceneLabel = lv_label_create(parent);
    lv_label_set_text(sceneLabel, "Scena:");
    lv_obj_align(sceneLabel, LV_ALIGN_TOP_LEFT, 10, 110);

    s_sceneDropdown = lv_dropdown_create(parent);
    lv_dropdown_set_options(s_sceneDropdown,
        "Rosso solido\nBlu solido\nVerde solido\nRainbow\nFire\nBreathing");
    lv_obj_set_size(s_sceneDropdown, 260, 36);
    lv_obj_align(s_sceneDropdown, LV_ALIGN_TOP_LEFT, 70, 106);

    // Lista controller (stub scrollabile)
    lv_obj_t* ctrlLabel = lv_label_create(parent);
    lv_label_set_text(ctrlLabel, "Controller collegati:");
    lv_obj_align(ctrlLabel, LV_ALIGN_TOP_LEFT, 10, 158);

    s_ctrlList = lv_list_create(parent);
    lv_obj_set_size(s_ctrlList, 380, 180);
    lv_obj_align(s_ctrlList, LV_ALIGN_TOP_LEFT, 10, 180);

    // Bottone DISCOVER
    lv_obj_t* btnDiscover = lv_btn_create(parent);
    lv_obj_align(btnDiscover, LV_ALIGN_TOP_LEFT, 400, 158);
    lv_obj_set_size(btnDiscover, 160, 36);
    lv_obj_set_style_bg_color(btnDiscover, lv_color_hex(0x226622), 0);
    lv_obj_add_event_cb(btnDiscover, cb_discover, LV_EVENT_CLICKED, NULL);
    lv_obj_t* lblDisc = lv_label_create(btnDiscover);
    lv_label_set_text(lblDisc, LV_SYMBOL_WIFI " DISCOVER");
    lv_obj_center(lblDisc);

    ui_wled_update_controllers();
}

void ui_wled_update_controllers() {
    if (!s_ctrlList) return;
    lv_list_clean(s_ctrlList);

    uint8_t n = wled_get_controller_count();
    if (n == 0) {
        lv_list_add_text(s_ctrlList, "Nessun controller rilevato");
        return;
    }
    for (uint8_t i = 0; i < n; i++) {
        WledController* ctrl = wled_get_controller(i);
        char buf[64];
        snprintf(buf, sizeof(buf), "%s  %s  [%s]",
                 ctrl->name, ctrl->ip,
                 ctrl->online ? "Online" : "Offline");
        lv_list_add_btn(s_ctrlList,
                        ctrl->online ? LV_SYMBOL_OK : LV_SYMBOL_CLOSE,
                        buf);
    }
}
