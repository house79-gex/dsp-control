#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>
#include "config.h"
#include "audio_rx.h"
#include "rs485_rx.h"
#include "dmx_rx.h"

// ======= Strutture frame wireless (condivise con master TX) =======

#define WIRELESS_MAGIC_AUDIO   0xAA55
#define WIRELESS_MAGIC_RS485   0xBB66
#define WIRELESS_MAGIC_DMX     0xCC77

struct AudioFramePerChannel {
    uint16_t magic;
    uint8_t  channelId;
    uint8_t  flags;
    uint16_t seqNum;
    uint32_t timestampUs;
    uint16_t sampleRate;
    uint8_t  audioMode;
    uint8_t  numSamples;
    int16_t  samples[96];
    uint8_t  crc8;
} __attribute__((packed));

struct Rs485FrameWireless {
    uint16_t magic;
    uint8_t  channelId;
    uint8_t  dataLen;
    uint8_t  data[64];
    uint8_t  crc8;
} __attribute__((packed));

struct DmxFrameWireless {
    uint16_t magic;
    uint8_t  channelId;
    uint8_t  chunkIdx;
    uint16_t startCh;
    uint8_t  numCh;
    uint8_t  dmxData[200];
    uint8_t  crc8;
} __attribute__((packed));

// ======= Statistiche ricezione =======

static uint32_t s_pktReceived = 0;
static uint32_t s_pktIgnored  = 0;   // Altro canale
static uint32_t s_pktCrcErr   = 0;
static uint32_t s_pktLost     = 0;
static uint16_t s_lastSeqAudio = 0;
static uint32_t s_lastPktMs    = 0;

// ======= CRC8 =======

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

// ======= LED stato =======

static void led_set_color(uint8_t r, uint8_t g, uint8_t b) {
    // LED integrato ESP32-S3 (se WS2812B)
    // Semplice: usa GPIO integrato se non disponibile WS2812B
    (void)r; (void)g; (void)b;
    // Implementazione hardware-specifica: da adattare al modulo usato
}

static void update_status_led() {
    uint32_t now = millis();
    bool offline = (now - s_lastPktMs) > WIRELESS_TIMEOUT_MS;
    float lossRate = (s_pktReceived + s_pktLost > 0)
        ? (float)s_pktLost / (float)(s_pktReceived + s_pktLost)
        : 0.0f;

    if (offline) {
        // Rosso lampeggiante: offline
        static bool blink = false;
        blink = !blink;
        led_set_color(blink ? 255 : 0, 0, 0);
    } else if (lossRate > 0.05f) {
        // Giallo lampeggiante: packet loss >5%
        static bool blink = false;
        blink = !blink;
        led_set_color(blink ? 255 : 0, blink ? 200 : 0, 0);
    } else {
        // Verde fisso: OK
        led_set_color(0, 255, 0);
    }
}

// ======= Callback ESP-NOW ricezione =======

