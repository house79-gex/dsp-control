#pragma once
#include <lvgl.h>

// Schermata Home: volume master, balance SX/DX, lista casse
void ui_home_create(lv_obj_t* parent);
void ui_home_update_speakers();  // chiama dopo discovery per aggiornare la lista
