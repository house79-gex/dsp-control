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

    char buf[4096];
    size_t written = serializeJson(doc, buf, sizeof(buf));
    if (written == 0) {
        Serial.println("[STORAGE] Errore serializzazione JSON");
        return;
    }

    s_prefs.putString(NVS_KEY_ASSIGN, buf);
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
