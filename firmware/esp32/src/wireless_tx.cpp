#include "wireless_tx.h"
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <string.h>

// ======= Costanti =======

#define WIRELESS_MAGIC_AUDIO   0xAA55
#define WIRELESS_MAGIC_RS485   0xBB66
#define WIRELESS_MAGIC_DMX     0xCC77
#define PEER_TIMEOUT_MS        3000
#define AUDIO_FRAME_SAMPLES    48
#define AUDIO_RING_BUF_FRAMES  8

// ======= Variabili statiche =======

static bool s_enabled = false;
static WirelessAudioMode s_audioMode = WirelessAudioMode::STEREO_SPLIT;

// Default MAC peer (da cambiare con valori reali)
static uint8_t s_peer1Mac[6] = { 0x24, 0x0A, 0xC4, 0x12, 0x34, 0x56 };
static uint8_t s_peer2Mac[6] = { 0x24, 0x0A, 0xC4, 0x78, 0x9A, 0xBC };

static WirelessChannelStats s_stats[2] = {};

// Contatori di sequenza per canale
static uint16_t s_seqCh1 = 0;
static uint16_t s_seqCh2 = 0;

// Buffer audio circolare (stereo int16)
static SemaphoreHandle_t s_audioMutex = nullptr;
static int16_t  s_audioBuf[AUDIO_RING_BUF_FRAMES * AUDIO_FRAME_SAMPLES * 2] = {};
static uint32_t s_audioBufWriteIdx = 0;
static uint32_t s_audioBufReadIdx  = 0;
static uint32_t s_audioBufCount    = 0;  // numero di frame disponibili

// ======= Utilità CRC8 =======

static uint8_t crc8_compute(const uint8_t* data, size_t len) {
    uint8_t crc = 0x00;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int b = 0; b < 8; b++) {
            if (crc & 0x80) crc = (crc << 1) ^ 0x07;
            else            crc <<= 1;
        }
    }
    return crc;
}

// ======= Callback ESP-NOW =======

static void on_data_sent(const uint8_t* mac_addr, esp_now_send_status_t status) {
    // Identifica quale canale ha risposto
    int ch = -1;
    if (memcmp(mac_addr, s_peer1Mac, 6) == 0) ch = 0;
    else if (memcmp(mac_addr, s_peer2Mac, 6) == 0) ch = 1;
    if (ch < 0) return;

    if (status == ESP_NOW_SEND_SUCCESS) {
        s_stats[ch].packetsAcked++;
        s_stats[ch].online    = true;
        s_stats[ch].lastSeenMs = millis();
    } else {
        s_stats[ch].packetsFailed++;
    }
    s_stats[ch].packetsSent++;

    // Aggiorna loss rate
    if (s_stats[ch].packetsSent > 0) {
        s_stats[ch].lossRate = (float)s_stats[ch].packetsFailed /
                               (float)s_stats[ch].packetsSent;
    }
}

// ======= Registrazione peer =======

static bool register_peer(const uint8_t* mac) {
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, mac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_is_peer_exist(mac)) {
        return esp_now_mod_peer(&peerInfo) == ESP_OK;
    }
    return esp_now_add_peer(&peerInfo) == ESP_OK;
}

// ======= Watchdog peer =======

static void check_peer_timeouts() {
    uint32_t now = millis();
    for (int i = 0; i < 2; i++) {
        if (s_stats[i].online &&
            (now - s_stats[i].lastSeenMs) > PEER_TIMEOUT_MS) {
            s_stats[i].online = false;
            Serial.printf("[WIRELESS] CH%d offline (timeout)\n", i + 1);
        }
    }
}

// ======= Invio frame =======

static bool send_to_peer(uint8_t channelId, const void* data, size_t len) {
    if (!s_enabled) return false;
    const uint8_t* mac = (channelId == 1) ? s_peer1Mac : s_peer2Mac;
    int result = esp_now_send(mac, (const uint8_t*)data, len);
    return result == ESP_OK;
}

// ======= API pubblica =======

bool wireless_tx_init() {
    // WiFi deve essere in modalità STA o AP+STA per ESP-NOW
    if (WiFi.getMode() == WIFI_OFF) {
        WiFi.mode(WIFI_AP_STA);
    }

    if (esp_now_init() != ESP_OK) {
        Serial.println("[WIRELESS] Errore init ESP-NOW");
        return false;
    }

    esp_now_register_send_cb(on_data_sent);

    // Registra i peer di default
    register_peer(s_peer1Mac);
    register_peer(s_peer2Mac);

    s_audioMutex = xSemaphoreCreateMutex();

    Serial.println("[WIRELESS] Sistema wireless TX inizializzato");
    Serial.printf("[WIRELESS] CH1 MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
        s_peer1Mac[0], s_peer1Mac[1], s_peer1Mac[2],
        s_peer1Mac[3], s_peer1Mac[4], s_peer1Mac[5]);
    Serial.printf("[WIRELESS] CH2 MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
        s_peer2Mac[0], s_peer2Mac[1], s_peer2Mac[2],
        s_peer2Mac[3], s_peer2Mac[4], s_peer2Mac[5]);

    return true;
}

