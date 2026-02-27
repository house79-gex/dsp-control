#include "dmx512.h"
#include "config.h"
#include <Arduino.h>
#include <string.h>
#include <vector>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// ======= Variabili di stato =======

// Buffer DMX universo (512 canali + 1 start code)
static uint8_t  s_dmxBuffer[DMX_CHANNELS + 1] = { 0 };  // [0]=start code(0x00), [1-512]=dati
static uint8_t  s_masterDimmer  = 255;   // Master dimmer globale
static bool     s_blackout      = false; // Stato blackout
static uint8_t  s_activeScene   = 0;     // Scena attiva (0=nessuna)
static bool     s_sceneRunning  = false; // Scena in esecuzione
static uint32_t s_sceneStartMs  = 0;     // Timestamp avvio scena

// Fixture, gruppi e scene registrati
static std::vector<DmxFixture> s_fixtures;
static std::vector<DmxGroup>   s_groups;
static std::vector<DmxScene>   s_scenes;

// Mutex per accesso thread-safe al buffer DMX
static portMUX_TYPE s_dmxMux = portMUX_INITIALIZER_UNLOCKED;

// ======= Scene predefinite di fabbrica (12 scene) =======

static void init_factory_scenes() {
    // 1. BLACKOUT
    DmxScene blackout = {};
    blackout.id = 1;
    strncpy(blackout.name, "BLACKOUT", sizeof(blackout.name) - 1);
    blackout.fadeInTime  = 0;
    blackout.fadeOutTime = 0;
    blackout.isFactory   = true;
    s_scenes.push_back(blackout);

    // 2. FULL_WHITE
    DmxScene fullWhite = {};
    fullWhite.id = 2;
    strncpy(fullWhite.name, "FULL_WHITE", sizeof(fullWhite.name) - 1);
    // Tutti i canali a 255 (gestito dinamicamente in dmx_play_scene)
    fullWhite.fadeInTime  = 1.0f;
    fullWhite.fadeOutTime = 1.0f;
    fullWhite.isFactory   = true;
    s_scenes.push_back(fullWhite);

    // 3. WARM_STAGE – ambra/caldo
    DmxScene warmStage = {};
    warmStage.id = 3;
    strncpy(warmStage.name, "WARM_STAGE", sizeof(warmStage.name) - 1);
    warmStage.fadeInTime  = 2.0f;
    warmStage.fadeOutTime = 2.0f;
    warmStage.isFactory   = true;
    s_scenes.push_back(warmStage);

    // 4. COOL_STAGE – blu/freddo
    DmxScene coolStage = {};
    coolStage.id = 4;
    strncpy(coolStage.name, "COOL_STAGE", sizeof(coolStage.name) - 1);
    coolStage.fadeInTime  = 2.0f;
    coolStage.fadeOutTime = 2.0f;
    coolStage.isFactory   = true;
    s_scenes.push_back(coolStage);

    // 5. CONCERT_ROCK – rossi intensi
    DmxScene concertRock = {};
    concertRock.id = 5;
    strncpy(concertRock.name, "CONCERT_ROCK", sizeof(concertRock.name) - 1);
    concertRock.fadeInTime  = 0.5f;
    concertRock.fadeOutTime = 0.5f;
    concertRock.isFactory   = true;
    s_scenes.push_back(concertRock);

    // 6. CONCERT_POP – colori vivaci
    DmxScene concertPop = {};
    concertPop.id = 6;
    strncpy(concertPop.name, "CONCERT_POP", sizeof(concertPop.name) - 1);
    concertPop.fadeInTime  = 1.0f;
    concertPop.fadeOutTime = 1.0f;
    concertPop.loop        = true;
    concertPop.speed       = 1.0f;
    concertPop.isFactory   = true;
    s_scenes.push_back(concertPop);

    // 7. SPEECH – luce neutra bianca
    DmxScene speech = {};
    speech.id = 7;
    strncpy(speech.name, "SPEECH", sizeof(speech.name) - 1);
    speech.fadeInTime  = 3.0f;
    speech.fadeOutTime = 3.0f;
    speech.isFactory   = true;
    s_scenes.push_back(speech);

    // 8. PARTY_STROBE – strobo veloce
    DmxScene partyStrobe = {};
    partyStrobe.id = 8;
    strncpy(partyStrobe.name, "PARTY_STROBE", sizeof(partyStrobe.name) - 1);
    partyStrobe.fadeInTime  = 0;
    partyStrobe.fadeOutTime = 0;
    partyStrobe.loop        = true;
    partyStrobe.speed       = 2.0f;
    partyStrobe.isFactory   = true;
    s_scenes.push_back(partyStrobe);

    // 9. SUNSET_FADE – fade lento
    DmxScene sunsetFade = {};
    sunsetFade.id = 9;
    strncpy(sunsetFade.name, "SUNSET_FADE", sizeof(sunsetFade.name) - 1);
    sunsetFade.fadeInTime  = 5.0f;
    sunsetFade.fadeOutTime = 5.0f;
    sunsetFade.loop        = true;
    sunsetFade.speed       = 0.5f;
    sunsetFade.holdTime    = 30.0f;
    sunsetFade.isFactory   = true;
    s_scenes.push_back(sunsetFade);

    // 10. BLUE_MOON – blu profondo
    DmxScene blueMoon = {};
    blueMoon.id = 10;
    strncpy(blueMoon.name, "BLUE_MOON", sizeof(blueMoon.name) - 1);
    blueMoon.fadeInTime  = 4.0f;
    blueMoon.fadeOutTime = 4.0f;
    blueMoon.isFactory   = true;
    s_scenes.push_back(blueMoon);

    // 11. RAINBOW_CHASE – chase arcobaleno
    DmxScene rainbowChase = {};
    rainbowChase.id = 11;
    strncpy(rainbowChase.name, "RAINBOW_CHASE", sizeof(rainbowChase.name) - 1);
    rainbowChase.fadeInTime  = 1.0f;
    rainbowChase.fadeOutTime = 1.0f;
    rainbowChase.loop        = true;
    rainbowChase.speed       = 1.5f;
    rainbowChase.isFactory   = true;
    s_scenes.push_back(rainbowChase);

    // 12. CUSTOM_1 – slot personalizzabile
    DmxScene custom1 = {};
    custom1.id = 12;
    strncpy(custom1.name, "CUSTOM_1", sizeof(custom1.name) - 1);
    custom1.isFactory = false;
    s_scenes.push_back(custom1);

    // 13. CUSTOM_2
    DmxScene custom2 = {};
    custom2.id = 13;
    strncpy(custom2.name, "CUSTOM_2", sizeof(custom2.name) - 1);
    custom2.isFactory = false;
    s_scenes.push_back(custom2);

    // 14. CUSTOM_3
    DmxScene custom3 = {};
    custom3.id = 14;
    strncpy(custom3.name, "CUSTOM_3", sizeof(custom3.name) - 1);
    custom3.isFactory = false;
    s_scenes.push_back(custom3);
}