static void on_data_recv(const uint8_t* mac_addr, const uint8_t* data, int len) {
    if (len < 2) return;
    uint16_t magic = *(const uint16_t*)data;

    s_pktReceived++;
    s_lastPktMs = millis();

    if (magic == WIRELESS_MAGIC_AUDIO) {
        if ((size_t)len < sizeof(AudioFramePerChannel)) return;
        const AudioFramePerChannel* frame = (const AudioFramePerChannel*)data;

        // Filtra per canale
        if (frame->channelId != MY_CHANNEL_ID) {
            s_pktIgnored++;
            return;
        }

        // Verifica CRC
        uint8_t expectedCrc = crc8_compute(data, len - 1);
        if (frame->crc8 != expectedCrc) {
            s_pktCrcErr++;
            return;
        }

        // Rileva packet loss (confronta seqNum con wraparound uint16)
        if (s_lastSeqAudio > 0) {
            uint16_t expected = (uint16_t)(s_lastSeqAudio + 1);
            if (frame->seqNum != expected) {
                // Calcola differenza con aritmetica modulare uint16
                uint16_t diff = (uint16_t)(frame->seqNum - expected);
                if (diff < 32768u) {  // Massimo gap ragionevole (metà range)
                    s_pktLost += diff;
                }
            }
        }
        s_lastSeqAudio = frame->seqNum;

        // Invia campioni al buffer playback
        audio_rx_push_samples(frame->samples, frame->numSamples, frame->audioMode);

    } else if (magic == WIRELESS_MAGIC_RS485) {
        if ((size_t)len < sizeof(Rs485FrameWireless)) return;
        const Rs485FrameWireless* frame = (const Rs485FrameWireless*)data;

        if (frame->channelId != MY_CHANNEL_ID && frame->channelId != 0xFF) {
            s_pktIgnored++;
            return;
        }

        uint8_t expectedCrc = crc8_compute(data, len - 1);
        if (frame->crc8 != expectedCrc) {
            s_pktCrcErr++;
            return;
        }

        rs485_rx_forward(frame->data, frame->dataLen);

    } else if (magic == WIRELESS_MAGIC_DMX) {
        if ((size_t)len < sizeof(DmxFrameWireless)) return;
        const DmxFrameWireless* frame = (const DmxFrameWireless*)data;

        if (frame->channelId != MY_CHANNEL_ID && frame->channelId != 0xFF) {
            s_pktIgnored++;
            return;
        }

        uint8_t expectedCrc = crc8_compute(data, len - 1);
        if (frame->crc8 != expectedCrc) {
            s_pktCrcErr++;
            return;
        }

        dmx_rx_apply_chunk(frame->startCh, frame->dmxData, frame->numCh);
    }
}

// ======= Task statistiche seriali =======

static void stats_task(void* param) {
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(5000));
        uint32_t total = s_pktReceived + s_pktLost;
        float lossRate = (total > 0) ? ((float)s_pktLost / (float)total * 100.0f) : 0.0f;

        Serial.printf("[STATS CH%d] Ricevuti=%lu Persi=%lu Ignorati=%lu CRC_ERR=%lu "
                      "Loss=%.1f%% AudioUnderrun=%lu AudioFrames=%lu\n",
                      MY_CHANNEL_ID,
                      (unsigned long)s_pktReceived,
                      (unsigned long)s_pktLost,
                      (unsigned long)s_pktIgnored,
                      (unsigned long)s_pktCrcErr,
                      lossRate,
                      (unsigned long)audio_rx_get_underruns(),
                      (unsigned long)audio_rx_get_frames_played());
    }
}

// ======= Setup =======

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.printf("\n[RX] ===== Modulo RX CH%d – avvio =====\n", MY_CHANNEL_ID);

    // WiFi in STA mode per ESP-NOW
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // Stampa MAC address per configurare il master
    Serial.printf("[RX] MAC: %s\n", WiFi.macAddress().c_str());

    // Inizializzazione moduli
    audio_rx_init();
    rs485_rx_init();
    dmx_rx_init();

    // Inizializzazione ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("[RX] Errore init ESP-NOW!");
        return;
    }
    esp_now_register_recv_cb(on_data_recv);

    Serial.printf("[RX] ESP-NOW inizializzato – in ascolto su CH%d\n", MY_CHANNEL_ID);

    // Task FreeRTOS
    xTaskCreatePinnedToCore(audio_rx_task, "AUDIO_RX", 4096, nullptr, 8, nullptr, 1);
    xTaskCreatePinnedToCore(dmx_rx_task,   "DMX_OUT",  4096, nullptr, 5, nullptr, 1);
    xTaskCreatePinnedToCore(stats_task,    "STATS",    2048, nullptr, 1, nullptr, 0);

    Serial.printf("[RX] Pronto – Modulo RX CH%d attivo\n", MY_CHANNEL_ID);
}

void loop() {
    update_status_led();
    delay(500);
}