void wireless_tx_enable(bool en) {
    s_enabled = en;
    Serial.printf("[WIRELESS] %s\n", en ? "Abilitato" : "Disabilitato");
}

bool wireless_tx_is_enabled() { return s_enabled; }

bool wireless_tx_set_peer_mac(uint8_t channelId, const uint8_t mac[6]) {
    if (channelId == 1) {
        memcpy(s_peer1Mac, mac, 6);
    } else if (channelId == 2) {
        memcpy(s_peer2Mac, mac, 6);
    } else {
        return false;
    }
    return register_peer(mac);
}

void wireless_tx_get_peer_mac(uint8_t channelId, uint8_t mac[6]) {
    if (channelId == 1)      memcpy(mac, s_peer1Mac, 6);
    else if (channelId == 2) memcpy(mac, s_peer2Mac, 6);
}

void wireless_tx_set_audio_mode(WirelessAudioMode mode) {
    s_audioMode = mode;
    Serial.printf("[WIRELESS] Modalità audio: %d\n", (int)mode);
}

WirelessAudioMode wireless_tx_get_audio_mode() { return s_audioMode; }

bool wireless_tx_send_rs485(uint8_t channelId, const uint8_t* rs485Frame, uint8_t len) {
    if (len > 64) len = 64;
    Rs485FrameWireless frame = {};
    frame.magic     = WIRELESS_MAGIC_RS485;
    frame.channelId = channelId;
    frame.dataLen   = len;
    memcpy(frame.data, rs485Frame, len);
    // CRC su tutto il frame eccetto l'ultimo byte
    frame.crc8 = crc8_compute((const uint8_t*)&frame,
                               sizeof(frame) - sizeof(frame.crc8));
    return send_to_peer(channelId, &frame, sizeof(frame));
}

bool wireless_tx_send_dmx(uint8_t channelId, uint16_t startCh,
                           const uint8_t* dmxData, uint8_t numCh) {
    // Dividi in chunk da 200 byte
    uint8_t offset = 0;
    uint8_t chunkIdx = 0;
    bool ok = true;

    while (offset < numCh) {
        uint8_t chunkSize = (numCh - offset > 200) ? 200 : (numCh - offset);
        DmxFrameWireless frame = {};
        frame.magic     = WIRELESS_MAGIC_DMX;
        frame.channelId = channelId;
        frame.chunkIdx  = chunkIdx++;
        frame.startCh   = startCh + offset;
        frame.numCh     = chunkSize;
        memcpy(frame.dmxData, dmxData + offset, chunkSize);
        frame.crc8 = crc8_compute((const uint8_t*)&frame,
                                   sizeof(frame) - sizeof(frame.crc8));

        if (channelId == 0xFF) {
            // Broadcast: invia a entrambi i canali
            ok &= send_to_peer(1, &frame, sizeof(frame));
            ok &= send_to_peer(2, &frame, sizeof(frame));
        } else {
            ok &= send_to_peer(channelId, &frame, sizeof(frame));
        }
        offset += chunkSize;
    }
    return ok;
}

bool wireless_tx_is_online(uint8_t channelId) {
    if (channelId < 1 || channelId > 2) return false;
    check_peer_timeouts();
    return s_stats[channelId - 1].online;
}

const WirelessChannelStats* wireless_tx_get_stats(uint8_t channelId) {
    if (channelId < 1 || channelId > 2) return nullptr;
    check_peer_timeouts();
    return &s_stats[channelId - 1];
}

void wireless_tx_push_audio(const int16_t* stereoSamples, size_t numStereoSamples) {
    if (!s_audioMutex) return;
    if (xSemaphoreTake(s_audioMutex, pdMS_TO_TICKS(2)) != pdTRUE) return;

    // Scrivi campioni nel buffer circolare (frame da AUDIO_FRAME_SAMPLES campioni stereo)
    size_t spaceFrames = AUDIO_RING_BUF_FRAMES - s_audioBufCount;
    if (spaceFrames == 0) {
        // Buffer pieno: scarta il frame più vecchio
        s_audioBufReadIdx = (s_audioBufReadIdx + 1) % AUDIO_RING_BUF_FRAMES;
        s_audioBufCount--;
    }

    size_t writePos = s_audioBufWriteIdx * AUDIO_FRAME_SAMPLES * 2;
    size_t copyLen  = (numStereoSamples > (size_t)(AUDIO_FRAME_SAMPLES))
                      ? (size_t)(AUDIO_FRAME_SAMPLES)
                      : numStereoSamples;
    // Bounds check: verify write stays within the ring buffer
    if (writePos + copyLen * 2 <= sizeof(s_audioBuf) / sizeof(s_audioBuf[0])) {
        memcpy(&s_audioBuf[writePos], stereoSamples, copyLen * 2 * sizeof(int16_t));
    }

    s_audioBufWriteIdx = (s_audioBufWriteIdx + 1) % AUDIO_RING_BUF_FRAMES;
    s_audioBufCount++;

    xSemaphoreGive(s_audioMutex);
}