// ======= Inizializzazione =======

void dmx_init() {
    // Inizializza buffer con start code 0x00
    memset(s_dmxBuffer, 0, sizeof(s_dmxBuffer));

    // Configura UART1 per DMX512 (250kbaud, 8N2)
    // DMX_TX e DMX_DE definiti in config.h
    Serial1.begin(DMX_BAUD_RATE, SERIAL_8N2, -1, DMX_TX);
    pinMode(DMX_DE, OUTPUT);
    digitalWrite(DMX_DE, HIGH);  // Output sempre abilitato per DMX

    // Inizializza scene di fabbrica
    init_factory_scenes();

    Serial.println("[DMX] Controller DMX512 inizializzato");
    Serial.printf("[DMX] TX=GPIO%d DE=GPIO%d Frame=%dHz\n", DMX_TX, DMX_DE, DMX_FRAME_RATE_HZ);
}

// ======= Output DMX (inviato dal task FreeRTOS) =======

static void dmx_send_frame() {
    // Sequenza DMX512:
    // 1. Break: linea LOW per almeno 88µs
    // 2. MAB (Mark After Break): linea HIGH per almeno 8µs
    // 3. Start byte (0x00) + 512 canali

    // Break: forza UART in idle (HIGH) poi invia break manuale
    Serial1.end();
    pinMode(DMX_TX, OUTPUT);
    digitalWrite(DMX_TX, LOW);  // Break
    delayMicroseconds(DMX_BREAK_US);
    digitalWrite(DMX_TX, HIGH); // MAB
    delayMicroseconds(DMX_MAB_US);
    Serial1.begin(DMX_BAUD_RATE, SERIAL_8N2, -1, DMX_TX);

    // Invia start code + 512 canali
    taskENTER_CRITICAL(&s_dmxMux);
    Serial1.write(s_dmxBuffer, DMX_CHANNELS + 1);
    taskEXIT_CRITICAL(&s_dmxMux);
    Serial1.flush();
}

