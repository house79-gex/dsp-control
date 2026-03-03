#pragma once
#include "rs485.h"
#include "dsp_control.h"
#include "dmx512.h"
#include "audio_reactive.h"
#include "autotune.h"
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

// Calibrazione microfono di misura
void storage_save_mic_calibration(const MicCalibration& cal);
bool storage_load_mic_calibration(MicCalibration& cal);
void storage_clear_mic_calibration();

// Controller e scene WLED
#include "wled_client.h"
void storage_save_wled_controllers();
void storage_load_wled_controllers();
void storage_save_wled_scenes();
void storage_load_wled_scenes();

// Configurazione wireless TX
#include "wireless_tx.h"
void storage_save_wireless_config(const WirelessConfig& cfg);
void storage_load_wireless_config(WirelessConfig& cfg);

// Configurazione audio (sample rate, bit depth, input gain ADC)
struct AudioConfig {
    float    inputGainDb;  // Guadagno ADC ES8388 (-96..+24 dB)
    uint32_t sampleRate;   // Hz (es. 44100)
    uint8_t  bitDepth;     // bit (es. 24)
};
bool storage_save_audio_config(float inputGainDb, uint32_t sampleRate, uint8_t bitDepth);
bool storage_load_audio_config(float* inputGainDb, uint32_t* sampleRate, uint8_t* bitDepth);

// Venue Map (mappa sala con posizioni speaker/luci)
struct SpeakerPosition {
    uint8_t id;
    char    name[32];
    float   x;       // posizione X in metri dalla parete sinistra
    float   y;       // posizione Y in metri dalla parete di fondo
    float   angle;   // angolo puntamento in gradi (0=fronte, 90=destra)
};
struct LightPosition {
    uint8_t id;
    char    name[32];
    float   x;
    float   y;
    float   height;  // altezza dal suolo (m)
    float   angle;
};
struct VenueMap {
    float roomWidth;   // larghezza sala in metri
    float roomDepth;   // profondità sala in metri
    std::vector<SpeakerPosition> speakers;
    std::vector<LightPosition>   lights;
};
void storage_save_venue_map(const VenueMap& map);
VenueMap storage_load_venue_map();
