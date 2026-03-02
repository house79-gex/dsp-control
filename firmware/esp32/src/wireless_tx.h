#pragma once
#include <Arduino.h>
#include <cstdint>

// ======= Sistema Wireless 2 Canali – Trasmettitore (Master TX) =======
// Trasmissione Audio + RS-485 + DMX su 2 canali ESP-NOW indipendenti
// CH1 e CH2 con peer MAC configurabili

// Modalità audio wireless
enum class WirelessAudioMode : uint8_t {
    STEREO_SPLIT = 0,  // CH1=Left, CH2=Right (da stereo ES8388)
    DUAL_MONO    = 1,  // CH1=Stereo A, CH2=Stereo B (entrambi stereo completo)
    MAIN_SUB     = 2,  // CH1=Full range, CH2=Sub (<120Hz crossover)
};

// Frame audio wireless per canale
struct AudioFramePerChannel {
    uint16_t magic;         // 0xAA55
    uint8_t  channelId;     // 1 o 2
    uint8_t  flags;         // riservato
    uint16_t seqNum;        // contatore per canale
    uint32_t timestampUs;   // sync
    uint16_t sampleRate;    // 48000
    uint8_t  audioMode;     // 0=Mono, 1=Stereo, 2=Sub
    uint8_t  numSamples;    // 48
    int16_t  samples[96];   // max 48 stereo
    uint8_t  crc8;
} __attribute__((packed));

// Frame RS-485 wireless
struct Rs485FrameWireless {
    uint16_t magic;         // 0xBB66
    uint8_t  channelId;     // 1 o 2
    uint8_t  dataLen;       // lunghezza dati (max 64)
    uint8_t  data[64];      // payload RS-485
    uint8_t  crc8;
} __attribute__((packed));

// Frame DMX wireless (chunk)
struct DmxFrameWireless {
    uint16_t magic;         // 0xCC77
    uint8_t  channelId;     // 1, 2 o 0xFF (broadcast)
    uint8_t  chunkIdx;      // indice chunk (0, 1, 2)
    uint16_t startCh;       // canale DMX iniziale (0-511)
    uint8_t  numCh;         // numero canali nel chunk (max 200)
    uint8_t  dmxData[200];  // valori DMX
    uint8_t  crc8;
} __attribute__((packed));

// Statistiche per canale
struct WirelessChannelStats {
    uint32_t packetsSent;      // Pacchetti inviati
    uint32_t packetsAcked;     // Pacchetti confermati (ACK)
    uint32_t packetsFailed;    // Pacchetti falliti
    float    lossRate;         // Percentuale perdita (0.0-1.0)
    int8_t   rssi;             // RSSI ultimo pacchetto (dBm)
    bool     online;           // Peer raggiungibile
    uint32_t lastSeenMs;       // Timestamp ultima risposta (millis)
};

// Configurazione wireless
struct WirelessConfig {
    uint8_t peer1Mac[6];       // MAC peer CH1
    uint8_t peer2Mac[6];       // MAC peer CH2
    WirelessAudioMode mode;    // Modalità audio
    bool enabled;              // Wireless abilitato
};

// ——— API pubblica ———

// Inizializza il modulo wireless TX (ESP-NOW + WiFi)
bool wireless_tx_init();

// Abilita/disabilita il sistema wireless
void wireless_tx_enable(bool en);
bool wireless_tx_is_enabled();

// Configura MAC per peer CH1 o CH2 (channelId = 1 o 2)
bool wireless_tx_set_peer_mac(uint8_t channelId, const uint8_t mac[6]);
void wireless_tx_get_peer_mac(uint8_t channelId, uint8_t mac[6]);

// Imposta modalità audio
void wireless_tx_set_audio_mode(WirelessAudioMode mode);
WirelessAudioMode wireless_tx_get_audio_mode();

// Invia frame RS-485 su canale specificato
bool wireless_tx_send_rs485(uint8_t channelId, const uint8_t* rs485Frame, uint8_t len);

// Invia dati DMX su canale specificato (channelId=0xFF = broadcast)
bool wireless_tx_send_dmx(uint8_t channelId, uint16_t startCh, const uint8_t* dmxData, uint8_t numCh);

// Stato peer
bool wireless_tx_is_online(uint8_t channelId);
const WirelessChannelStats* wireless_tx_get_stats(uint8_t channelId);

// Task FreeRTOS trasmissione audio (da registrare in main.cpp)
void wireless_audio_tx_task(void* param);

// Carica/salva configurazione wireless
void wireless_tx_load_config(const WirelessConfig& cfg);
void wireless_tx_get_config(WirelessConfig& cfg);

// Aggiorna buffer audio (chiamare dal task FFT con campioni ES8388)
void wireless_tx_push_audio(const int16_t* stereoSamples, size_t numStereoSamples);
