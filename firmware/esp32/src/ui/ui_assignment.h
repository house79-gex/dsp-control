#pragma once
#include <lvgl.h>
#include "../rs485.h"

// Schermata Assignment: assegnazione posizione, tipo e nome a un dispositivo
// deviceId: ID del dispositivo RS-485 da assegnare
void ui_assignment_create(lv_obj_t* parent, uint8_t deviceId = 0);
