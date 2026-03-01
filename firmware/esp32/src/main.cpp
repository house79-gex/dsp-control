#include <Arduino.h>
#include <lvgl.h>
#include "config.h"
#include "audio_mode.h"
#include "rs485.h"
#include "storage.h"
#include "dsp_control.h"
#include "dsp_protocol.h"
#include "dmx512.h"
#include "audio_reactive.h"
#include "autotune.h"
#include "usb_storage.h"
#include "web_server.h"
#include "wled_client.h"
#include "ui/ui_home.h"
#include "ui/ui_discovery.h"
#include "ui/ui_assignment.h"
#include "ui/ui_dmx.h"
#include "ui/ui_dsp_advanced.h"
#include "ui/ui_wled.h"
#include "led_ring.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Lista assegnazioni globale (condivisa con web_server.cpp)
std::vector<SpeakerAssignment> g_assignments;

// ======= LVGL =======

static lv_disp_draw_buf_t s_drawBuf;
static lv_color_t* s_buf1 = nullptr;
static lv_color_t* s_buf2 = nullptr;

// Stub flush callback – da sostituire con driver SPI reale
static void disp_flush_cb(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_p) {
    lv_disp_flush_ready(drv);
}

// Stub touch read callback
static void touch_read_cb(lv_indev_drv_t* drv, lv_indev_data_t* data) {
    data->state = LV_INDEV_STATE_REL;
}

static void lvgl_init() {
    lv_init();

    const size_t bufSize = 800 * 20;
    s_buf1 = (lv_color_t*)heap_caps_malloc(bufSize * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    s_buf2 = (lv_color_t*)heap_caps_malloc(bufSize * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!s_buf1) s_buf1 = (lv_color_t*)malloc(bufSize * sizeof(lv_color_t));
    if (!s_buf2) s_buf2 = (lv_color_t*)malloc(bufSize * sizeof(lv_color_t));

    lv_disp_draw_buf_init(&s_drawBuf, s_buf1, s_buf2, bufSize);

    static lv_disp_drv_t dispDrv;
    lv_disp_drv_init(&dispDrv);
    dispDrv.hor_res  = 800;
    dispDrv.ver_res  = 480;
    dispDrv.flush_cb = disp_flush_cb;
    dispDrv.draw_buf = &s_drawBuf;
    lv_disp_drv_register(&dispDrv);

    static lv_indev_drv_t indevDrv;
    lv_indev_drv_init(&indevDrv);
    indevDrv.type    = LV_INDEV_TYPE_POINTER;
    indevDrv.read_cb = touch_read_cb;
    lv_indev_drv_register(&indevDrv);

    Serial.println("[LVGL] Inizializzato");
}

// ======= UI con tab (Audio + Discovery + Assegnazione) =======
static void ui_create_tabview() {
    lv_obj_t* tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 40);

    lv_obj_t* tabHome       = lv_tabview_add_tab(tabview, LV_SYMBOL_AUDIO   " Home");
    lv_obj_t* tabDiscovery  = lv_tabview_add_tab(tabview, LV_SYMBOL_REFRESH " Discovery");
    lv_obj_t* tabAssignment = lv_tabview_add_tab(tabview, LV_SYMBOL_EDIT    " Assegna");

    ui_home_create(tabHome);
    ui_discovery_create(tabDiscovery);
    ui_assignment_create(tabAssignment, 0);

    lv_obj_t* tabDmx = lv_tabview_add_tab(tabview, LV_SYMBOL_EYE_OPEN " Luci");
    lv_obj_t* tabDsp = lv_tabview_add_tab(tabview, LV_SYMBOL_SETTINGS " DSP");
    lv_obj_t* tabNeon = lv_tabview_add_tab(tabview, LV_SYMBOL_LED " Neon");

    ui_dmx_create(tabDmx);
    ui_dsp_advanced_create(tabDsp);
    ui_wled_create(tabNeon);
}

