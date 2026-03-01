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
            if (i < sizeof(g.fixtureIds) / sizeof(g.fixtureIds[0])) g.fixtureIds[i++] = fid;
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

// ======= Persistenza Calibrazione Microfono di Misura =======

void storage_save_mic_calibration(const MicCalibration& cal) {
    DynamicJsonDocument doc(4096);
    doc["valid"]     = cal.valid;
    doc["micName"]   = cal.micName;
    doc["numPoints"] = cal.numPoints;
    JsonArray freqs = doc.createNestedArray("freqHz");
    JsonArray corrs = doc.createNestedArray("correctionDb");
    for (uint8_t i = 0; i < cal.numPoints; i++) {
        freqs.add(cal.freqHz[i]);
        corrs.add(cal.correctionDb[i]);
    }
    String buf;
    serializeJson(doc, buf);
    s_prefs.putString("mic_cal", buf.c_str());
    Serial.printf("[STORAGE] Calibrazione microfono salvata: %s (%d punti)\n",
                  cal.micName, cal.numPoints);
}

bool storage_load_mic_calibration(MicCalibration& cal) {
    memset(&cal, 0, sizeof(MicCalibration));
    String json = s_prefs.getString("mic_cal", "{}");
    if (json.isEmpty() || json == "{}") {
        Serial.println("[STORAGE] Nessuna calibrazione microfono salvata");
        return false;
    }

    DynamicJsonDocument doc(4096);
    if (deserializeJson(doc, json)) {
        Serial.println("[STORAGE] Errore parsing calibrazione microfono");
        return false;
    }

    cal.valid     = doc["valid"] | false;
    cal.numPoints = doc["numPoints"] | 0;
    strncpy(cal.micName, doc["micName"] | "", sizeof(cal.micName) - 1);

    JsonArray freqs = doc["freqHz"].as<JsonArray>();
    JsonArray corrs = doc["correctionDb"].as<JsonArray>();
    uint8_t i = 0;
    for (float v : freqs) { if (i < 128) cal.freqHz[i++] = v; }
    i = 0;
    for (float v : corrs) { if (i < 128) cal.correctionDb[i++] = v; }

    Serial.printf("[STORAGE] Calibrazione microfono caricata: %s (%d punti)\n",
                  cal.micName, cal.numPoints);
    return cal.valid;
}

void storage_clear_mic_calibration() {
    s_prefs.remove("mic_cal");
    Serial.println("[STORAGE] Calibrazione microfono cancellata");
}

// ======= Persistenza Controller e Scene WLED =======

#include "wled_client.h"

static const char*   NVS_KEY_WLED_CTRL  = "wled_ctrl";
static const char*   NVS_KEY_WLED_SCENE = "wled_scene";
static const size_t  WLED_JSON_DOC_SIZE = 4096;

void storage_save_wled_controllers() {
    uint8_t n = wled_get_controller_count();
    DynamicJsonDocument doc(WLED_JSON_DOC_SIZE);
    JsonArray arr = doc.to<JsonArray>();
    for (uint8_t i = 0; i < n; i++) {
        WledController* c = wled_get_controller(i);
        JsonObject obj = arr.createNestedObject();
        obj["name"]         = c->name;
        obj["ip"]           = c->ip;
        obj["num_channels"] = c->num_channels;
        obj["sync_enabled"] = c->sync_enabled;
        JsonArray ppc  = obj.createNestedArray("pixels_per_ch");
        JsonArray zmap = obj.createNestedArray("zone_map");
        for (int ch = 0; ch < 4; ch++) {
            ppc.add(c->pixels_per_ch[ch]);
            zmap.add((int)c->zone_map[ch]);
        }
    }
    String buf;
    serializeJson(doc, buf);
    s_prefs.putString(NVS_KEY_WLED_CTRL, buf.c_str());
    Serial.printf("[STORAGE] Salvati %d controller WLED\n", n);
}

void storage_load_wled_controllers() {
    String json = s_prefs.getString(NVS_KEY_WLED_CTRL, "[]");
    if (json.isEmpty() || json == "[]") return;

    DynamicJsonDocument doc(WLED_JSON_DOC_SIZE);
    if (deserializeJson(doc, json)) return;

    for (JsonObject obj : doc.as<JsonArray>()) {
        WledController c = {};
        strncpy(c.name, obj["name"] | "", sizeof(c.name) - 1);
        strncpy(c.ip,   obj["ip"]   | "", sizeof(c.ip)   - 1);
        c.num_channels = obj["num_channels"] | 2;
        c.sync_enabled = obj["sync_enabled"] | false;
        JsonArray ppc  = obj["pixels_per_ch"].as<JsonArray>();
        JsonArray zmap = obj["zone_map"].as<JsonArray>();
        for (int ch = 0; ch < 4; ch++) {
            c.pixels_per_ch[ch] = ppc[ch] | 80;
            c.zone_map[ch]      = (WledZone)(int)(zmap[ch] | ch);
        }
        wled_client_add_controller(c);
    }
    Serial.printf("[STORAGE] Caricati controller WLED da NVS\n");
}

void storage_save_wled_scenes() {
    uint8_t n = wled_get_scene_count();
    DynamicJsonDocument doc(WLED_JSON_DOC_SIZE);
    JsonArray arr = doc.to<JsonArray>();
    for (uint8_t i = 0; i < n; i++) {
        WledScene* s = wled_get_scene(i);
        JsonObject obj = arr.createNestedObject();
        obj["name"]      = s->name;
        obj["effect_id"] = s->effect_id;
        obj["r"]         = s->r;
        obj["g"]         = s->g;
        obj["b"]         = s->b;
        obj["r2"]        = s->r2;
        obj["g2"]        = s->g2;
        obj["b2"]        = s->b2;
        obj["brightness"]= s->brightness;
        obj["speed"]     = s->speed;
        obj["intensity"] = s->intensity;
        obj["mirror"]    = s->mirror;
        obj["reverse"]   = s->reverse;
    }
    String buf;
    serializeJson(doc, buf);
    s_prefs.putString(NVS_KEY_WLED_SCENE, buf.c_str());
    Serial.printf("[STORAGE] Salvate %d scene WLED\n", n);
}

void storage_load_wled_scenes() {
    String json = s_prefs.getString(NVS_KEY_WLED_SCENE, "[]");
    if (json.isEmpty() || json == "[]") return;

    DynamicJsonDocument doc(WLED_JSON_DOC_SIZE);
    if (deserializeJson(doc, json)) return;

    for (JsonObject obj : doc.as<JsonArray>()) {
        WledScene s = {};
        strncpy(s.name, obj["name"] | "", sizeof(s.name) - 1);
        s.effect_id = obj["effect_id"] | 0;
        s.r         = obj["r"]         | 255;
        s.g         = obj["g"]         | 255;
        s.b         = obj["b"]         | 255;
        s.r2        = obj["r2"]        | 0;
        s.g2        = obj["g2"]        | 0;
        s.b2        = obj["b2"]        | 0;
        s.brightness= obj["brightness"]| 255;
        s.speed     = obj["speed"]     | 128;
        s.intensity = obj["intensity"] | 128;
        s.mirror    = obj["mirror"]    | false;
        s.reverse   = obj["reverse"]   | false;
        wled_add_scene(s);
    }
    Serial.printf("[STORAGE] Caricate scene WLED da NVS\n");
}
