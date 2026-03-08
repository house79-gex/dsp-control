#include "wireless_slave.h"
#include "config.h"
#include <WiFi.h>
#include <esp_now.h>

// Indirizzo broadcast ESP-NOW (FF:FF:FF:FF:FF:FF = tutti i peer)
static uint8_t kBroadcastAddr[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

static uint32_t s_txCount = 0;
static uint8_t  s_seqNum  = 0;
static bool     s_initialized = false;

static void onDataSent(const uint8_t* mac, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        s_txCount++;
    }
}

void wireless_slave_init() {
    // WiFi in modalità Station (richiesto da ESP-NOW)
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // Imposta potenza TX massima (antenna IPEX 3dBi)
    esp_err_t err = esp_wifi_set_max_tx_power(WIFI_TX_POWER_DBM * 4);  // unità: 0.25dBm
    if (err != ESP_OK) {
        Serial.printf("[WIRELESS] Warning: impossibile impostare TX power: %d\n", err);
    }

    // Inizializza ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("[WIRELESS] Errore init ESP-NOW");
        return;
    }
    esp_now_register_send_cb(onDataSent);

    // Registra peer broadcast
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, kBroadcastAddr, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("[WIRELESS] Errore aggiunta peer broadcast");
        return;
    }

    s_initialized = true;
    Serial.printf("[WIRELESS] ESP-NOW inizializzato, TX power %d dBm, MAC: %s\n",
                  WIFI_TX_POWER_DBM, WiFi.macAddress().c_str());
}

void wireless_slave_send_frame(const WirelessAudioFrame& frame) {
    if (!s_initialized) return;

    WirelessAudioFrame f = frame;
    f.seqNum = s_seqNum++;

    esp_now_send(kBroadcastAddr, reinterpret_cast<const uint8_t*>(&f), sizeof(f));
}

uint32_t wireless_slave_get_tx_count() {
    return s_txCount;
}

int8_t wireless_slave_get_rssi() {
    // RSSI non disponibile per trasmissioni broadcast ESP-NOW
    // -128 è il valore sentinella per "non disponibile"
    return -128;
}