// ======= Task FreeRTOS DSP heartbeat (su Core 1) =======
static void dsp_heartbeat_task(void* param) {
    Serial.println("[DSP] Task heartbeat avviato");
    for (;;) {
        dsp_heartbeat_tick();
        // Processa byte RS-485 in arrivo per il protocollo DSP
        uint8_t rxBuf[64];
        size_t rxLen = 0;
        if (rs485_receive(rxBuf, sizeof(rxBuf), rxLen, 1)) {
            for (size_t i = 0; i < rxLen; i++) {
                dsp_rx_process_byte(rxBuf[i]);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// ======= Task FreeRTOS autotune tick (su Core 0) =======
static void autotune_task(void* param) {
    for (;;) {
        autotune_tick();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// ======= Demo all'avvio =======
static void demo_startup() {
    if (!g_assignments.empty()) {
        Serial.println("[MAIN] Demo: beep benvenuto al primo dispositivo");
        rs485_send_beep(g_assignments[0].deviceId, BeepPattern::Single);
        delay(500);
    }
    Serial.println("[MAIN] Demo: applicazione preset 2WAY_STD");
    dsp_apply_preset("2WAY_STD");
    Serial.println("[MAIN] Demo: modalità audio → MixerPassThrough");
    setAudioMode(AudioMode::MixerPassThrough);
}

// ======= Setup =======
void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n[MAIN] ===== Sistema Audio PA v2.0 – avvio =====");

    pinMode(LED_STATUS, OUTPUT);
    digitalWrite(LED_STATUS, HIGH);

    // Inizializzazione moduli core
    rs485_init();
    audio_init();
    storage_init();
    dsp_init();

    // Inizializzazione DMX512
    dmx_init();

    // Inizializzazione audio-reactive
    audio_reactive_init();

    // Inizializzazione autotune
    autotune_init();

    // Inizializzazione USB storage
    usb_storage_init();

    // Inizializzazione WLED client
    wled_client_init();

    // Inizializzazione LED ring e encoder
    led_ring_init();
    encoder_init();

    // Carica assegnazioni salvate
    g_assignments = storage_load_assignments();
    Serial.printf("[MAIN] %d assegnazioni caricate da NVS\n", (int)g_assignments.size());

    // Carica configurazione sistema
    SystemConfig sysCfg = storage_load_system_config();
    led_ring_set_volume(sysCfg.masterVolume > 0 ? sysCfg.masterVolume : 80);

    // Carica controller e scene WLED da NVS
    storage_load_wled_controllers();
    storage_load_wled_scenes();

    // UI display
    lvgl_init();
    ui_create_tabview();

    // Web server (WiFi AP + REST API)
    web_server_init();

    // ——— Task FreeRTOS ———
    // Task DMX output su Core 1 (priorità alta per timing preciso)
    xTaskCreatePinnedToCore(dmx_task,           "DMX",       4096, nullptr, 10, nullptr, 1);
    // Task FFT audio-reactive su Core 0
    xTaskCreatePinnedToCore(audio_fft_task,     "FFT",       8192, nullptr,  5, nullptr, 0);
    // Task heartbeat DSP su Core 1
    xTaskCreatePinnedToCore(dsp_heartbeat_task, "DSP_HB",    2048, nullptr,  3, nullptr, 1);
    // Task autotune su Core 0
    xTaskCreatePinnedToCore(autotune_task,      "AUTOTUNE",  4096, nullptr,  2, nullptr, 0);

    demo_startup();

    digitalWrite(LED_STATUS, LOW);
    Serial.println("[MAIN] Pronto – Sistema Audio PA v2.0");
    Serial.printf("[MAIN] IP Access Point: %s\n", WiFi.softAPIP().toString().c_str());
}

// ======= Loop principale =======
void loop() {
    lv_timer_handler();      // Task LVGL (~5ms)
    web_server_handle();     // No-op per ESPAsyncWebServer
    usb_storage_tick();      // Gestione eventi USB
    encoder_tick();
    led_ring_update();

    // Genera tono di test se in modalità TestTone
    if (getAudioMode() == AudioMode::TestTone) {
        audio_generate_test_tone(1000.0f, 0.5f);
    } else {
        // ADC processing (FFT audio-reactive)
        audio_fft_process();
    }

    delay(5);
}
