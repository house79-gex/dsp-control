#include "dsp_control.h"
#include "dsp_protocol.h"
#include "storage.h"
#include <Arduino.h>
#include <vector>
#include <string>
#include <cmath>

// ======= Variabili di stato =======

static bool s_expertMode = false;    // Modalità esperto attiva
static DspId s_defaultId = { 0, 0 }; // ID DSP di default (broadcast)

// ======= Preset di fabbrica predefiniti =======
// 10 preset standard come richiesto dalle specifiche

static DspPreset s_factoryPresets[] = {
    {
        "2WAY_STD", "Cassa 2 vie standard (crossover 1.2kHz, HPF 80Hz)",
        { /* SysReg vuoto con valori default */ },
        true
    },
    {
        "3WAY_STD", "Cassa 3 vie (crossover 300Hz/3kHz)",
        { }, true
    },
    {
        "SUB_STD", "Subwoofer (LPF 120Hz, senza HPF)",
        { }, true
    },
    {
        "MONITOR", "Monitor palco (HPF 100Hz, boost medi +3dB)",
        { }, true
    },
    {
        "OUTDOOR", "Outdoor (boost HF +2dB per compensare dispersione)",
        { }, true
    },
    {
        "SPEECH", "Parlato (HPF 120Hz, compressore leggero, de-esser)",
        { }, true
    },
    {
        "MUSIC", "Musica (EQ flat, compressore soft)",
        { }, true
    },
    {
        "DJ", "DJ (sub boost +6dB, limitatore aggressivo)",
        { }, true
    },
    {
        "CONCERT", "Concerto live (EQ calibrato, delay allineamento)",
        { }, true
    },
    {
        "CUSTOM_1", "Slot personalizzabile 1",
        { }, false
    },
    {
        "CUSTOM_2", "Slot personalizzabile 2",
        { }, false
    },
    {
        "CUSTOM_3", "Slot personalizzabile 3",
        { }, false
    },
};

static const size_t NUM_FACTORY_PRESETS = sizeof(s_factoryPresets) / sizeof(DspPreset);

// Preset utente aggiuntivi (in RAM, salvati su NVS)
static std::vector<DspPreset> s_userPresets;

// ======= Funzioni helper interne =======

// Converte dB in valore uint16 per SYSREG (range -60..+12 dB → 0..65535)
static uint16_t db_to_uint16(float db) {
    float normalized = (db + 60.0f) / 72.0f;  // Normalizza a 0..1
    normalized = fmaxf(0.0f, fminf(1.0f, normalized));
    return (uint16_t)(normalized * 65535.0f);
}

// Converte frequenza Hz in valore uint16 per SYSREG
static uint16_t freq_to_uint16(float freqHz) {
    return (uint16_t)fminf(freqHz, 65535.0f);
}

// Converte delay ms in campioni (48kHz)
static uint16_t ms_to_samples(float delayMs) {
    return (uint16_t)(delayMs * 48.0f);  // 48 campioni/ms a 48kHz
}

// Invia un parametro singolo al DSP tramite protocollo
static void send_param(uint8_t deviceId, uint8_t paramCmd, uint8_t channel,
                        uint16_t value) {
    DspId id = { 0, deviceId };
    uint8_t payload[] = {
        4,                              // Lunghezza payload
        DSP_TXCMD_PARAM_SET,            // Comando imposta parametro
        paramCmd,                       // Tipo parametro
        channel,                        // Canale
        (uint8_t)(value & 0xFF),        // Valore LE basso
        (uint8_t)(value >> 8)           // Valore LE alto
    };
    dsp_command_send(payload, sizeof(payload), id);
}

// ======= Inizializzazione =======

