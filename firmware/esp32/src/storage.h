#pragma once
#include "rs485.h"
#include <vector>

void storage_init();
void storage_save_assignments(const std::vector<SpeakerAssignment>& assignments);
std::vector<SpeakerAssignment> storage_load_assignments();
void storage_clear();
