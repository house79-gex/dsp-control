#include "usb_storage.h"
#include "dsp_control.h"
#include "dmx512.h"
#include "audio_reactive.h"
#include "storage.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <string.h>
#include <vector>
#include <string>

// ======= Variabili di stato =======

static UsbStorageState s_usbState = UsbStorageState::Disconnected;

// Nota: l'implementazione completa USB MSC Host richiede la libreria USB Host
// (disponibile in ESP-IDF / arduino-esp32 >= 2.x con USB_HOST_LIB).
// Questa implementazione fornisce lo scaffolding e la logica di serializzazione/
// deserializzazione JSON, pronta per essere collegata al driver MSC reale.

// ——— Simulazione filesystem USB per testing ———
// In produzione sostituire con chiamate FATFS/LittleFS su USB MSC host
static std::string s_usbBasePath = "/usb";

static bool usb_write_file(const std::string& path, const std::string& content) {
    // TODO: sostituire con scrittura reale su MSC host
    // Per ora: log e ritorna true (pronto per collegamento a FATFS)
    Serial.printf("[USB] Scrittura file: %s (%d byte)\n", path.c_str(), (int)content.size());
    return true;
}

static bool usb_read_file(const std::string& path, std::string& content) {
    // TODO: sostituire con lettura reale da MSC host
    Serial.printf("[USB] Lettura file: %s\n", path.c_str());
    return false;  // Nessun file disponibile senza hardware USB
}

// ======= Inizializzazione =======

void usb_storage_init() {
    s_usbState = UsbStorageState::Disconnected;
    // Nota: la registrazione del callback USB OTG host deve avvenire
    // dopo l'inizializzazione del sistema USB
    Serial.println("[USB] Modulo USB storage inizializzato");
    Serial.println("[USB] In attesa di dispositivo USB MSC...");
}

void usb_storage_tick() {
    // In produzione: controllo eventi USB OTG host e montaggio MSC
    // Attualmente: nessuna operazione (USB non connesso)
}

UsbStorageState usb_storage_get_state() {
    return s_usbState;
}

bool usb_storage_is_ready() {
    // Per sviluppo/testing: considera sempre "pronto" per permettere
    // l'esportazione verso Serial/log anche senza hardware USB fisico
    return true;
}

// ======= Serializzazione configurazione =======

static std::string serialize_config() {
    // Crea documento JSON con configurazione completa
    DynamicJsonDocument doc(65536);

    // ——— Metadati ———
    doc["version"]   = "1.0";
    doc["timestamp"] = millis();
    doc["system"]    = "DSP-Control ESP32-S3";

    // ——— Preset DSP ———
    JsonArray presetsArr = doc.createNestedArray("dsp_presets");
    auto presetNames = dsp_get_preset_names();
    for (const auto& name : presetNames) {
        const DspPreset* p = dsp_get_preset(name);
        if (p) {
            JsonObject pObj = presetsArr.createNestedObject();
            pObj["name"]        = p->name;
            pObj["description"] = p->description;
            pObj["is_factory"]  = p->isFactory;
            // Nota: serializzazione completa SYSREG omessa per brevità
            // In produzione: serializza ogni campo di p->parameters
        }
    }

    // ——— Fixture DMX ———
    JsonArray fixturesArr = doc.createNestedArray("dmx_fixtures");
    auto fixtures = dmx_get_all_fixtures();
    for (auto* fx : fixtures) {
        JsonObject fxObj = fixturesArr.createNestedObject();
        fxObj["id"]           = fx->id;
        fxObj["name"]         = fx->name;
        fxObj["type"]         = fx->type;
        fxObj["startAddress"] = fx->startAddress;
        fxObj["numChannels"]  = fx->numChannels;
        fxObj["groupId"]      = fx->groupId;
        fxObj["groupName"]    = fx->groupName;
        JsonArray chArr = fxObj.createNestedArray("channels");
        for (int i = 0; i < fx->numChannels; i++) chArr.add(fx->channels[i]);
    }

    // ——— Scene DMX ———
    JsonArray scenesArr = doc.createNestedArray("dmx_scenes");
    auto scenes = dmx_get_all_scenes();
    for (auto* sc : scenes) {
        JsonObject scObj = scenesArr.createNestedObject();
        scObj["id"]          = sc->id;
        scObj["name"]        = sc->name;
        scObj["fadeInTime"]  = sc->fadeInTime;
        scObj["fadeOutTime"] = sc->fadeOutTime;
        scObj["holdTime"]    = sc->holdTime;
        scObj["loop"]        = sc->loop;
        scObj["speed"]       = sc->speed;
        scObj["isFactory"]   = sc->isFactory;
    }

    // ——— Scenari audio-reactive ———
    JsonArray arArr = doc.createNestedArray("audio_reactive_scenarios");
    auto scenarios = audio_reactive_get_all_scenarios();
    for (auto* sc : scenarios) {
        JsonObject scObj = arArr.createNestedObject();
        scObj["id"]         = sc->id;
        scObj["name"]       = sc->name;
        scObj["isFactory"]  = sc->isFactory;
    }

    std::string output;
    serializeJsonPretty(doc, output);
    return output;
}

