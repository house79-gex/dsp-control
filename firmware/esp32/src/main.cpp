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
#include "encoder.h"
#include "wireless_tx.h"
#include "drivers/display_driver.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Lista assegnazioni globale (condivisa con web_server.cpp)
std::vector<SpeakerAssignment> g_assignments;

// ======= LVGL =======

static lv_disp_draw_buf_t s_drawBuf;
static lv_color_t* s_buf1 = nullptr;
static lv_color_t* s_buf2 = nullptr;

static void lvgl_init() {
    display_init();
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
    dispDrv.flush_cb = display_flush;
    dispDrv.draw_buf = &s_drawBuf;
    lv_disp_drv_register(&dispDrv);

    static lv_indev_drv_t indevDrv;
    lv_indev_drv_init(&indevDrv);
    indevDrv.type    = LV_INDEV_TYPE_POINTER;
    indevDrv.read_cb = touch_read;
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

    // Storage + IPC prima di audio/encoder (periferiche Slave)
    storage_init();
    rs485_init();
    audio_init();
    dsp_init();

    // Inizializzazione DMX512
    dmx_init();

    // Carica fixture DMX da NVS; se nessuna presente carica quelle di fabbrica
    {
        std::vector<DmxFixture> savedFixtures = storage_load_dmx_fixtures();
        if (savedFixtures.empty()) {
            Serial.println("[MAIN] Nessuna fixture DMX salvata → carico fixture di fabbrica");
            dmx_load_factory_fixtures();
        } else {
            for (const auto& fx : savedFixtures) dmx_add_fixture(fx);
            Serial.printf("[MAIN] %d fixture DMX caricate da NVS\n", (int)savedFixtures.size());
        }
    }

    // Inizializzazione audio-reactive
    audio_reactive_init();

    // Inizializzazione autotune
    autotune_init();

    // Inizializzazione USB storage
    usb_storage_init();

    // Inizializzazione WLED client
    wled_client_init();

    // Inizializzazione LED ring e encoder (encoder_init chiama led_ring_init internamente)
    encoder_init();

    // Registra callback encoder volume
    encoder_vol_on_change([](int8_t delta) {
        Serial.printf("[ENCODER] Volume: %d%% (delta %d)\n", encoder_get_volume(), delta);
    });
    encoder_vol_on_button([](bool pressed) {
        if (pressed) {
            Serial.println("[ENCODER] Volume button pressed!");
            led_ring_effect_pulse(255, 0, 0);  // Pulse rosso
        }
    });

    // Registra callback encoder balance
    encoder_bal_on_change([](int8_t delta) {
        Serial.printf("[ENCODER] Balance: %d (delta %d)\n", encoder_get_balance(), delta);
    });
    encoder_bal_on_button([](bool pressed) {
        if (pressed) {
            Serial.println("[ENCODER] Balance button pressed! Reset a centro");
            encoder_set_balance(0);
            led_ring_effect_pulse(0, 255, 255);  // Pulse ciano
        }
    });

    // Carica assegnazioni salvate
    g_assignments = storage_load_assignments();
    Serial.printf("[MAIN] %d assegnazioni caricate da NVS\n", (int)g_assignments.size());

    // Carica configurazione audio da NVS (default SC LIVE 4: -14 dB, 44100 Hz, 24 bit)
    float    inputGainDb = -14.0f;
    uint32_t sampleRate  = AUDIO_SAMPLE_RATE;
    uint8_t  bitDepth    = AUDIO_BIT_DEPTH;
    if (storage_load_audio_config(&inputGainDb, &sampleRate, &bitDepth)) {
        Serial.println("[MAIN] Audio config caricata da NVS");
    } else {
        Serial.println("[MAIN] Audio config default SC LIVE 4 (-14 dB, 44100 Hz, 24 bit)");
        storage_save_audio_config(inputGainDb, sampleRate, bitDepth);
    }
    audio_set_input_gain(inputGainDb);

    // Carica configurazione sistema (sync volume encoder con valore salvato)
    SystemConfig sysCfg = storage_load_system_config();
    encoder_set_volume(sysCfg.masterVolume > 0 ? sysCfg.masterVolume : 80);

    // Carica controller e scene WLED da NVS
    storage_load_wled_controllers();
    storage_load_wled_scenes();

    // UI display
    lvgl_init();
    ui_create_tabview();

    // Web server (WiFi AP + REST API)
    web_server_init();

    // Inizializzazione sistema wireless TX (dopo web_server per WiFi già attivo)
    wireless_tx_init();
    {
        WirelessConfig wCfg;
        storage_load_wireless_config(wCfg);
        wireless_tx_load_config(wCfg);
    }

    // ——— Task FreeRTOS ———
    // Task DMX output su Core 1 (priorità alta per timing preciso)
    xTaskCreatePinnedToCore(dmx_task,           "DMX",       4096, nullptr, 10, nullptr, 1);
    // Task FFT audio-reactive su Core 0
    xTaskCreatePinnedToCore(audio_fft_task,     "FFT",       8192, nullptr,  5, nullptr, 0);
    // Task heartbeat DSP su Core 1
    xTaskCreatePinnedToCore(dsp_heartbeat_task, "DSP_HB",    2048, nullptr,  3, nullptr, 1);
    // Task autotune su Core 0
    xTaskCreatePinnedToCore(autotune_task,           "AUTOTUNE",  4096, nullptr,  2, nullptr, 0);
    // Task wireless audio TX su Core 1 (alta priorità per latenza 1ms)
    xTaskCreatePinnedToCore(wireless_audio_tx_task,  "WIFI_TX",   4096, nullptr,  8, nullptr, 1);

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
    encoder_tick();          // Processa encoder delta, callback e LED ring update

    // Genera tono di test se in modalità TestTone
    if (getAudioMode() == AudioMode::TestTone) {
        audio_generate_test_tone(1000.0f, 0.5f);
    } else {
        // ADC processing (FFT audio-reactive)
        audio_fft_process();
    }

    delay(5);
}
