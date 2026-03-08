#include "config.h"
#include "ipc_slave.h"
#include "audio_slave.h"
#include "dmx512_slave.h"
#include "wireless_slave.h"
#include "storage_slave.h"
#include "relay_control.h"

// Task FreeRTOS: elaborazione audio + FFT su Core 0
void audio_task(void* pvParameters) {
    while (true) {
        audio_slave_process_fft();
        // Aggiorna strobo audio-reactive
        const float* bins = audio_slave_get_fft_bins();
        strobe_trigger_from_bass(bins[0]);  // Banda 0 = Bass
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// Task FreeRTOS: invio wireless audio frame su Core 0
void wireless_task(void* pvParameters) {
    const TickType_t period = pdMS_TO_TICKS(25);  // ~40 Hz
    TickType_t lastWake = xTaskGetTickCount();

    while (true) {
        WirelessAudioFrame frame;
        const float* bins = audio_slave_get_fft_bins();
        memcpy(frame.fftBins, bins, sizeof(frame.fftBins));
        frame.peakDb  = audio_slave_get_peak_db();
        frame.dmxFps  = (uint8_t)dmx_slave_get_fps();
        wireless_slave_send_frame(frame);
        vTaskDelayUntil(&lastWake, period);
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.printf("\n\n[SLAVE] %s %s avvio\n", BOARD_NAME, FIRMWARE_VERSION);

    // Inizializzazioni
    storage_slave_init();
    ipc_slave_init();
    relay_control_init();
    dmx_slave_init();
    audio_slave_init();
    wireless_slave_init();

    // Task DMX su Core 1 (priorità alta per timing preciso)
    xTaskCreatePinnedToCore(dmx_slave_task, "dmx_task", 4096, nullptr, 5, nullptr, 1);

    // Task audio FFT su Core 0
    xTaskCreatePinnedToCore(audio_task, "audio_task", 8192, nullptr, 4, nullptr, 0);

    // Task wireless su Core 0
    xTaskCreatePinnedToCore(wireless_task, "wifi_task", 4096, nullptr, 3, nullptr, 0);

    Serial.println("[SLAVE] Setup completato – loop IPC attivo");
}

void loop() {
    // Loop principale: elabora comandi IPC dal Master
    ipc_slave_process();
    delay(1);
}