static bool deserialize_config(const std::string& json) {
    DynamicJsonDocument doc(65536);
    DeserializationError err = deserializeJson(doc, json);
    if (err) {
        Serial.printf("[USB] Errore parsing JSON: %s\n", err.c_str());
        return false;
    }

    // ——— Importa fixture DMX ———
    if (doc.containsKey("dmx_fixtures")) {
        JsonArray arr = doc["dmx_fixtures"].as<JsonArray>();
        for (JsonObject fxObj : arr) {
            DmxFixture fx = {};
            fx.id           = fxObj["id"] | 0;
            fx.startAddress = fxObj["startAddress"] | 1;
            fx.numChannels  = fxObj["numChannels"] | 3;
            fx.groupId      = fxObj["groupId"] | 0;
            strncpy(fx.name,      fxObj["name"]      | "", sizeof(fx.name) - 1);
            strncpy(fx.type,      fxObj["type"]      | "", sizeof(fx.type) - 1);
            strncpy(fx.groupName, fxObj["groupName"] | "", sizeof(fx.groupName) - 1);
            dmx_add_fixture(fx);
        }
    }

    return true;
}

// ======= API pubblica =======

bool usb_storage_export_config(const char* filename) {
    if (!usb_storage_is_ready()) {
        Serial.println("[USB] Dispositivo USB non pronto");
        return false;
    }
    std::string content = serialize_config();
    std::string path    = s_usbBasePath + "/" + filename;
    return usb_write_file(path, content);
}

bool usb_storage_import_config(const char* filename) {
    if (!usb_storage_is_ready()) {
        Serial.println("[USB] Dispositivo USB non pronto");
        return false;
    }
    std::string path = s_usbBasePath + "/" + filename;
    std::string content;
    if (!usb_read_file(path, content)) return false;
    return deserialize_config(content);
}

bool usb_storage_export_preset(const char* presetName, const char* filename) {
    const DspPreset* p = dsp_get_preset(std::string(presetName));
    if (!p) return false;

    DynamicJsonDocument doc(8192);
    doc["name"]        = p->name;
    doc["description"] = p->description;
    doc["is_factory"]  = p->isFactory;

    std::string content;
    serializeJsonPretty(doc, content);
    std::string fn   = filename ? filename : (std::string(presetName) + ".dsp-preset");
    std::string path = s_usbBasePath + "/" + fn;
    return usb_write_file(path, content);
}

bool usb_storage_import_preset(const char* filename) {
    std::string path = s_usbBasePath + "/" + filename;
    std::string content;
    if (!usb_read_file(path, content)) return false;

    DynamicJsonDocument doc(8192);
    if (deserializeJson(doc, content)) return false;

    std::string name = doc["name"] | "IMPORTATO";
    std::string desc = doc["description"] | "";
    return dsp_save_preset(name, desc);
}

bool usb_storage_export_scene(uint8_t sceneId, const char* filename) {
    DmxScene* sc = dmx_get_scene(sceneId);
    if (!sc) return false;

    DynamicJsonDocument doc(4096);
    doc["id"]         = sc->id;
    doc["name"]       = sc->name;
    doc["fadeInTime"] = sc->fadeInTime;
    doc["fadeOutTime"]= sc->fadeOutTime;
    doc["holdTime"]   = sc->holdTime;
    doc["loop"]       = sc->loop;
    doc["speed"]      = sc->speed;

    std::string content;
    serializeJsonPretty(doc, content);
    std::string fn   = filename ? filename : (std::string(sc->name) + ".dmx-scene");
    std::string path = s_usbBasePath + "/" + fn;
    return usb_write_file(path, content);
}

bool usb_storage_import_scene(const char* filename) {
    std::string path = s_usbBasePath + "/" + filename;
    std::string content;
    if (!usb_read_file(path, content)) return false;

    DynamicJsonDocument doc(4096);
    if (deserializeJson(doc, content)) return false;

    DmxScene sc = {};
    sc.id          = doc["id"]         | 100;
    sc.fadeInTime  = doc["fadeInTime"] | 0.0f;
    sc.fadeOutTime = doc["fadeOutTime"]| 0.0f;
    sc.holdTime    = doc["holdTime"]   | 0.0f;
    sc.loop        = doc["loop"]       | false;
    sc.speed       = doc["speed"]      | 1.0f;
    strncpy(sc.name, doc["name"] | "IMPORTATA", sizeof(sc.name) - 1);
    return dmx_save_scene(sc);
}