// Task FreeRTOS dedicato per output DMX a ~40Hz su Core 1
void dmx_task(void* param) {
    const TickType_t period = pdMS_TO_TICKS(DMX_FRAME_PERIOD_MS);
    TickType_t lastWake = xTaskGetTickCount();

    Serial.println("[DMX] Task DMX avviato su Core 1");

    for (;;) {
        // Aggiorna scene dinamiche (effetti loop, fade)
        if (s_sceneRunning && s_activeScene != 0) {
            uint32_t elapsed = millis() - s_sceneStartMs;
            DmxScene* scene = dmx_get_scene(s_activeScene);
            if (scene) {
                // Gestione scene dinamiche (CONCERT_POP, RAINBOW_CHASE, ecc.)
                if (scene->loop && scene->speed > 0) {
                    float t = (elapsed / 1000.0f) * scene->speed;
                    uint8_t phase = (uint8_t)((uint32_t)(t * 255) & 0xFF);

                    if (strcmp(scene->name, "RAINBOW_CHASE") == 0) {
                        // Chase arcobaleno: ogni fixture ha un offset di fase diverso
                        for (size_t i = 0; i < s_fixtures.size(); i++) {
                            uint8_t hue = phase + (uint8_t)(i * 255 / (s_fixtures.size() + 1));
                            // Converti hue in RGB (formula semplificata)
                            uint8_t r, g, b;
                            uint8_t h6 = hue / 43;
                            uint8_t f  = (hue - h6 * 43) * 6;
                            switch (h6 % 6) {
                                case 0: r=255; g=f;   b=0;   break;
                                case 1: r=255-f; g=255; b=0; break;
                                case 2: r=0; g=255; b=f;     break;
                                case 3: r=0; g=255-f; b=255; break;
                                case 4: r=f; g=0; b=255;     break;
                                default: r=255; g=0; b=255-f; break;
                            }
                            dmx_set_fixture_color(s_fixtures[i].id, r, g, b);
                        }
                    } else if (strcmp(scene->name, "CONCERT_POP") == 0) {
                        // Ciclo colori vivaci
                        uint8_t r = (uint8_t)(sin(t * 1.0f) * 127 + 128);
                        uint8_t g = (uint8_t)(sin(t * 1.5f) * 127 + 128);
                        uint8_t blue = (uint8_t)(sin(t * 0.7f) * 127 + 128);
                        for (auto& fx : s_fixtures) {
                            dmx_set_fixture_color(fx.id, r, g, blue);
                        }
                    } else if (strcmp(scene->name, "SUNSET_FADE") == 0) {
                        // Fade rosso-arancio-viola
                        float cycle = fmodf(t / 10.0f, 1.0f);
                        uint8_t r, g, blue;
                        if (cycle < 0.33f) {
                            float f2 = cycle / 0.33f;
                            r = 255; g = (uint8_t)(f2 * 100); blue = 0;
                        } else if (cycle < 0.66f) {
                            float f2 = (cycle - 0.33f) / 0.33f;
                            r = 255; g = (uint8_t)(100 - f2 * 100); blue = (uint8_t)(f2 * 150);
                        } else {
                            float f2 = (cycle - 0.66f) / 0.34f;
                            r = 255; g = 0; blue = (uint8_t)(150 - f2 * 150);
                        }
                        for (auto& fx : s_fixtures) {
                            dmx_set_fixture_color(fx.id, r, g, blue);
                        }
                    }
                }
            }
        }

        // Invia frame DMX
        dmx_send_frame();

        // Attendi prossimo ciclo (periodo fisso 25ms)
        vTaskDelayUntil(&lastWake, period);
    }
}

// ======= API pubblica =======

void dmx_set_channel(uint16_t channel, uint8_t value) {
    if (channel < 1 || channel > DMX_CHANNELS) return;
    taskENTER_CRITICAL(&s_dmxMux);
    s_dmxBuffer[channel] = s_blackout ? 0 : (uint8_t)((value * s_masterDimmer) / 255);
    taskEXIT_CRITICAL(&s_dmxMux);
}

void dmx_set_fixture_channel(uint8_t fixtureId, uint8_t channelIdx, uint8_t value) {
    DmxFixture* fx = dmx_get_fixture(fixtureId);
    if (!fx || channelIdx >= fx->numChannels) return;
    fx->channels[channelIdx] = value;
    dmx_set_channel(fx->startAddress + channelIdx, value);
}