void dsp_init() {
    dsp_protocol_init();

    // Inizializza preset di fabbrica con valori sensibili
    // 2WAY_STD: HPF 80Hz, crossover a 1.2kHz
    s_factoryPresets[0].parameters.HLPF_Freq[0] = 80;    // HPF ingresso 1
    s_factoryPresets[0].parameters.HLPF_Type[0] = 1;     // 24dB/oct

    // SUB_STD: LPF 120Hz
    s_factoryPresets[2].parameters.HLPF_Freq[4] = 120;   // LPF uscita 1
    s_factoryPresets[2].parameters.HLPF_Type[4] = 1;     // 24dB/oct

    // MONITOR: HPF 100Hz, EQ boost medi +3dB @ 2kHz
    s_factoryPresets[3].parameters.HLPF_Freq[0] = 100;
    s_factoryPresets[3].parameters.EQ_Freq[0]   = 2000;
    s_factoryPresets[3].parameters.EQ_Gain[0]   = db_to_uint16(3.0f);

    // Connessione broadcast iniziale (tenta, non bloccante)
    DspId broadcastId = { 0, 0 };
    Serial.println("[DSP] Tentativo connessione broadcast CQ260D...");
    if (dsp_connect(broadcastId)) {
        Serial.println("[DSP] CQ260D connesso");
        dsp_upload_parameters(broadcastId);
    } else {
        Serial.println("[DSP] CQ260D non risponde (verificare cablaggio RS-485)");
    }
}

// ======= Controllo parametri ======= 

void dsp_apply_preset(const std::string& presetName, uint8_t deviceId) {
    Serial.printf("[DSP] Applicazione preset '%s' a ID=%d\n", presetName.c_str(), deviceId);

    const DspPreset* preset = dsp_get_preset(presetName);
    if (!preset) {
        Serial.printf("[DSP] Preset '%s' non trovato\n", presetName.c_str());
        return;
    }

    // Copia i parametri nel SYSREG locale
    memcpy(dsp_get_sysreg(), &preset->parameters, sizeof(SysReg));

    // Scarica parametri al DSP
    DspId id = { 0, deviceId };
    dsp_download_parameters(id);
}

void dsp_set_speaker_gain(uint8_t id, float db) {
    Serial.printf("[DSP] Gain cassa ID=%d → %.1f dB\n", id, db);
    uint16_t val = db_to_uint16(db);
    // Imposta gain ingresso (canale 0 per semplicità; nella realtà mappare id→canale)
    dsp_get_sysreg()->OutGain[0] = val;
    send_param(id, 0x01, 0, val);
}

void dsp_set_speaker_hpf(uint8_t id, float freqHz, uint8_t slope) {
    Serial.printf("[DSP] HPF cassa ID=%d → %.0f Hz (pendenza %d)\n", id, freqHz, slope);
    uint16_t freq = freq_to_uint16(freqHz);
    dsp_get_sysreg()->HLPF_Freq[0] = freq;
    dsp_get_sysreg()->HLPF_Type[0] = slope;
    send_param(id, 0x10, 0, freq);
    send_param(id, 0x11, 0, slope);
}

void dsp_set_speaker_lpf(uint8_t id, float freqHz, uint8_t slope) {
    Serial.printf("[DSP] LPF cassa ID=%d → %.0f Hz (pendenza %d)\n", id, freqHz, slope);
    uint16_t freq = freq_to_uint16(freqHz);
    dsp_get_sysreg()->HLPF_Freq[4] = freq;
    dsp_get_sysreg()->HLPF_Type[4] = slope;
    send_param(id, 0x12, 0, freq);
    send_param(id, 0x13, 0, slope);
}

void dsp_set_speaker_delay(uint8_t id, float delayMs) {
    Serial.printf("[DSP] Delay cassa ID=%d → %.2f ms\n", id, delayMs);
    uint16_t samples = ms_to_samples(delayMs);
    dsp_get_sysreg()->Delay[0] = samples;
    send_param(id, 0x20, 0, samples);
}

