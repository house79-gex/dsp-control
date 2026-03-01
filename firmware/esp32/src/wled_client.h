#pragma once
#include <stdint.h>
#include <stdbool.h>

// ======= WLED Client – Controllo strip neon LED via Wi-Fi =======

#define MAX_WLED_CONTROLLERS 8
#define MAX_WLED_SCENES      20

enum WledZone {
    ZONE_LOGO_LEFT   = 0,
    ZONE_LOGO_RIGHT  = 1,
    ZONE_FRONT_FRAME = 2,
    ZONE_SPARE       = 3
};

struct WledController {
    char     name[32];            // es. "Cabinet_SX"
    char     ip[16];              // es. "192.168.4.10"
    uint8_t  num_channels;        // 2 o 4
    uint16_t pixels_per_ch[4];    // pixel per canale
    WledZone zone_map[4];         // zona per ogni canale
    bool     online;              // stato connessione
    bool     sync_enabled;        // sincronizzazione con altri controller
    uint32_t last_seen_ms;        // ultimo contatto (ms)
};

struct WledScene {
    char    name[32];
    uint8_t effect_id;            // ID effetto WLED
    uint8_t r, g, b;              // colore primario
    uint8_t r2, g2, b2;           // colore secondario
    uint8_t brightness;           // 0-255
    uint8_t speed;                // velocità effetto 0-255
    uint8_t intensity;            // intensità effetto 0-255
    bool    mirror;               // rispecchia su zone accoppiate
    bool    reverse;              // inverte direzione
};

// ——— API pubblica ———

void wled_client_init();
void wled_client_add_controller(const WledController& ctrl);
void wled_client_remove_controller(uint8_t index);

void wled_client_set_color(uint8_t ctrl_idx, WledZone zone,
                            uint8_t r, uint8_t g, uint8_t b);
void wled_client_set_brightness(uint8_t ctrl_idx, WledZone zone,
                                 uint8_t brightness);
void wled_client_set_effect(uint8_t ctrl_idx, WledZone zone,
                             uint8_t effect_id, uint8_t speed, uint8_t intensity);

void wled_client_apply_scene(uint8_t scene_idx, bool all_controllers);
void wled_client_sync_all();
void wled_client_set_independent(uint8_t ctrl_idx);
void wled_client_mirror_zones(uint8_t ctrl_idx, WledZone zone_a, WledZone zone_b);
void wled_client_blackout();
bool wled_client_get_status(uint8_t ctrl_idx);
void wled_client_discover();

// ——— Accesso diretto a controller/scene in RAM ———

uint8_t         wled_get_controller_count();
WledController* wled_get_controller(uint8_t idx);

uint8_t         wled_get_scene_count();
WledScene*      wled_get_scene(uint8_t idx);
void            wled_add_scene(const WledScene& scene);
void            wled_remove_scene(uint8_t idx);
