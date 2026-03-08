#include "storage_slave.h"
#include "config.h"
#include <Preferences.h>

static Preferences s_prefs;

void storage_slave_init() {
    s_prefs.begin(NVS_NAMESPACE, false);
    Serial.printf("[STORAGE_SLAVE] NVS inizializzato (namespace: %s)\n", NVS_NAMESPACE);
}

bool storage_slave_save(const char* key, const uint8_t* data, size_t len) {
    if (key == nullptr || data == nullptr) {
        Serial.println("[STORAGE_SLAVE] Parametri nulli");
        return false;
    }
    if (len == 0 || len > 4096) {
        Serial.printf("[STORAGE_SLAVE] Dimensione payload invalida: %zu\n", len);
        return false;
    }

    size_t written = s_prefs.putBytes(key, data, len);
    bool ok = (written == len);
    if (!ok) {
        Serial.printf("[STORAGE_SLAVE] Errore scrittura key='%s' (%zu/%zu bytes)\n", key, written, len);
    } else {
        Serial.printf("[STORAGE_SLAVE] Salvato key='%s' (%zu bytes)\n", key, len);
    }
    return ok;
}

bool storage_slave_load(const char* key, uint8_t* data, size_t maxLen, size_t& outLen) {
    if (key == nullptr || data == nullptr) {
        outLen = 0;
        return false;
    }

    outLen = s_prefs.getBytesLength(key);
    if (outLen == 0) {
        Serial.printf("[STORAGE_SLAVE] Key non trovata: '%s'\n", key);
        return false;
    }
    if (outLen > maxLen) {
        Serial.printf("[STORAGE_SLAVE] Buffer troppo piccolo: %zu > %zu\n", outLen, maxLen);
        outLen = maxLen;
    }

    size_t read = s_prefs.getBytes(key, data, outLen);
    bool ok = (read > 0);
    if (ok) {
        Serial.printf("[STORAGE_SLAVE] Caricato key='%s' (%zu bytes)\n", key, read);
        outLen = read;
    }
    return ok;
}

void storage_slave_clear(const char* key) {
    if (key == nullptr) return;
    s_prefs.remove(key);
    Serial.printf("[STORAGE_SLAVE] Key rimossa: '%s'\n", key);
}

void storage_slave_clear_all() {
    s_prefs.clear();
    Serial.println("[STORAGE_SLAVE] Tutti i dati cancellati");
}