void dsp_set_speaker_mute(uint8_t id, bool muted) {
    Serial.printf("[DSP] Mute cassa ID=%d → %s\n", id, muted ? "SI" : "NO");
    dsp_get_sysreg()->Mute[0] = muted ? 1 : 0;
    send_param(id, 0x30, 0, muted ? 1 : 0);
}

void dsp_set_speaker_phase(uint8_t id, bool inverted) {
    Serial.printf("[DSP] Fase cassa ID=%d → %s\n", id, inverted ? "INVERTITA" : "NORMALE");
    dsp_get_sysreg()->Phase[0] = inverted ? 1 : 0;
    send_param(id, 0x31, 0, inverted ? 1 : 0);
}

void dsp_set_eq_band(uint8_t id, uint8_t band, uint16_t freqHz, float gainDb, float q, uint8_t type) {
    if (band >= 28) return;  // Max 28 bande uscita
    Serial.printf("[DSP] EQ band %d: freq=%dHz gain=%.1fdB Q=%.1f\n", band, freqHz, gainDb, q);
    dsp_get_sysreg()->EQ_Freq[band] = freqHz;
    dsp_get_sysreg()->EQ_Gain[band] = db_to_uint16(gainDb);
    dsp_get_sysreg()->EQ_Q[band]    = (uint8_t)(q * 10);
    dsp_get_sysreg()->EQ_Type[band] = type;
    send_param(id, 0x40 + band, 0, freqHz);
    send_param(id, 0x40 + band, 1, db_to_uint16(gainDb));
}

void dsp_set_compressor(uint8_t id, uint8_t threshold, uint8_t ratio,
                         uint8_t attack, uint8_t release, uint8_t makeup) {
    Serial.printf("[DSP] Compressore ID=%d: thr=%d ratio=%d\n", id, threshold, ratio);
    dsp_get_sysreg()->Threhold[0]   = threshold;
    dsp_get_sysreg()->ATack[0]      = attack;
    dsp_get_sysreg()->Decay[0]      = release;
    dsp_get_sysreg()->CompRatio[0]  = ratio;
    dsp_get_sysreg()->CompMakeup[0] = makeup;
    send_param(id, 0x50, 0, threshold);
    send_param(id, 0x51, 0, ratio);
    send_param(id, 0x52, 0, attack);
    send_param(id, 0x53, 0, release);
    send_param(id, 0x54, 0, makeup);
}

void dsp_set_noise_gate(uint8_t id, uint8_t threshold, uint8_t attack, uint8_t decay) {
    Serial.printf("[DSP] Noise gate ID=%d: thr=%d\n", id, threshold);
    dsp_get_sysreg()->Threhold_Noise[0] = threshold;
    dsp_get_sysreg()->ATack_Noise[0]    = attack;
    dsp_get_sysreg()->Decay_Noise[0]    = decay;
    send_param(id, 0x60, 0, threshold);
    send_param(id, 0x61, 0, attack);
    send_param(id, 0x62, 0, decay);
}

void dsp_set_group_gain(DspGroupType group, float db) {
    Serial.printf("[DSP] Gain gruppo → %.1f dB\n", db);
    // Invia a tutti i dispositivi del gruppo (ID 1-8 per SX, 9-16 per DX, ecc.)
    uint8_t startId = 1, endId = 8;
    switch (group) {
        case DspGroupType::SX:  startId = 1;  endId = 8;   break;
        case DspGroupType::DX:  startId = 9;  endId = 16;  break;
        case DspGroupType::SUB: startId = 17; endId = 20;  break;
        case DspGroupType::ALL: startId = 1;  endId = 127; break;
    }
    for (uint8_t id = startId; id <= endId; id++) {
        dsp_set_speaker_gain(id, db);
    }
}