void dmx_set_fixture_color(uint8_t fixtureId, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    DmxFixture* fx = dmx_get_fixture(fixtureId);
    if (!fx) return;

    // Mappa colori sui canali corretti in base al tipo fixture
    if (strstr(fx->type, "RGB") || strstr(fx->type, "PAR") ||
        strstr(fx->type, "MOVING") || strstr(fx->type, "WASH")) {
        if (fx->numChannels >= 3) {
            dmx_set_fixture_channel(fixtureId, 0, r);
            dmx_set_fixture_channel(fixtureId, 1, g);
            dmx_set_fixture_channel(fixtureId, 2, b);
        }
        if (strstr(fx->type, "RGBW") && fx->numChannels >= 4) {
            dmx_set_fixture_channel(fixtureId, 3, w);
        }
    }
}

void dmx_set_fixture_dimmer(uint8_t fixtureId, uint8_t value) {
    DmxFixture* fx = dmx_get_fixture(fixtureId);
    if (!fx) return;
    // Il dimmer è tipicamente l'ultimo canale o il canale 0 per fixture con dimmer separato
    // Per semplicità, scala tutti i canali colore
    uint8_t scale = s_blackout ? 0 : (uint8_t)((value * s_masterDimmer) / 255);
    taskENTER_CRITICAL(&s_dmxMux);
    for (uint8_t ch = 0; ch < fx->numChannels; ch++) {
        s_dmxBuffer[fx->startAddress + ch] = (uint8_t)(((uint16_t)fx->channels[ch] * scale) / 255);
    }
    taskEXIT_CRITICAL(&s_dmxMux);
}

void dmx_set_master_dimmer(uint8_t value) {
    s_masterDimmer = value;
    // Ricalcola tutti i canali con il nuovo master dimmer
    if (!s_blackout) {
        for (auto& fx : s_fixtures) {
            dmx_set_fixture_dimmer(fx.id, 255);  // Riapplica con nuovo master
        }
    }
    Serial.printf("[DMX] Master dimmer → %d\n", value);
}

void dmx_blackout() {
    s_blackout = true;
    taskENTER_CRITICAL(&s_dmxMux);
    memset(&s_dmxBuffer[1], 0, DMX_CHANNELS);  // Azzera tutti i canali (preserva start code)
    taskEXIT_CRITICAL(&s_dmxMux);
    Serial.println("[DMX] BLACKOUT attivato");
}

void dmx_set_group_color(uint8_t groupId, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    for (auto& grp : s_groups) {
        if (grp.id == groupId) {
            for (uint8_t i = 0; i < grp.numFixtures; i++) {
                dmx_set_fixture_color(grp.fixtureIds[i], r, g, b, w);
            }
            s_blackout = false;
            return;
        }
    }
}

void dmx_set_group_dimmer(uint8_t groupId, uint8_t value) {
    for (auto& grp : s_groups) {
        if (grp.id == groupId) {
            for (uint8_t i = 0; i < grp.numFixtures; i++) {
                dmx_set_fixture_dimmer(grp.fixtureIds[i], value);
            }
            return;
        }
    }
}

void dmx_play_scene(uint8_t sceneId) {
    DmxScene* scene = dmx_get_scene(sceneId);
    if (!scene) {
        Serial.printf("[DMX] Scena %d non trovata\n", sceneId);
        return;
    }

    s_blackout      = false;
    s_activeScene   = sceneId;
    s_sceneRunning  = true;
    s_sceneStartMs  = millis();

    // Applica valori iniziali della scena
    if (strcmp(scene->name, "BLACKOUT") == 0) {
        dmx_blackout();
    } else if (strcmp(scene->name, "FULL_WHITE") == 0) {
        for (auto& fx : s_fixtures) {
            dmx_set_fixture_color(fx.id, 255, 255, 255, 255);
        }
    } else if (strcmp(scene->name, "WARM_STAGE") == 0) {
        for (auto& fx : s_fixtures) {
            dmx_set_fixture_color(fx.id, 255, 180, 80);
            dmx_set_fixture_dimmer(fx.id, 178);  // 70%
        }
    } else if (strcmp(scene->name, "COOL_STAGE") == 0) {
        for (auto& fx : s_fixtures) {
            dmx_set_fixture_color(fx.id, 50, 100, 255);
            dmx_set_fixture_dimmer(fx.id, 153);  // 60%
        }
    } else if (strcmp(scene->name, "CONCERT_ROCK") == 0) {
        for (auto& fx : s_fixtures) {
            dmx_set_fixture_color(fx.id, 255, 0, 0);
        }
    } else if (strcmp(scene->name, "SPEECH") == 0) {
        for (auto& fx : s_fixtures) {
            dmx_set_fixture_color(fx.id, 255, 255, 240);
            dmx_set_fixture_dimmer(fx.id, 204);  // 80%
        }
    } else if (strcmp(scene->name, "PARTY_STROBE") == 0) {
        for (auto& fx : s_fixtures) {
            dmx_set_fixture_color(fx.id, 255, 255, 255);
        }
    } else if (strcmp(scene->name, "BLUE_MOON") == 0) {
        for (auto& fx : s_fixtures) {
            dmx_set_fixture_color(fx.id, 0, 10, 180);
        }
    } else {
        // Applica valori fissi della scena personalizzata
        for (uint8_t i = 0; i < scene->numFixtures; i++) {
            uint8_t fxId = scene->fixtureIds[i];
            DmxFixture* fx = dmx_get_fixture(fxId);
            if (!fx) continue;
            for (uint8_t ch = 0; ch < fx->numChannels && ch < DMX_MAX_CHANNELS_PER_FIXTURE; ch++) {
                dmx_set_fixture_channel(fxId, ch, scene->fixtureChannels[i][ch]);
            }
        }
    }

    Serial.printf("[DMX] Scena '%s' avviata\n", scene->name);
}

