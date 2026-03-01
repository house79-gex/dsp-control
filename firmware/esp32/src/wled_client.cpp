#include "wled_client.h"
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>

// ======= Stato interno =======

static WledController s_controllers[MAX_WLED_CONTROLLERS];
static uint8_t        s_ctrl_count = 0;

static WledScene s_scenes[MAX_WLED_SCENES];
static uint8_t   s_scene_count = 0;

static const uint16_t WLED_UDP_PORT  = 21324;
static const uint32_t HTTP_TIMEOUT_MS = 500;
static const int      HTTP_RETRIES   = 3;

// ======= Helper HTTP ======= 

static bool http_post_json(const char* ip, const char* path,
                            const String& payload) {
    HTTPClient http;
    char url[64];
    snprintf(url, sizeof(url), "http://%s%s", ip, path);

    for (int attempt = 0; attempt < HTTP_RETRIES; attempt++) {
        http.begin(url);
        http.setTimeout(HTTP_TIMEOUT_MS);
        http.addHeader("Content-Type", "application/json");
        int code = http.POST(payload);
        http.end();
        if (code == 200) return true;
        delay(50);
    }
    Serial.printf("[WLED] POST a %s fallito dopo %d tentativi\n", url, HTTP_RETRIES);
    return false;
}

static bool http_get_json(const char* ip, const char* path,
                           String& response) {
    HTTPClient http;
    char url[64];
    snprintf(url, sizeof(url), "http://%s%s", ip, path);

    for (int attempt = 0; attempt < HTTP_RETRIES; attempt++) {
        http.begin(url);
        http.setTimeout(HTTP_TIMEOUT_MS);
        int code = http.GET();
        if (code == 200) {
            response = http.getString();
            http.end();
            return true;
        }
        http.end();
        delay(50);
    }
    Serial.printf("[WLED] GET a %s fallito dopo %d tentativi\n", url, HTTP_RETRIES);
    return false;
}

// ======= Trova l'indice di canale corrispondente a una zona =======

static int find_channel_for_zone(const WledController& ctrl, WledZone zone) {
    for (int ch = 0; ch < ctrl.num_channels && ch < 4; ch++) {
        if (ctrl.zone_map[ch] == zone) return ch;
    }
    return -1;
}

// ======= Costruisce il payload JSON per /json/state =======
// seg è usato per impostare un singolo segmento (canale WLED)

static String build_segment_payload(int seg_id,
                                     uint8_t r, uint8_t g, uint8_t b,
                                     uint8_t brightness,
                                     uint8_t effect_id,
                                     uint8_t speed,
                                     uint8_t intensity) {
    StaticJsonDocument<512> doc;
    doc["bri"] = brightness;
    JsonArray segs = doc.createNestedArray("seg");
    JsonObject seg  = segs.createNestedObject();
    seg["id"] = seg_id;
    JsonArray col = seg.createNestedArray("col");
    JsonArray c1  = col.createNestedArray();
    c1.add(r); c1.add(g); c1.add(b);
    seg["fx"]  = effect_id;
    seg["sx"]  = speed;
    seg["ix"]  = intensity;
    String out;
    serializeJson(doc, out);
    return out;
}

// ======= Task FreeRTOS: health-check ogni 5s =======