void wireless_tx_load_config(const WirelessConfig& cfg) {
    memcpy(s_peer1Mac, cfg.peer1Mac, 6);
    memcpy(s_peer2Mac, cfg.peer2Mac, 6);
    s_audioMode = cfg.mode;
    s_enabled   = cfg.enabled;
    // Re-registra i peer con nuovi MAC
    if (esp_now_is_peer_exist(cfg.peer1Mac)) esp_now_del_peer(cfg.peer1Mac);
    if (esp_now_is_peer_exist(cfg.peer2Mac)) esp_now_del_peer(cfg.peer2Mac);
    register_peer(s_peer1Mac);
    register_peer(s_peer2Mac);
}

void wireless_tx_get_config(WirelessConfig& cfg) {
    memcpy(cfg.peer1Mac, s_peer1Mac, 6);
    memcpy(cfg.peer2Mac, s_peer2Mac, 6);
    cfg.mode    = s_audioMode;
    cfg.enabled = s_enabled;
}

// ======= Task FreeRTOS trasmissione audio =======

void wireless_audio_tx_task(void* param) {
    Serial.println("[WIRELESS] Task audio TX avviato su Core 1");
    TickType_t lastWake = xTaskGetTickCount();

    for (;;) {
        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(1));  // 1ms = 1000Hz

        if (!s_enabled || !s_audioMutex) continue;
        if (xSemaphoreTake(s_audioMutex, 0) != pdTRUE) continue;

        if (s_audioBufCount == 0) {
            xSemaphoreGive(s_audioMutex);
            continue;
        }

        // Leggi frame dal buffer
        size_t readPos = s_audioBufReadIdx * AUDIO_FRAME_SAMPLES * 2;
        int16_t frame_data[AUDIO_FRAME_SAMPLES * 2];
        memcpy(frame_data, &s_audioBuf[readPos], sizeof(frame_data));
        s_audioBufReadIdx = (s_audioBufReadIdx + 1) % AUDIO_RING_BUF_FRAMES;
        s_audioBufCount--;

        xSemaphoreGive(s_audioMutex);

        // Prepara frame audio per CH1
        AudioFramePerChannel ch1Frame = {};
        ch1Frame.magic       = WIRELESS_MAGIC_AUDIO;
        ch1Frame.channelId   = 1;
        ch1Frame.seqNum      = s_seqCh1++;
        ch1Frame.timestampUs = (uint32_t)(esp_timer_get_time() & 0xFFFFFFFF);
        ch1Frame.sampleRate  = 48000;
        ch1Frame.numSamples  = AUDIO_FRAME_SAMPLES;

        // Prepara frame audio per CH2
        AudioFramePerChannel ch2Frame = {};
        ch2Frame.magic       = WIRELESS_MAGIC_AUDIO;
        ch2Frame.channelId   = 2;
        ch2Frame.seqNum      = s_seqCh2++;
        ch2Frame.timestampUs = ch1Frame.timestampUs;
        ch2Frame.sampleRate  = 48000;
        ch2Frame.numSamples  = AUDIO_FRAME_SAMPLES;

        // Distribuzione campioni secondo la modalità
        switch (s_audioMode) {
            case WirelessAudioMode::STEREO_SPLIT:
                // CH1 = Left, CH2 = Right (mono per canale)
                ch1Frame.audioMode = 0;  // Mono
                ch2Frame.audioMode = 0;
                for (int i = 0; i < AUDIO_FRAME_SAMPLES; i++) {
                    ch1Frame.samples[i] = frame_data[i * 2];      // Left
                    ch2Frame.samples[i] = frame_data[i * 2 + 1];  // Right
                }
                break;

            case WirelessAudioMode::DUAL_MONO:
                // Entrambi stereo completo
                ch1Frame.audioMode = 1;  // Stereo
                ch2Frame.audioMode = 1;
                memcpy(ch1Frame.samples, frame_data, sizeof(frame_data));
                memcpy(ch2Frame.samples, frame_data, sizeof(frame_data));
                break;

            case WirelessAudioMode::MAIN_SUB:
                // CH1 = full range, CH2 = sub (<120Hz, mono)
                ch1Frame.audioMode = 1;  // Stereo
                ch2Frame.audioMode = 2;  // Sub
                memcpy(ch1Frame.samples, frame_data, sizeof(frame_data));
                // Sub: mix L+R, poi passa-basso semplice (media su 2 campioni)
                for (int i = 0; i < AUDIO_FRAME_SAMPLES; i++) {
                    int32_t mix = ((int32_t)frame_data[i * 2] +
                                   (int32_t)frame_data[i * 2 + 1]) / 2;
                    ch2Frame.samples[i] = (int16_t)mix;
                }
                break;
        }

        // Calcola CRC e invia
        ch1Frame.crc8 = crc8_compute((const uint8_t*)&ch1Frame,
                                      sizeof(ch1Frame) - 1);
        ch2Frame.crc8 = crc8_compute((const uint8_t*)&ch2Frame,
                                      sizeof(ch2Frame) - 1);

        send_to_peer(1, &ch1Frame, sizeof(ch1Frame));
        send_to_peer(2, &ch2Frame, sizeof(ch2Frame));
    }
}