void dmx_stop_scene() {
    s_sceneRunning = false;
    s_activeScene  = 0;
    Serial.println("[DMX] Scena fermata");
}

uint8_t dmx_get_active_scene() {
    return s_activeScene;
}

bool dmx_add_fixture(const DmxFixture& fixture) {
    // Rimuovi eventuale fixture esistente con lo stesso ID
    dmx_remove_fixture(fixture.id);
    s_fixtures.push_back(fixture);
    Serial.printf("[DMX] Fixture aggiunta: ID=%d '%s' addr=%d ch=%d\n",
                  fixture.id, fixture.name, fixture.startAddress, fixture.numChannels);
    return true;
}

bool dmx_remove_fixture(uint8_t fixtureId) {
    for (auto it = s_fixtures.begin(); it != s_fixtures.end(); ++it) {
        if (it->id == fixtureId) {
            s_fixtures.erase(it);
            return true;
        }
    }
    return false;
}

DmxFixture* dmx_get_fixture(uint8_t fixtureId) {
    for (auto& fx : s_fixtures) {
        if (fx.id == fixtureId) return &fx;
    }
    return nullptr;
}

std::vector<DmxFixture*> dmx_get_all_fixtures() {
    std::vector<DmxFixture*> result;
    for (auto& fx : s_fixtures) result.push_back(&fx);
    return result;
}

bool dmx_add_group(const DmxGroup& group) {
    for (auto it = s_groups.begin(); it != s_groups.end(); ++it) {
        if (it->id == group.id) { s_groups.erase(it); break; }
    }
    s_groups.push_back(group);
    return true;
}

bool dmx_remove_group(uint8_t groupId) {
    for (auto it = s_groups.begin(); it != s_groups.end(); ++it) {
        if (it->id == groupId) { s_groups.erase(it); return true; }
    }
    return false;
}

std::vector<DmxGroup*> dmx_get_all_groups() {
    std::vector<DmxGroup*> result;
    for (auto& grp : s_groups) result.push_back(&grp);
    return result;
}

bool dmx_save_scene(const DmxScene& scene) {
    for (auto it = s_scenes.begin(); it != s_scenes.end(); ++it) {
        if (it->id == scene.id && !it->isFactory) {
            *it = scene;
            Serial.printf("[DMX] Scena '%s' aggiornata\n", scene.name);
            return true;
        }
    }
    s_scenes.push_back(scene);
    Serial.printf("[DMX] Scena '%s' salvata\n", scene.name);
    return true;
}

bool dmx_delete_scene(uint8_t sceneId) {
    for (auto it = s_scenes.begin(); it != s_scenes.end(); ++it) {
        if (it->id == sceneId && !it->isFactory) {
            s_scenes.erase(it);
            return true;
        }
    }
    return false;
}

DmxScene* dmx_get_scene(uint8_t sceneId) {
    for (auto& s : s_scenes) {
        if (s.id == sceneId) return &s;
    }
    return nullptr;
}

std::vector<DmxScene*> dmx_get_all_scenes() {
    std::vector<DmxScene*> result;
    for (auto& s : s_scenes) result.push_back(&s);
    return result;
}

const uint8_t* dmx_get_buffer() {
    return s_dmxBuffer;
}
