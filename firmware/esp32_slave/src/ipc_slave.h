#pragma once
#include <Arduino.h>

// ===== Protocollo UART IPC (identico a ipc_master.h sul Master) =====
// Questo file deve rimanere sincronizzato con firmware/esp32/src/ipc_master.h

#define IPC_SYNC_BYTE_1     0xAA
#define IPC_SYNC_BYTE_2     0x55
#define IPC_MAX_PAYLOAD     128

// Comandi Master → Slave
enum class IPCCommand : uint8_t {
    CMD_SCENE_CHANGE        = 0x01,  // Cambia scena DMX
    CMD_DMX_RAW             = 0x02,  // Invia valori DMX raw
    CMD_FFT_BINS            = 0x03,  // (Opzionale) FFT dal Master
    CMD_STORAGE_SAVE_PRESET = 0x10,  // Salva preset DSP
    CMD_STORAGE_LOAD_PRESET = 0x11,  // Carica preset DSP
    CMD_STORAGE_SAVE_SCENE  = 0x12,  // Salva scena DMX
    CMD_STORAGE_LOAD_SCENE  = 0x13,  // Carica scena DMX
    CMD_PING                = 0xF0,
    CMD_POLL_INPUTS         = 0x20,
    CMD_SET_LED_RING        = 0x21,
    CMD_RELAY_SET           = 0x22
};

// Risposte Slave → Master
enum class IPCResponse : uint8_t {
    RESP_ACK                = 0x80,
    RESP_NACK               = 0x81,
    RESP_STATUS_DMX         = 0x82,
    RESP_STATUS_WIRELESS    = 0x83,
    RESP_STORAGE_DATA       = 0x84,
    RESP_PONG               = 0xF0,
    RESP_INPUT_STATE        = 0x85
};

#pragma pack(push, 1)
struct IPCFrame {
    uint8_t sync1;
    uint8_t sync2;
    uint8_t command;
    uint8_t payload_len;
    uint8_t payload[IPC_MAX_PAYLOAD];
    uint8_t crc8;
};
#pragma pack(pop)

// ===== API Slave =====
void ipc_slave_init();
void ipc_slave_process();
void ipc_slave_send_response(IPCResponse resp, const uint8_t* data, uint8_t len);
