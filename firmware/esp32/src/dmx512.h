#pragma once
#include <Arduino.h>
#include <vector>
#include <string>
#include <cstdint>

// ======= Controller DMX512 per ESP32-S3 =======
// Standard USITT DMX512-A: 250kbaud, 8N2, break+MAB+slots
// Utilizza UART1 con task FreeRTOS dedicato su Core 1

// ——— Costanti protocollo DMX ———
#define DMX_BAUD_RATE       250000   // 250kbaud standard DMX512
#define DMX_CHANNELS        512      // Massimo canali DMX per universo
#define DMX_BREAK_US        88       // Durata break (µs) – minimo 88µs
#define DMX_MAB_US          8        // Mark After Break (µs) – minimo 8µs
#define DMX_FRAME_RATE_HZ   40       // Frame rate: 40 Hz (25ms per frame)
#define DMX_FRAME_PERIOD_MS 25       // Periodo frame in ms

// ——— Tipi fixture DMX ———
#define DMX_TYPE_PAR_RGB     "PAR_RGB"
#define DMX_TYPE_PAR_RGBW    "PAR_RGBW"
#define DMX_TYPE_MOVING_HEAD "MOVING_HEAD"
#define DMX_TYPE_WASH        "WASH"
#define DMX_TYPE_STROBE      "STROBE"
#define DMX_TYPE_SPOT        "SPOT"
#define DMX_TYPE_LASER       "LASER"
#define DMX_TYPE_SMOKE       "SMOKE"
#define DMX_MAX_CHANNELS_PER_FIXTURE 32

// ——— Struttura fixture DMX ———
struct DmxFixture {
    uint8_t     id;                                          // ID univoco (1-127)
    char        name[32];                                    // Nome ("PAR SX1")
    char        type[20];                                    // Tipo ("PAR_RGB")
    uint16_t    startAddress;                                // Indirizzo DMX (1-512)
    uint8_t     numChannels;                                 // Canali occupati (1-32)
    uint8_t     groupId;                                     // ID gruppo (0 = nessun gruppo)
    char        groupName[32];                               // Nome gruppo
    uint8_t     channels[DMX_MAX_CHANNELS_PER_FIXTURE];     // Valori attuali canali
    char        channelNames[DMX_MAX_CHANNELS_PER_FIXTURE][16]; // Nomi canali
    bool        online;                                      // Stato online
};

// ——— Struttura gruppo DMX ———
struct DmxGroup {
    uint8_t  id;           // ID gruppo
    char     name[32];     // Nome gruppo
    uint8_t  fixtureIds[32]; // IDs fixture nel gruppo
    uint8_t  numFixtures;    // Numero fixture nel gruppo
};

// ——— Struttura scena DMX ———
struct DmxScene {
    uint8_t  id;
    char     name[32];
    uint8_t  fixtureChannels[128][DMX_MAX_CHANNELS_PER_FIXTURE]; // [fixtureId][ch]
    uint8_t  numFixtures;
    uint8_t  fixtureIds[128];
    float    fadeInTime;   // Tempo fade-in (s, 0-10)
    float    fadeOutTime;  // Tempo fade-out (s, 0-10)
    float    holdTime;     // Durata hold (s, 0=infinito)
    bool     loop;         // Loop scena
    float    speed;        // Velocità effetto (1.0=normale)
    bool     isFactory;    // Scena di fabbrica
};

// ——— API pubblica ———

// Inizializza il controller DMX512 (UART1, pin da config.h)
void dmx_init();

// Imposta il valore di un singolo canale DMX (1-512)
void dmx_set_channel(uint16_t channel, uint8_t value);

// Imposta i canali di una fixture
void dmx_set_fixture_channel(uint8_t fixtureId, uint8_t channelIdx, uint8_t value);

// Imposta il colore RGB di una fixture
void dmx_set_fixture_color(uint8_t fixtureId, uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0);

// Imposta il dimmer di una fixture (0-255)
void dmx_set_fixture_dimmer(uint8_t fixtureId, uint8_t value);

// Imposta il master dimmer globale (0-255)
void dmx_set_master_dimmer(uint8_t value);

// Blackout immediato (tutti i canali a 0)
void dmx_blackout();

// Controlla un intero gruppo
void dmx_set_group_color(uint8_t groupId, uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0);
void dmx_set_group_dimmer(uint8_t groupId, uint8_t value);

// Attiva/disattiva una scena
void dmx_play_scene(uint8_t sceneId);
void dmx_stop_scene();
uint8_t dmx_get_active_scene();

// Aggiunge o modifica una fixture
bool dmx_add_fixture(const DmxFixture& fixture);
bool dmx_remove_fixture(uint8_t fixtureId);
DmxFixture* dmx_get_fixture(uint8_t fixtureId);
std::vector<DmxFixture*> dmx_get_all_fixtures();

// Aggiunge o modifica un gruppo
bool dmx_add_group(const DmxGroup& group);
bool dmx_remove_group(uint8_t groupId);
std::vector<DmxGroup*> dmx_get_all_groups();

// Salva/carica una scena
bool dmx_save_scene(const DmxScene& scene);
bool dmx_delete_scene(uint8_t sceneId);
DmxScene* dmx_get_scene(uint8_t sceneId);
std::vector<DmxScene*> dmx_get_all_scenes();

// Restituisce il buffer DMX corrente (512 byte)
const uint8_t* dmx_get_buffer();

// Task FreeRTOS per output DMX (da chiamare con xTaskCreate)
void dmx_task(void* param);