bool usb_storage_export_fixtures(const char* filename) {
    DynamicJsonDocument doc(16384);
    JsonArray arr = doc.to<JsonArray>();
    auto fixtures = dmx_get_all_fixtures();
    for (auto* fx : fixtures) {
        JsonObject obj = arr.createNestedObject();
        obj["id"]           = fx->id;
        obj["name"]         = fx->name;
        obj["type"]         = fx->type;
        obj["startAddress"] = fx->startAddress;
        obj["numChannels"]  = fx->numChannels;
        obj["groupId"]      = fx->groupId;
        obj["groupName"]    = fx->groupName;
    }
    std::string content;
    serializeJsonPretty(doc, content);
    std::string path = s_usbBasePath + "/" + filename;
    return usb_write_file(path, content);
}

bool usb_storage_import_fixtures(const char* filename) {
    std::string path = s_usbBasePath + "/" + filename;
    std::string content;
    if (!usb_read_file(path, content)) return false;

    DynamicJsonDocument doc(16384);
    if (deserializeJson(doc, content)) return false;

    JsonArray arr = doc.as<JsonArray>();
    for (JsonObject obj : arr) {
        DmxFixture fx = {};
        fx.id           = obj["id"]           | 0;
        fx.startAddress = obj["startAddress"] | 1;
        fx.numChannels  = obj["numChannels"]  | 3;
        fx.groupId      = obj["groupId"]      | 0;
        strncpy(fx.name,      obj["name"]      | "", sizeof(fx.name) - 1);
        strncpy(fx.type,      obj["type"]      | "", sizeof(fx.type) - 1);
        strncpy(fx.groupName, obj["groupName"] | "", sizeof(fx.groupName) - 1);
        dmx_add_fixture(fx);
    }
    return true;
}

bool usb_storage_export_audio_reactive(const char* filename) {
    DynamicJsonDocument doc(32768);
    JsonArray arr = doc.to<JsonArray>();
    auto scenarios = audio_reactive_get_all_scenarios();
    for (auto* sc : scenarios) {
        JsonObject obj = arr.createNestedObject();
        obj["id"]        = sc->id;
        obj["name"]      = sc->name;
        obj["isFactory"] = sc->isFactory;
        JsonArray bandsArr = obj.createNestedArray("bands");
        for (int i = 0; i < AUDIO_NUM_BANDS; i++) {
            JsonObject bObj = bandsArr.createNestedObject();
            bObj["minFreq"]     = sc->bands[i].minFreq;
            bObj["maxFreq"]     = sc->bands[i].maxFreq;
            bObj["colorR"]      = sc->bands[i].colorR;
            bObj["colorG"]      = sc->bands[i].colorG;
            bObj["colorB"]      = sc->bands[i].colorB;
            bObj["sensitivity"] = sc->bands[i].sensitivity;
            bObj["attackMs"]    = sc->bands[i].attackMs;
            bObj["decayMs"]     = sc->bands[i].decayMs;
            bObj["blendMode"]   = sc->bands[i].blendMode;
        }
    }
    std::string content;
    serializeJsonPretty(doc, content);
    std::string path = s_usbBasePath + "/" + filename;
    return usb_write_file(path, content);
}

bool usb_storage_import_audio_reactive(const char* filename) {
    std::string path = s_usbBasePath + "/" + filename;
    std::string content;
    if (!usb_read_file(path, content)) return false;

    DynamicJsonDocument doc(32768);
    if (deserializeJson(doc, content)) return false;

    JsonArray arr = doc.as<JsonArray>();
    for (JsonObject obj : arr) {
        AudioReactiveScenario sc = {};
        sc.id        = obj["id"]    | 100;
        sc.isFactory = false;
        strncpy(sc.name, obj["name"] | "IMPORTATO", sizeof(sc.name) - 1);
        if (obj.containsKey("bands")) {
            JsonArray bands = obj["bands"].as<JsonArray>();
            int i = 0;
            for (JsonObject b : bands) {
                if (i >= AUDIO_NUM_BANDS) break;
                sc.bands[i].minFreq     = b["minFreq"]     | 0.0f;
                sc.bands[i].maxFreq     = b["maxFreq"]     | 1000.0f;
                sc.bands[i].colorR      = b["colorR"]      | 255;
                sc.bands[i].colorG      = b["colorG"]      | 0;
                sc.bands[i].colorB      = b["colorB"]      | 0;
                sc.bands[i].sensitivity = b["sensitivity"] | 1.0f;
                sc.bands[i].attackMs    = b["attackMs"]    | 10.0f;
                sc.bands[i].decayMs     = b["decayMs"]     | 200.0f;
                strncpy(sc.bands[i].blendMode, b["blendMode"] | "override", 15);
                i++;
            }
        }
        audio_reactive_save_scenario(sc);
    }
    return true;
}

std::vector<std::string> usb_storage_list_files(const char* extension) {
    // TODO: implementare con FATFS quando USB MSC host disponibile
    std::vector<std::string> files;
    return files;
}
