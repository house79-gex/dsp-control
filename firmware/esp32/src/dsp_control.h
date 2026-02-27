#pragma once
#include <string>
#include <cstdint>
#include "dsp_protocol.h"
#include "dsp_registers.h"

// ======= Controllo DSP CQ260D – livello applicazione =======
// Gestisce preset, parametri e controllo di gruppo/individuale/totale

// ——— Struttura preset DSP ———
struct DspPreset {
    char     name[32];          // Nome preset
    char     description[64];   // Descrizione
    SysReg   parameters;        // Parametri completi SYSREG
    bool     isFactory;         // true = preset di fabbrica (non eliminabile)
};

// ——— Tipi di controllo ———
enum class DspControlType {
    Single,   // Cassa singola
    Group,    // Gruppo (SX, DX, SUB)
    All       // Tutte le casse
};

enum class DspGroupType {
    SX,   // Casse sinistra
    DX,   // Casse destra
    SUB,  // Subwoofer
    ALL   // Tutte
};

// ——— API pubblica ———

void dsp_init();

// Applica un preset per nome a un ID specifico o broadcast
void dsp_apply_preset(const std::string& presetName, uint8_t deviceId = 0xFF);

// Controllo parametri singola cassa
void dsp_set_speaker_gain(uint8_t id, float db);
void dsp_set_speaker_hpf(uint8_t id, float freqHz, uint8_t slope = 1);
void dsp_set_speaker_lpf(uint8_t id, float freqHz, uint8_t slope = 1);
void dsp_set_speaker_delay(uint8_t id, float delayMs);
void dsp_set_speaker_mute(uint8_t id, bool muted);
void dsp_set_speaker_phase(uint8_t id, bool inverted);
void dsp_set_eq_band(uint8_t id, uint8_t band, uint16_t freqHz, float gainDb, float q, uint8_t type);
void dsp_set_compressor(uint8_t id, uint8_t threshold, uint8_t ratio, uint8_t attack, uint8_t release, uint8_t makeup);
void dsp_set_noise_gate(uint8_t id, uint8_t threshold, uint8_t attack, uint8_t decay);

// Controllo di gruppo
void dsp_set_group_gain(DspGroupType group, float db);
void dsp_set_group_mute(DspGroupType group, bool muted);
void dsp_apply_preset_group(DspGroupType group, const std::string& presetName);

// Lista preset disponibili
std::vector<std::string> dsp_get_preset_names();

// Salva preset personalizzato
bool dsp_save_preset(const std::string& name, const std::string& description);

// Elimina preset (solo se non factory)
bool dsp_delete_preset(const std::string& name);

// Restituisce un preset per nome (nullptr se non trovato)
const DspPreset* dsp_get_preset(const std::string& name);

// Imposta il tipo di controllo (base/esperto) — password opzionale
bool dsp_set_expert_mode(bool expert, const char* password = nullptr);
bool dsp_is_expert_mode();
