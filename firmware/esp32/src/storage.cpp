#include "storage.h"
#include <Preferences.h>
#include <ArduinoJson.h>

static Preferences s_prefs;
static const char* NVS_NAMESPACE = "dsp_ctrl";
static const char* NVS_KEY_ASSIGN = "assignments";

// Converte SpeakerRole → stringa
static const char* roleToStr(SpeakerRole role) {
    switch (role) {
        case SpeakerRole::SX:   return "SX";
        case SpeakerRole::DX:   return "DX";
        case SpeakerRole::SUB:  return "SUB";
        case SpeakerRole::MONO: return "MONO";
        default:                return "MONO";
    }
}

// Converte stringa → SpeakerRole
static SpeakerRole strToRole(const char* s) {
    if (strcmp(s, "SX")  == 0) return SpeakerRole::SX;
    if (strcmp(s, "DX")  == 0) return SpeakerRole::DX;
    if (strcmp(s, "SUB") == 0) return SpeakerRole::SUB;
    return SpeakerRole::MONO;
}

void storage_init() {
    s_prefs.begin(NVS_NAMESPACE, false);
    Serial.println("[STORAGE] NVS inizializzato");
}

void storage_save_assignments(const std::vector<SpeakerAssignment>& assignments) {
    // Serializza la lista in JSON e salva come stringa NVS
    StaticJsonDocument<4096> doc;
    JsonArray arr = doc.to<JsonArray>();

    for (const auto& a : assignments) {
        JsonObject obj = arr.createNestedObject();
        obj["deviceId"] = a.deviceId;
        obj["position"] = a.position.c_str();
        obj["role"]     = roleToStr(a.role);
        obj["type"]     = a.type.c_str();
        obj["name"]     = a.name.c_str();
    }

    // Usa String su heap per evitare stack overflow su ESP32
    String buf;
    size_t written = serializeJson(doc, buf);
    if (written == 0) {
        Serial.println("[STORAGE] Errore serializzazione JSON");
        return;
    }

    s_prefs.putString(NVS_KEY_ASSIGN, buf.c_str());
    Serial.printf("[STORAGE] Salvate %d assegnazioni\n", (int)assignments.size());
}

std::vector<SpeakerAssignment> storage_load_assignments() {
    std::vector<SpeakerAssignment> assignments;

    String json = s_prefs.getString(NVS_KEY_ASSIGN, "[]");
    if (json.isEmpty() || json == "[]") {
        Serial.println("[STORAGE] Nessuna assegnazione salvata");
        return assignments;
    }

    StaticJsonDocument<4096> doc;
    DeserializationError err = deserializeJson(doc, json);
    if (err) {
        Serial.printf("[STORAGE] Errore parsing JSON: %s\n", err.c_str());
        return assignments;
    }

    JsonArray arr = doc.as<JsonArray>();
    for (JsonObject obj : arr) {
        SpeakerAssignment a;
        a.deviceId = obj["deviceId"] | 0;
        a.position = obj["position"] | "";
        a.role     = strToRole(obj["role"] | "MONO");
        a.type     = obj["type"] | "2WAY";
        a.name     = obj["name"] | "";
        assignments.push_back(a);
    }

    Serial.printf("[STORAGE] Caricate %d assegnazioni\n", (int)assignments.size());
    return assignments;
}

void storage_clear() {
    s_prefs.remove(NVS_KEY_ASSIGN);
    Serial.println("[STORAGE] Assegnazioni cancellate");
}

// ======= Persistenza Preset DSP =======

void storage_save_dsp_presets(const std::vector<DspPreset>& presets) {
    DynamicJsonDocument doc(8192);
    JsonArray arr = doc.to<JsonArray>();
    for (const auto& p : presets) {
        JsonObject obj = arr.createNestedObject();
        obj["name"]        = p.name;
        obj["description"] = p.description;
        obj["isFactory"]   = p.isFactory;
    }
    String buf;
    serializeJson(doc, buf);
    s_prefs.putString("dsp_presets", buf.c_str());
    Serial.printf("[STORAGE] Salvati %d preset DSP\n", (int)presets.size());
}

std::vector<DspPreset> storage_load_dsp_presets() {
    std::vector<DspPreset> presets;
    String json = s_prefs.getString("dsp_presets", "[]");
    if (json.isEmpty() || json == "[]") return presets;

    DynamicJsonDocument doc(8192);
    if (deserializeJson(doc, json)) return presets;

    for (JsonObject obj : doc.as<JsonArray>()) {
        DspPreset p = {};
        strncpy(p.name,        obj["name"]        | "", sizeof(p.name) - 1);
        strncpy(p.description, obj["description"] | "", sizeof(p.description) - 1);
        p.isFactory = obj["isFactory"] | false;
        presets.push_back(p);
    }
    Serial.printf("[STORAGE] Caricati %d preset DSP\n", (int)presets.size());
    return presets;
}

