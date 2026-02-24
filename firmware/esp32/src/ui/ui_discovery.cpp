#include "ui_discovery.h"
#include "../rs485.h"

static lv_obj_t* s_resultList   = nullptr;
static lv_obj_t* s_statusLabel  = nullptr;

// Callback pulsante scansione
static void cb_scan(lv_event_t* e) {
    if (!s_statusLabel || !s_resultList) return;

    lv_label_set_text(s_statusLabel, "Scansione RS-485 in corso...");
    lv_obj_clean(s_resultList);

    // Avvia discovery (blocca brevemente – accettabile per scansione manuale)
    std::vector<SpeakerDevice> devices = rs485_scan_devices();

    if (devices.empty()) {
        lv_label_set_text(s_statusLabel, "Nessun dispositivo trovato");
        return;
    }

    char statusBuf[48];
    snprintf(statusBuf, sizeof(statusBuf), "Trovati: %d dispositivi", (int)devices.size());
    lv_label_set_text(s_statusLabel, statusBuf);

    for (const auto& dev : devices) {
        char buf[32];
        snprintf(buf, sizeof(buf), "ID %d – %s", dev.id, dev.type.c_str());
        lv_list_add_btn(s_resultList, LV_SYMBOL_WIFI, buf);
    }
}

void ui_discovery_create(lv_obj_t* parent) {
    // Titolo
    lv_obj_t* title = lv_label_create(parent);
    lv_label_set_text(title, "DISCOVERY RS-485");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 8);

    // Pulsante SCANSIONE
    lv_obj_t* scanBtn = lv_btn_create(parent);
    lv_obj_set_size(scanBtn, 300, 50);
    lv_obj_align(scanBtn, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_add_event_cb(scanBtn, cb_scan, LV_EVENT_CLICKED, NULL);

    lv_obj_t* scanLabel = lv_label_create(scanBtn);
    lv_label_set_text(scanLabel, LV_SYMBOL_REFRESH " SCANSIONE RS-485");
    lv_obj_center(scanLabel);

    // Label di stato
    s_statusLabel = lv_label_create(parent);
    lv_label_set_text(s_statusLabel, "Premi SCANSIONE per cercare i dispositivi");
    lv_obj_align(s_statusLabel, LV_ALIGN_TOP_MID, 0, 116);

    // Lista risultati
    s_resultList = lv_list_create(parent);
    lv_obj_set_size(s_resultList, 780, 300);
    lv_obj_align(s_resultList, LV_ALIGN_TOP_LEFT, 10, 148);
}
