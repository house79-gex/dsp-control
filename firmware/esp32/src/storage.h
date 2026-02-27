#pragma once
#include "rs485.h"
#include "dsp_control.h"
#include "dmx512.h"
#include "audio_reactive.h"
#include <vector>

void storage_init();
void storage_save_assignments(const std::vector<SpeakerAssignment>& assignments);
std::vector<SpeakerAssignment> storage_load_assignments();
void storage_clear();

// Preset DSP
void storage_save_dsp_presets(const std::vector<DspPreset>& presets);
std::vector<DspPreset> storage_load_dsp_presets();

// Fixture DMX
void storage_save_dmx_fixtures(const std::vector<DmxFixture*>& fixtures);
std::vector<DmxFixture> storage_load_dmx_fixtures();

// Scene DMX
void storage_save_dmx_scenes(const std::vector<DmxScene*>& scenes);
std::vector<DmxScene> storage_load_dmx_scenes();

// Gruppi DMX
void storage_save_dmx_groups(const std::vector<DmxGroup*>& groups);
std::vector<DmxGroup> storage_load_dmx_groups();

// Scenari audio-reactive
void storage_save_audio_reactive_scenarios(const std::vector<AudioReactiveScenario*>& scenarios);
std::vector<AudioReactiveScenario> storage_load_audio_reactive_scenarios();

// Configurazione sistema
struct SystemConfig {
    bool expertMode;
    char expertPassword[32];
    uint8_t masterVolume;
    int8_t masterBalance;
};
void storage_save_system_config(const SystemConfig& config);
SystemConfig storage_load_system_config();