// ======= Persistenza Fixture DMX =======

void storage_save_dmx_fixtures(const std::vector<DmxFixture*>& fixtures) {
    DynamicJsonDocument doc(8192);
    JsonArray arr = doc.to<JsonArray>();
    for (const auto* f : fixtures) {
        JsonObject obj = arr.createNestedObject();
        obj["id"]           = f->id;
        obj["name"]         = f->name;
        obj["type"]         = f->type;
        obj["startAddress"] = f->startAddress;
        obj["numChannels"]  = f->numChannels;
        obj["groupId"]      = f->groupId;
        obj["groupName"]    = f->groupName;
        obj["online"]       = f->online;
    }
    String buf;
    serializeJson(doc, buf);
    s_prefs.putString("dmx_fixtures", buf.c_str());
    Serial.printf("[STORAGE] Salvate %d fixture DMX\n", (int)fixtures.size());
}

std::vector<DmxFixture> storage_load_dmx_fixtures() {
    std::vector<DmxFixture> fixtures;
    String json = s_prefs.getString("dmx_fixtures", "[]");
    if (json.isEmpty() || json == "[]") return fixtures;

    DynamicJsonDocument doc(8192);
    if (deserializeJson(doc, json)) return fixtures;

    for (JsonObject obj : doc.as<JsonArray>()) {
        DmxFixture f = {};
        f.id           = obj["id"]           | 0;
        f.startAddress = obj["startAddress"] | 1;
        f.numChannels  = obj["numChannels"]  | 1;
        f.groupId      = obj["groupId"]      | 0;
        f.online       = obj["online"]       | false;
        strncpy(f.name,      obj["name"]      | "", sizeof(f.name) - 1);
        strncpy(f.type,      obj["type"]      | "", sizeof(f.type) - 1);
        strncpy(f.groupName, obj["groupName"] | "", sizeof(f.groupName) - 1);
        fixtures.push_back(f);
    }
    Serial.printf("[STORAGE] Caricate %d fixture DMX\n", (int)fixtures.size());
    return fixtures;
}

// ======= Persistenza Scene DMX =======

void storage_save_dmx_scenes(const std::vector<DmxScene*>& scenes) {
    DynamicJsonDocument doc(8192);
    JsonArray arr = doc.to<JsonArray>();
    for (const auto* s : scenes) {
        JsonObject obj = arr.createNestedObject();
        obj["id"]          = s->id;
        obj["name"]        = s->name;
        obj["fadeInTime"]  = s->fadeInTime;
        obj["fadeOutTime"] = s->fadeOutTime;
        obj["holdTime"]    = s->holdTime;
        obj["loop"]        = s->loop;
        obj["speed"]       = s->speed;
        obj["isFactory"]   = s->isFactory;
    }
    String buf;
    serializeJson(doc, buf);
    s_prefs.putString("dmx_scenes", buf.c_str());
    Serial.printf("[STORAGE] Salvate %d scene DMX\n", (int)scenes.size());
}

std::vector<DmxScene> storage_load_dmx_scenes() {
    std::vector<DmxScene> scenes;
    String json = s_prefs.getString("dmx_scenes", "[]");
    if (json.isEmpty() || json == "[]") return scenes;

    DynamicJsonDocument doc(8192);
    if (deserializeJson(doc, json)) return scenes;

    for (JsonObject obj : doc.as<JsonArray>()) {
        DmxScene s = {};
        s.id          = obj["id"]          | 0;
        s.fadeInTime  = obj["fadeInTime"]  | 0.0f;
        s.fadeOutTime = obj["fadeOutTime"] | 0.0f;
        s.holdTime    = obj["holdTime"]    | 0.0f;
        s.loop        = obj["loop"]        | false;
        s.speed       = obj["speed"]       | 1.0f;
        s.isFactory   = obj["isFactory"]   | false;
        strncpy(s.name, obj["name"] | "", sizeof(s.name) - 1);
        scenes.push_back(s);
    }
    Serial.printf("[STORAGE] Caricate %d scene DMX\n", (int)scenes.size());
    return scenes;
}

// ======= Persistenza Gruppi DMX =======

