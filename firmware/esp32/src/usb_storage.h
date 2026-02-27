#pragma once
#include <Arduino.h>
#include <string>
#include <cstdint>

// ======= Gestione USB-C Import/Export configurazione =======
// Supporta chiavette USB tramite ESP32-S3 USB OTG (MSC host)
// Formati file:
//   .dsp-config   – Configurazione completa sistema
//   .dsp-preset   – Singolo preset DSP
//   .dmx-scene    – Singola scena DMX
//   .dmx-fixtures – Configurazione fixture DMX
//   .audio-reactive – Configurazione audio-reactive

// Stato connessione USB
enum class UsbStorageState {
    Disconnected,   // Nessun dispositivo USB
    Mounting,       // Montaggio in corso
    Ready,          // Pronto per I/O
    Busy,           // I/O in corso
    Error           // Errore
};

// ——— API pubblica ———

// Inizializza il modulo USB storage (USB OTG host)
void usb_storage_init();

// Restituisce lo stato corrente
UsbStorageState usb_storage_get_state();
bool usb_storage_is_ready();

// Esporta configurazione completa in JSON su USB
// filename: nome file senza percorso (default: "dsp-config.json")
bool usb_storage_export_config(const char* filename = "sistema-audio.dsp-config");

// Importa configurazione completa da USB
bool usb_storage_import_config(const char* filename = "sistema-audio.dsp-config");

// Esporta preset DSP singolo
bool usb_storage_export_preset(const char* presetName, const char* filename = nullptr);

// Importa preset DSP da USB
bool usb_storage_import_preset(const char* filename);

// Esporta scena DMX
bool usb_storage_export_scene(uint8_t sceneId, const char* filename = nullptr);

// Importa scena DMX
bool usb_storage_import_scene(const char* filename);

// Esporta tutte le fixture DMX
bool usb_storage_export_fixtures(const char* filename = "fixture.dmx-fixtures");

// Importa fixture DMX
bool usb_storage_import_fixtures(const char* filename = "fixture.dmx-fixtures");

// Esporta scenari audio-reactive
bool usb_storage_export_audio_reactive(const char* filename = "audio-reactive.audio-reactive");

// Importa scenari audio-reactive
bool usb_storage_import_audio_reactive(const char* filename = "audio-reactive.audio-reactive");

// Lista file sul dispositivo USB (per UI)
std::vector<std::string> usb_storage_list_files(const char* extension = nullptr);

// Tick periodico – gestione eventi USB (chiamare dal loop)
void usb_storage_tick();
