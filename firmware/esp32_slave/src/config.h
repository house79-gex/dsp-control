#pragma once

// ======= CONFIGURAZIONE PIN ESP32-S3 #2 (SLAVE – DMX + Wireless + Storage) =======
// Board: ESP32-S3-WROOM1-N16R8 Dev Board (16MB Flash, 8MB PSRAM, antenna IPEX)
// Architettura: Dual-ESP32 – questo è lo Slave; vedere firmware/esp32/ per il Master

#define BOARD_NAME          "ESP32-S3 Slave DMX+Wireless"
#define FIRMWARE_VERSION    "v2.0.0"

// ——— I2S Slave (RX + opzionale TX verso ES8388 DSDIN) ———
#define I2S_BCLK_IN         5    // ← Master GPIO12
#define I2S_WS_IN           6    // ← Master GPIO13
#define I2S_DIN             7    // ← ES8388 ASDOUT (fan-out con Master GPIO11)
// Se Master USE_SLAVE_ES8388_DOUT=1: questo pin → ES8388 DSDIN (DAC)
#ifndef I2S_DOUT_TO_ES8388_PIN
#define I2S_DOUT_TO_ES8388_PIN  4
#endif
#ifndef SLAVE_DRIVES_ES8388_DOUT
#define SLAVE_DRIVES_ES8388_DOUT  0   // 1 = GPIO4→ES8388 DSDIN + Master USE_SLAVE_ES8388_DOUT=1
#endif

// ——— UART IPC da Master ———
// Riceve comandi da ESP32 #1 (Master) e invia risposte
// IMPORTANTE: TX e RX sono INCROCIATI rispetto al Master
//   Master GPIO17 (TX) → Slave GPIO2 (RX)
//   Master GPIO18 (RX) ← Slave GPIO1 (TX)
#define IPC_UART_NUM        1        // UART1
#define IPC_UART_RX         2        // GPIO2  ← da Master GPIO17 (TX)
#define IPC_UART_TX         1        // GPIO1  → a Master GPIO18 (RX)
#define IPC_UART_BAUD       115200   // Baudrate IPC (deve corrispondere al Master)

// ——— DMX512 Output ———
// UART2; DE sempre HIGH (trasmissione continua)
#define DMX_UART_NUM        2    // UART2
#define DMX_TX              8    // GPIO8  – DMX TX output
#define DMX_DE              9    // GPIO9  – Driver Enable (sempre HIGH)

// ——— Relay / Strobo ———
#define RELAY_GPIO          10
#define STROBE_GPIO         11

// ——— Encoder + LED ring (tutto sullo Slave – Master solo IPC) ———
#ifndef SLAVE_HAS_ENCODERS_LED
#define SLAVE_HAS_ENCODERS_LED  1
#endif
#define ENCODER_VOL_A       15
#define ENCODER_VOL_B       16
#define ENCODER_VOL_BTN     17
#define ENCODER_BAL_A       18
#define ENCODER_BAL_B       21
#define ENCODER_BAL_BTN     22
#define LED_RING_VOL_PIN    23
#define LED_RING_BAL_PIN    24
#define LED_RING_COUNT      12
#define LED_RING_BRIGHTNESS 80

// ——— WiFi / ESP-NOW ———
// Antenna IPEX connector (hardware, nessun GPIO dedicato)
// Potenza TX: 20 dBm max (con antenna esterna 3dBi → range 150-300m)
#define WIFI_TX_POWER_DBM   20   // dBm

// ——— NVS Storage ———
// Questo ESP32 gestisce tutta la persistenza per il sistema (preset, scene, config)
#define NVS_NAMESPACE       "slave_store"
#define NVS_KEY_PRESETS     "dsp_presets"
#define NVS_KEY_SCENES      "dmx_scenes"
#define NVS_KEY_WLED        "wled_cfg"

// ——— Audio FFT ———
#define FFT_SIZE            512    // Punti FFT
#define FFT_NUM_BANDS       6      // Bande audio-reactive

// ——— DMX512 ———
#define DMX_UNIVERSE_SIZE   512    // Canali DMX
#define DMX_UPDATE_RATE_HZ  40     // Frame rate DMX output