void dsp_set_group_mute(DspGroupType group, bool muted) {
    Serial.printf("[DSP] Mute gruppo → %s\n", muted ? "SI" : "NO");
    uint8_t startId = 1, endId = 127;
    if (group == DspGroupType::SX)  { startId = 1;  endId = 8;   }
    if (group == DspGroupType::DX)  { startId = 9;  endId = 16;  }
    if (group == DspGroupType::SUB) { startId = 17; endId = 20;  }
    for (uint8_t id = startId; id <= endId; id++) {
        dsp_set_speaker_mute(id, muted);
    }
}

void dsp_apply_preset_group(DspGroupType group, const std::string& presetName) {
    Serial.printf("[DSP] Preset '%s' a gruppo\n", presetName.c_str());
    uint8_t startId = 1, endId = 127;
    if (group == DspGroupType::SX)  { startId = 1;  endId = 8;   }
    if (group == DspGroupType::DX)  { startId = 9;  endId = 16;  }
    if (group == DspGroupType::SUB) { startId = 17; endId = 20;  }
    for (uint8_t id = startId; id <= endId; id++) {
        dsp_apply_preset(presetName, id);
    }
}

// ======= Gestione preset =======

std::vector<std::string> dsp_get_preset_names() {
    std::vector<std::string> names;
    for (size_t i = 0; i < NUM_FACTORY_PRESETS; i++) {
        names.push_back(std::string(s_factoryPresets[i].name));
    }
    for (const auto& p : s_userPresets) {
        names.push_back(std::string(p.name));
    }
    return names;
}

const DspPreset* dsp_get_preset(const std::string& name) {
    for (size_t i = 0; i < NUM_FACTORY_PRESETS; i++) {
        if (name == s_factoryPresets[i].name) {
            return &s_factoryPresets[i];
        }
    }
    for (const auto& p : s_userPresets) {
        if (name == p.name) return &p;
    }
    return nullptr;
}

bool dsp_save_preset(const std::string& name, const std::string& description) {
    // Controlla se esiste già un preset utente con questo nome
    for (auto& p : s_userPresets) {
        if (name == p.name) {
            // Aggiorna preset esistente
            strncpy(p.description, description.c_str(), sizeof(p.description) - 1);
            memcpy(&p.parameters, dsp_get_sysreg(), sizeof(SysReg));
            Serial.printf("[DSP] Preset '%s' aggiornato\n", name.c_str());
            return true;
        }
    }
    // Nuovo preset
    DspPreset newPreset;
    strncpy(newPreset.name,        name.c_str(),        sizeof(newPreset.name) - 1);
    strncpy(newPreset.description, description.c_str(), sizeof(newPreset.description) - 1);
    memcpy(&newPreset.parameters, dsp_get_sysreg(), sizeof(SysReg));
    newPreset.isFactory = false;
    s_userPresets.push_back(newPreset);
    Serial.printf("[DSP] Preset '%s' salvato\n", name.c_str());
    return true;
}

bool dsp_delete_preset(const std::string& name) {
    for (auto it = s_userPresets.begin(); it != s_userPresets.end(); ++it) {
        if (name == it->name) {
            s_userPresets.erase(it);
            Serial.printf("[DSP] Preset '%s' eliminato\n", name.c_str());
            return true;
        }
    }
    Serial.printf("[DSP] Preset '%s' non trovato o non eliminabile\n", name.c_str());
    return false;
}

bool dsp_set_expert_mode(bool expert, const char* password) {
    // Password opzionale per modalità esperto
    if (expert && password) {
        SysReg* reg = dsp_get_sysreg();
        // Verifica password (confronto con quella nel SYSREG)
        if (strncmp(password, (char*)reg->password, 6) != 0 &&
            strlen(password) > 0) {
            Serial.println("[DSP] Password modalità esperto errata");
            return false;
        }
    }
    s_expertMode = expert;
    Serial.printf("[DSP] Modalità %s attivata\n", expert ? "ESPERTO" : "BASE");
    return true;
}

bool dsp_is_expert_mode() {
    return s_expertMode;
}
