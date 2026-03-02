#pragma once
#include <stdint.h>
#include <stdbool.h>

// ======= WLED Client – Controllo strip neon LED via Wi-Fi =======
//
// Supporto controller:
//   2CH – GLEDOPTO GL-C-015WL-D in modalità 2 canali
//          CH1 → ZONE_LOGO_LEFT, CH2 → ZONE_LOGO_RIGHT
//   4CH – GLEDOPTO GL-C-015WL-D in modalità 4 canali (default consigliato)
//          CH1 → ZONE_LOGO_LEFT, CH2 → ZONE_LOGO_RIGHT
//          CH3 → ZONE_FRONT_FRAME, CH4 → ZONE_SPARE
//
// Calcolo PSU per ~8m totali (logo_sx + logo_dx + front_frame):
//   WS2811 12V RGBIC 96 LED/m, 1 pixel = 3 LED → ~9.6 W/m
//   P = 8m × 9.6 W/m = 76.8 W  →  I = 6.4 A a 12 V
//   Alimentatore consigliato: Mean Well LRS-150-12 (12V, 12.5A, 150W)
//   Alternativa:              Mean Well LRS-100-12 (12V, 8.5A, 100W)

#define MAX_WLED_CONTROLLERS 8
#define MAX_WLED_SCENES      20

// Zone WLED – ogni zona corrisponde a un canale del controller 4CH.
// In modalità 2CH solo ZONE_LOGO_LEFT e ZONE_LOGO_RIGHT sono disponibili.
enum WledZone {
    ZONE_LOGO_LEFT   = 0,   // CH1 – profilo sinistro logo cassa  (~2.5m, ~80px)
    ZONE_LOGO_RIGHT  = 1,   // CH2 – profilo destro logo cassa    (~2.5m, ~80px)
    ZONE_FRONT_FRAME = 2,   // CH3 – cornice frontale cassa       (~3.0m, ~96px) [4CH only]
    ZONE_SPARE       = 3    // CH4 – canale aggiuntivo opzionale                [4CH only]
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

// Configura il numero di pixel per una zona specifica.
// Utile per adattare la configurazione a lunghezze di strip diverse da quelle di default.
void wled_client_set_zone_config(uint8_t ctrl_idx, WledZone zone, uint16_t pixels);

// ——— Accesso diretto a controller/scene in RAM ———

uint8_t         wled_get_controller_count();
WledController* wled_get_controller(uint8_t idx);

uint8_t         wled_get_scene_count();
WledScene*      wled_get_scene(uint8_t idx);
void            wled_add_scene(const WledScene& scene);
void            wled_remove_scene(uint8_t idx);
