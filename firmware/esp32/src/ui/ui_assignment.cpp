#include "ui_assignment.h"
#include "../storage.h"
#include <cstring>

// Posizioni disponibili
static const char* POSITIONS[] = {
    "SX1", "SX2", "SX3", "DX1", "DX2", "DX3", "SUB1", "SUB2", "P1", "P2"
};
static const int POS_COUNT = 10;

// Tipi cassa
static const char* TYPES[] = { "2 VIE", "3 VIE", "SUB" };
static const char* TYPES_ID[] = { "2WAY", "3WAY", "SUB" };
static const int TYPE_COUNT = 3;

// Stato selezione corrente
static int  s_selPos  = 0;
static int  s_selType = 0;
static char s_name[32] = "";
static uint8_t s_deviceId = 0;

// Bottoni posizione e tipo (per evidenziare selezione)
static lv_obj_t* s_posButtons[POS_COUNT];
static lv_obj_t* s_typeButtons[TYPE_COUNT];

static void highlight_btn(lv_obj_t** btns, int count, int selected) {
    for (int i = 0; i < count; i++) {
        if (i == selected) {
            lv_obj_set_style_bg_color(btns[i], lv_color_hex(0xe94560), 0);
        } else {
            lv_obj_set_style_bg_color(btns[i], lv_color_hex(0x16213e), 0);
        }
    }
}

// Callbacks posizione
static void cb_pos(lv_event_t* e) {
    int idx = (int)(intptr_t)lv_event_get_user_data(e);
    s_selPos = idx;
    highlight_btn(s_posButtons, POS_COUNT, idx);
}

// Callbacks tipo
static void cb_type(lv_event_t* e) {
    int idx = (int)(intptr_t)lv_event_get_user_data(e);
    s_selType = idx;
    highlight_btn(s_typeButtons, TYPE_COUNT, idx);
}

// Callback SALVA
static void cb_save(lv_event_t* e) {
    lv_obj_t* nameField = (lv_obj_t*)lv_event_get_user_data(e);
    const char* nameText = lv_textarea_get_text(nameField);

    SpeakerAssignment a;
    a.deviceId = s_deviceId;
    a.position = POSITIONS[s_selPos];
    a.type     = TYPES_ID[s_selType];
    a.name     = (nameText && nameText[0]) ? nameText : ("Cassa " + std::to_string(s_deviceId));

    if (a.type == "SUB")                        a.role = SpeakerRole::SUB;
    else if (s_selPos >= 0 && s_selPos <= 2)    a.role = SpeakerRole::SX;
    else if (s_selPos >= 3 && s_selPos <= 5)    a.role = SpeakerRole::DX;
    else                                         a.role = SpeakerRole::MONO;

    // Carica lista esistente, sostituisce o aggiunge, poi salva
    auto list = storage_load_assignments();
    bool found = false;
    for (auto& existing : list) {
        if (existing.deviceId == s_deviceId) {
            existing = a;
            found = true;
            break;
        }
    }
    if (!found) list.push_back(a);
    storage_save_assignments(list);

    Serial.printf("[UI] Assegnazione salvata: ID=%d pos=%s tipo=%s nome=%s\n",
                  a.deviceId, a.position.c_str(), a.type.c_str(), a.name.c_str());
}

// Callback AVANTI (beep di conferma + placeholder navigazione)
static void cb_next(lv_event_t* e) {
    rs485_send_beep(s_deviceId, BeepPattern::Triple);
    Serial.printf("[UI] AVANTI → prossimo dispositivo (TODO: navigazione)\n");
}

void ui_assignment_create(lv_obj_t* parent, uint8_t deviceId) {
    s_deviceId = deviceId;

    // Titolo
    lv_obj_t* title = lv_label_create(parent);
    char titleBuf[40];
    snprintf(titleBuf, sizeof(titleBuf), "ASSEGNA DISPOSITIVO ID %d", deviceId);
    lv_label_set_text(title, titleBuf);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 8);

    // Bottoni posizione
    lv_obj_t* posLabel = lv_label_create(parent);
    lv_label_set_text(posLabel, "Posizione:");
    lv_obj_align(posLabel, LV_ALIGN_TOP_LEFT, 10, 40);

    for (int i = 0; i < POS_COUNT; i++) {
        s_posButtons[i] = lv_btn_create(parent);
        lv_obj_set_size(s_posButtons[i], 70, 40);
        lv_obj_set_pos(s_posButtons[i], 10 + (i % 5) * 78, 62 + (i / 5) * 48);
        lv_obj_add_event_cb(s_posButtons[i], cb_pos, LV_EVENT_CLICKED, (void*)(intptr_t)i);

        lv_obj_t* lbl = lv_label_create(s_posButtons[i]);
        lv_label_set_text(lbl, POSITIONS[i]);
        lv_obj_center(lbl);
    }
    highlight_btn(s_posButtons, POS_COUNT, 0);

    // Bottoni tipo cassa
    lv_obj_t* typeLabel = lv_label_create(parent);
    lv_label_set_text(typeLabel, "Tipo:");
    lv_obj_align(typeLabel, LV_ALIGN_TOP_LEFT, 10, 166);

    for (int i = 0; i < TYPE_COUNT; i++) {
        s_typeButtons[i] = lv_btn_create(parent);
        lv_obj_set_size(s_typeButtons[i], 110, 40);
        lv_obj_set_pos(s_typeButtons[i], 10 + i * 118, 188);
        lv_obj_add_event_cb(s_typeButtons[i], cb_type, LV_EVENT_CLICKED, (void*)(intptr_t)i);

        lv_obj_t* lbl = lv_label_create(s_typeButtons[i]);
        lv_label_set_text(lbl, TYPES[i]);
        lv_obj_center(lbl);
    }
    highlight_btn(s_typeButtons, TYPE_COUNT, 0);

    // Campo nome personalizzato
    lv_obj_t* nameLabel = lv_label_create(parent);
    lv_label_set_text(nameLabel, "Nome:");
    lv_obj_align(nameLabel, LV_ALIGN_TOP_LEFT, 10, 244);

    lv_obj_t* nameField = lv_textarea_create(parent);
    lv_textarea_set_one_line(nameField, true);
    lv_textarea_set_placeholder_text(nameField, "es. Palco Sinistra");
    lv_obj_set_size(nameField, 400, 44);
    lv_obj_set_pos(nameField, 10, 266);

    // Pulsanti SALVA e AVANTI
    lv_obj_t* saveBtn = lv_btn_create(parent);
    lv_obj_set_size(saveBtn, 160, 48);
    lv_obj_set_pos(saveBtn, 10, 326);
    lv_obj_add_event_cb(saveBtn, cb_save, LV_EVENT_CLICKED, nameField);
    lv_obj_t* saveLbl = lv_label_create(saveBtn);
    lv_label_set_text(saveLbl, LV_SYMBOL_SAVE " SALVA");
    lv_obj_center(saveLbl);

    lv_obj_t* nextBtn = lv_btn_create(parent);
    lv_obj_set_size(nextBtn, 160, 48);
    lv_obj_set_pos(nextBtn, 182, 326);
    lv_obj_add_event_cb(nextBtn, cb_next, LV_EVENT_CLICKED, NULL);
    lv_obj_t* nextLbl = lv_label_create(nextBtn);
    lv_label_set_text(nextLbl, "AVANTI " LV_SYMBOL_NEXT);
    lv_obj_center(nextLbl);
}
