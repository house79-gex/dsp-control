#pragma once

// ======= CONFIGURAZIONE PIN ESP32-S3 #2 (SLAVE – DMX + Wireless + Storage) =======
// Board: ESP32-S3-WROOM1-N16R8 Dev Board (16MB Flash, 8MB PSRAM, antenna IPEX)
// Architettura: Dual-ESP32 – questo è lo Slave; vedere firmware/esp32/ per il Master

#define BOARD_NAME          "ESP32-S3 Slave DMX+Wireless"
#define FIRMWARE_VERSION    "v2.0.0"

// ——— I2S Slave RX (riceve audio da Master ESP32 #1) ———
// Clock generati dal Master (I2S Master); questo ESP32 è I2S Slave RX-only
#define I2S_BCLK_IN         5    // GPIO5  ← da Master GPIO12 (BCLK)
#define I2S_WS_IN           6    // GPIO6  ← da Master GPIO13 (WS)
#define I2S_DIN             7    // GPIO7  ← da ES8388 DOUT (fan-out, parallelo con Master)

// ——— UART IPC da Master ———
// Riceve comandi da ESP32 #1 (Master) e invia risposte
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
#define RELAY_GPIO          10   // GPIO10 – Relay generale
#define STROBE_GPIO         11   // GPIO11 – Strobo

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
