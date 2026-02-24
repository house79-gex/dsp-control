#include <Arduino.h>
#include <lvgl.h>
#include "config.h"
#include "audio_mode.h"
#include "rs485.h"
#include "storage.h"
#include "dsp_control.h"
#include "web_server.h"
#include "ui/ui_home.h"
#include "ui/ui_discovery.h"
#include "ui/ui_assignment.h"

// Lista assegnazioni globale (condivisa con web_server.cpp)
std::vector<SpeakerAssignment> g_assignments;

// ======= LVGL =======
// TODO: collegare driver reale display SPI e touch I2C

// Buffer LVGL allocati in PSRAM (evita di esaurire DRAM su ESP32-S3)
static lv_disp_draw_buf_t s_drawBuf;
static lv_color_t* s_buf1 = nullptr;
static lv_color_t* s_buf2 = nullptr;

// Stub flush callback – da sostituire con driver SPI reale
static void disp_flush_cb(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_p) {
    // TODO: inviare i pixel via SPI al controller display (es. ILI9488, ST7796)
    lv_disp_flush_ready(drv);
}

// Stub touch read callback – da sostituire con driver I2C reale (es. GT911, FT6236)
static void touch_read_cb(lv_indev_drv_t* drv, lv_indev_data_t* data) {
    // TODO: leggere coordinata touch dal chip I2C
    data->state = LV_INDEV_STATE_REL;
}

static void lvgl_init() {
    lv_init();

    // Alloca buffer in PSRAM se disponibile, altrimenti in DRAM
    const size_t bufSize = 800 * 20;
    s_buf1 = (lv_color_t*)heap_caps_malloc(bufSize * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    s_buf2 = (lv_color_t*)heap_caps_malloc(bufSize * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!s_buf1 || !s_buf2) {
        // Fallback su DRAM se PSRAM non disponibile
        if (!s_buf1) s_buf1 = (lv_color_t*)malloc(bufSize * sizeof(lv_color_t));
        if (!s_buf2) s_buf2 = (lv_color_t*)malloc(bufSize * sizeof(lv_color_t));
    }

    lv_disp_draw_buf_init(&s_drawBuf, s_buf1, s_buf2, bufSize);

    static lv_disp_drv_t dispDrv;
    lv_disp_drv_init(&dispDrv);
    dispDrv.hor_res    = 800;
    dispDrv.ver_res    = 480;
    dispDrv.flush_cb   = disp_flush_cb;
    dispDrv.draw_buf   = &s_drawBuf;
    lv_disp_drv_register(&dispDrv);

    static lv_indev_drv_t indevDrv;
    lv_indev_drv_init(&indevDrv);
    indevDrv.type     = LV_INDEV_TYPE_POINTER;
    indevDrv.read_cb  = touch_read_cb;
    lv_indev_drv_register(&indevDrv);

    Serial.println("[LVGL] Inizializzato");
}

// ======= Creazione UI con tab =======
static void ui_create_tabview() {
    lv_obj_t* tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 40);

    lv_obj_t* tabHome       = lv_tabview_add_tab(tabview, LV_SYMBOL_AUDIO   " Home");
    lv_obj_t* tabDiscovery  = lv_tabview_add_tab(tabview, LV_SYMBOL_REFRESH " Discovery");
    lv_obj_t* tabAssignment = lv_tabview_add_tab(tabview, LV_SYMBOL_EDIT    " Assegna");

    ui_home_create(tabHome);
    ui_discovery_create(tabDiscovery);
    ui_assignment_create(tabAssignment, 0);
}

// ======= Sequenza demo all'avvio =======
static void demo_startup() {
    if (!g_assignments.empty()) {
        Serial.println("[MAIN] Demo: invio beep di benvenuto al primo dispositivo");
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
    Serial.println("\n[MAIN] ===== Sistema Audio PA – avvio =====");

    pinMode(LED_STATUS, OUTPUT);
    digitalWrite(LED_STATUS, HIGH);

    rs485_init();
    audio_init();
    storage_init();
    dsp_init();

    // Carica assegnazioni salvate
    g_assignments = storage_load_assignments();
    Serial.printf("[MAIN] %d assegnazioni caricate da NVS\n", (int)g_assignments.size());

    lvgl_init();
    ui_create_tabview();

    web_server_init();

    demo_startup();

    digitalWrite(LED_STATUS, LOW);
    Serial.println("[MAIN] Pronto");
}

// ======= Loop =======
void loop() {
    lv_timer_handler();     // task LVGL (~5 ms)
    web_server_handle();    // no-op per ESPAsyncWebServer

    // Genera tono di test in continuo se in modalità TestTone
    if (getAudioMode() == AudioMode::TestTone) {
        audio_generate_test_tone(1000.0f, 0.5f);
    } else {
        audio_fft_process_stub();  // mantieni ADC attivo per futura analisi
    }

    delay(5);
}