static void wled_health_task(void* param) {
    (void)param;
    for (;;) {
        for (uint8_t i = 0; i < s_ctrl_count; i++) {
            String resp;
            bool ok = http_get_json(s_controllers[i].ip, "/json/info", resp);
            s_controllers[i].online = ok;
            if (ok) s_controllers[i].last_seen_ms = millis();
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

// ======= API pubblica =======

void wled_client_init() {
    s_ctrl_count  = 0;
    s_scene_count = 0;
    xTaskCreatePinnedToCore(wled_health_task, "WLED_HB", 4096, nullptr, 2, nullptr, 0);
    Serial.println("[WLED] Client inizializzato");
}

void wled_client_add_controller(const WledController& ctrl) {
    if (s_ctrl_count >= MAX_WLED_CONTROLLERS) return;
    s_controllers[s_ctrl_count++] = ctrl;
    Serial.printf("[WLED] Controller aggiunto: %s (%s)\n", ctrl.name, ctrl.ip);
}

void wled_client_remove_controller(uint8_t index) {
    if (index >= s_ctrl_count) return;
    for (uint8_t i = index; i < s_ctrl_count - 1; i++) {
        s_controllers[i] = s_controllers[i + 1];
    }
    s_ctrl_count--;
}

void wled_client_set_color(uint8_t ctrl_idx, WledZone zone,
                            uint8_t r, uint8_t g, uint8_t b) {
    if (ctrl_idx >= s_ctrl_count) return;
    WledController& ctrl = s_controllers[ctrl_idx];
    int ch = find_channel_for_zone(ctrl, zone);
    if (ch < 0) return;

    String payload = build_segment_payload(ch, r, g, b, 255, 0, 128, 128);
    if (http_post_json(ctrl.ip, "/json/state", payload)) {
        ctrl.last_seen_ms = millis();
        ctrl.online = true;
    }
}

void wled_client_set_brightness(uint8_t ctrl_idx, WledZone zone,
                                 uint8_t brightness) {
    if (ctrl_idx >= s_ctrl_count) return;
    WledController& ctrl = s_controllers[ctrl_idx];
    int ch = find_channel_for_zone(ctrl, zone);
    if (ch < 0) return;

    StaticJsonDocument<128> doc;
    doc["bri"] = brightness;
    JsonArray segs = doc.createNestedArray("seg");
    JsonObject seg  = segs.createNestedObject();
    seg["id"] = ch;
    String payload;
    serializeJson(doc, payload);

    if (http_post_json(ctrl.ip, "/json/state", payload)) {
        ctrl.last_seen_ms = millis();
        ctrl.online = true;
    }
}

void wled_client_set_effect(uint8_t ctrl_idx, WledZone zone,
                             uint8_t effect_id, uint8_t speed, uint8_t intensity) {
    if (ctrl_idx >= s_ctrl_count) return;
    WledController& ctrl = s_controllers[ctrl_idx];
    int ch = find_channel_for_zone(ctrl, zone);
    if (ch < 0) return;

    StaticJsonDocument<256> doc;
    JsonArray segs = doc.createNestedArray("seg");
    JsonObject seg  = segs.createNestedObject();
    seg["id"] = ch;
    seg["fx"] = effect_id;
    seg["sx"] = speed;
    seg["ix"] = intensity;
    String payload;
    serializeJson(doc, payload);

    if (http_post_json(ctrl.ip, "/json/state", payload)) {
        ctrl.last_seen_ms = millis();
        ctrl.online = true;
    }
}

void wled_client_apply_scene(uint8_t scene_idx, bool all_controllers) {
    if (scene_idx >= s_scene_count) return;
    const WledScene& sc = s_scenes[scene_idx];

    uint8_t start = 0;
    uint8_t end   = s_ctrl_count;

    for (uint8_t ci = start; ci < end; ci++) {
        WledController& ctrl = s_controllers[ci];
        for (int ch = 0; ch < ctrl.num_channels && ch < 4; ch++) {
            String payload = build_segment_payload(
                ch, sc.r, sc.g, sc.b, sc.brightness,
                sc.effect_id, sc.speed, sc.intensity);
            if (http_post_json(ctrl.ip, "/json/state", payload)) {
                ctrl.last_seen_ms = millis();
                ctrl.online = true;
            }
        }
        if (!all_controllers) break;
    }
}

void wled_client_sync_all() {
    if (s_ctrl_count == 0) return;
    // Legge lo stato del primo controller e lo replica sugli altri
    String resp;
    if (!http_get_json(s_controllers[0].ip, "/json/state", resp)) return;

    for (uint8_t i = 1; i < s_ctrl_count; i++) {
        http_post_json(s_controllers[i].ip, "/json/state", resp);
    }
}

void wled_client_set_independent(uint8_t ctrl_idx) {
    if (ctrl_idx >= s_ctrl_count) return;
    s_controllers[ctrl_idx].sync_enabled = false;
    // Disabilita la sincronizzazione WLED (udpn send/recv = false)
    const char* payload = "{\"udpn\":{\"send\":false,\"recv\":false}}";
    http_post_json(s_controllers[ctrl_idx].ip, "/json/state",
                   String(payload));
}

void wled_client_mirror_zones(uint8_t ctrl_idx,
                               WledZone zone_a, WledZone zone_b) {
    if (ctrl_idx >= s_ctrl_count) return;
    WledController& ctrl = s_controllers[ctrl_idx];
    int ch_a = find_channel_for_zone(ctrl, zone_a);
    int ch_b = find_channel_for_zone(ctrl, zone_b);
    if (ch_a < 0 || ch_b < 0) return;

    // Legge lo stato del segmento A e lo copia sul segmento B con reverse
    String resp;
    if (!http_get_json(ctrl.ip, "/json/state", resp)) return;

    StaticJsonDocument<1024> doc;
    if (deserializeJson(doc, resp)) return;

    JsonArray segs = doc["seg"].as<JsonArray>();
    for (JsonObject seg : segs) {
        if ((int)seg["id"] == ch_a) {
            seg["id"]  = ch_b;
            seg["rev"] = true;
            break;
        }
    }
    String payload;
    serializeJson(doc, payload);
    http_post_json(ctrl.ip, "/json/state", payload);
}

void wled_client_blackout() {
    const char* payload = "{\"bri\":0}";
    for (uint8_t i = 0; i < s_ctrl_count; i++) {
        http_post_json(s_controllers[i].ip, "/json/state", String(payload));
    }
    Serial.println("[WLED] BLACKOUT inviato a tutti i controller");
}

bool wled_client_get_status(uint8_t ctrl_idx) {
    if (ctrl_idx >= s_ctrl_count) return false;
    WledController& ctrl = s_controllers[ctrl_idx];
    String resp;
    bool ok = http_get_json(ctrl.ip, "/json/info", resp);
    ctrl.online = ok;
    if (ok) ctrl.last_seen_ms = millis();
    return ok;
}

void wled_client_discover() {
    // Scansione UDP broadcast sulla porta WLED (21324)
    // I device WLED rispondono con un pacchetto di notifica
    WiFiUDP udp;
    udp.begin(WLED_UDP_PORT);

    const uint8_t probe[] = { 0x00 };
    udp.beginPacket("255.255.255.255", WLED_UDP_PORT);
    udp.write(probe, sizeof(probe));
    udp.endPacket();

    uint32_t deadline = millis() + 2000;
    while (millis() < deadline) {
        int pktSize = udp.parsePacket();
        if (pktSize > 0) {
            char remoteIp[16];
            strncpy(remoteIp, udp.remoteIP().toString().c_str(), 15);
            remoteIp[15] = '\0';

            // Controlla se già registrato
            bool known = false;
            for (uint8_t i = 0; i < s_ctrl_count; i++) {
                if (strcmp(s_controllers[i].ip, remoteIp) == 0) { known = true; break; }
            }
            if (!known && s_ctrl_count < MAX_WLED_CONTROLLERS) {
                WledController ctrl = {};
                strncpy(ctrl.ip, remoteIp, sizeof(ctrl.ip) - 1);
                snprintf(ctrl.name, sizeof(ctrl.name), "WLED_%d", s_ctrl_count);
                ctrl.num_channels  = 2;
                ctrl.pixels_per_ch[0] = 80;
                ctrl.pixels_per_ch[1] = 80;
                ctrl.zone_map[0] = ZONE_LOGO_LEFT;
                ctrl.zone_map[1] = ZONE_LOGO_RIGHT;
                ctrl.online       = true;
                ctrl.last_seen_ms = millis();
                s_controllers[s_ctrl_count++] = ctrl;
                Serial.printf("[WLED] Scoperto nuovo controller: %s\n", remoteIp);
            }
        }
        delay(10);
    }
    udp.stop();
}

// ======= Accesso diretto in RAM =======

uint8_t wled_get_controller_count()   { return s_ctrl_count; }
WledController* wled_get_controller(uint8_t idx) {
    if (idx >= s_ctrl_count) return nullptr;
    return &s_controllers[idx];
}

uint8_t wled_get_scene_count()        { return s_scene_count; }
WledScene* wled_get_scene(uint8_t idx) {
    if (idx >= s_scene_count) return nullptr;
    return &s_scenes[idx];
}

void wled_add_scene(const WledScene& scene) {
    if (s_scene_count >= MAX_WLED_SCENES) return;
    s_scenes[s_scene_count++] = scene;
}

void wled_remove_scene(uint8_t idx) {
    if (idx >= s_scene_count) return;
    for (uint8_t i = idx; i < s_scene_count - 1; i++) {
        s_scenes[i] = s_scenes[i + 1];
    }
    s_scene_count--;
}