void storage_save_dmx_groups(const std::vector<DmxGroup*>& groups) {
    DynamicJsonDocument doc(8192);
    JsonArray arr = doc.to<JsonArray>();
    for (const auto* g : groups) {
        JsonObject obj = arr.createNestedObject();
        obj["id"]          = g->id;
        obj["name"]        = g->name;
        obj["numFixtures"] = g->numFixtures;
        JsonArray fids = obj.createNestedArray("fixtureIds");
        for (int i = 0; i < g->numFixtures; i++) fids.add(g->fixtureIds[i]);
    }
    String buf;
    serializeJson(doc, buf);
    s_prefs.putString("dmx_groups", buf.c_str());
    Serial.printf("[STORAGE] Salvati %d gruppi DMX\n", (int)groups.size());
}

std::vector<DmxGroup> storage_load_dmx_groups() {
    std::vector<DmxGroup> groups;
    String json = s_prefs.getString("dmx_groups", "[]");
    if (json.isEmpty() || json == "[]") return groups;

    DynamicJsonDocument doc(8192);
    if (deserializeJson(doc, json)) return groups;

    for (JsonObject obj : doc.as<JsonArray>()) {
        DmxGroup g = {};
        g.id          = obj["id"]          | 0;
        g.numFixtures = obj["numFixtures"] | 0;
        strncpy(g.name, obj["name"] | "", sizeof(g.name) - 1);
        JsonArray fids = obj["fixtureIds"].as<JsonArray>();
        uint8_t i = 0;
        for (uint8_t fid : fids) {
            if (i < sizeof(g.fixtureIds)) g.fixtureIds[i++] = fid;
        }
        groups.push_back(g);
    }
    Serial.printf("[STORAGE] Caricati %d gruppi DMX\n", (int)groups.size());
    return groups;
}

// ======= Persistenza Scenari Audio-Reactive =======

void storage_save_audio_reactive_scenarios(const std::vector<AudioReactiveScenario*>& scenarios) {
    DynamicJsonDocument doc(8192);
    JsonArray arr = doc.to<JsonArray>();
    for (const auto* sc : scenarios) {
        JsonObject obj = arr.createNestedObject();
        obj["id"]        = sc->id;
        obj["name"]      = sc->name;
        obj["isFactory"] = sc->isFactory;
    }
    String buf;
    serializeJson(doc, buf);
    s_prefs.putString("ar_scenarios", buf.c_str());
    Serial.printf("[STORAGE] Salvati %d scenari audio-reactive\n", (int)scenarios.size());
}

std::vector<AudioReactiveScenario> storage_load_audio_reactive_scenarios() {
    std::vector<AudioReactiveScenario> scenarios;
    String json = s_prefs.getString("ar_scenarios", "[]");
    if (json.isEmpty() || json == "[]") return scenarios;

    DynamicJsonDocument doc(8192);
    if (deserializeJson(doc, json)) return scenarios;

    for (JsonObject obj : doc.as<JsonArray>()) {
        AudioReactiveScenario sc = {};
        sc.id        = obj["id"]        | 0;
        sc.isFactory = obj["isFactory"] | false;
        strncpy(sc.name, obj["name"] | "", sizeof(sc.name) - 1);
        scenarios.push_back(sc);
    }
    Serial.printf("[STORAGE] Caricati %d scenari audio-reactive\n", (int)scenarios.size());
    return scenarios;
}

// ======= Persistenza Configurazione Sistema =======

void storage_save_system_config(const SystemConfig& config) {
    DynamicJsonDocument doc(512);
    doc["expertMode"]      = config.expertMode;
    doc["expertPassword"]  = config.expertPassword;
    doc["masterVolume"]    = config.masterVolume;
    doc["masterBalance"]   = config.masterBalance;
    String buf;
    serializeJson(doc, buf);
    s_prefs.putString("sys_config", buf.c_str());
    Serial.println("[STORAGE] Configurazione sistema salvata");
}

SystemConfig storage_load_system_config() {
    SystemConfig config = {};
    config.masterVolume = 80;  // default

    String json = s_prefs.getString("sys_config", "{}");
    if (json.isEmpty() || json == "{}") return config;

    DynamicJsonDocument doc(512);
    if (deserializeJson(doc, json)) return config;

    config.expertMode    = doc["expertMode"]    | false;
    config.masterVolume  = doc["masterVolume"]  | 80;
    config.masterBalance = doc["masterBalance"] | 0;
    strncpy(config.expertPassword, doc["expertPassword"] | "", sizeof(config.expertPassword) - 1);
    Serial.println("[STORAGE] Configurazione sistema caricata");
    return config;
}
