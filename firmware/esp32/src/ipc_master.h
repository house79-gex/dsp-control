#pragma once
#include <Arduino.h>
#include <vector>

// ===== Protocollo UART IPC =====
#define IPC_SYNC_BYTE_1     0xAA
#define IPC_SYNC_BYTE_2     0x55
#define IPC_MAX_PAYLOAD     128

// Comandi Master → Slave
enum class IPCCommand : uint8_t {
    CMD_SCENE_CHANGE        = 0x01,  // Cambia scena DMX
    CMD_DMX_RAW             = 0x02,  // Invia valori DMX raw
    CMD_FFT_BINS            = 0x03,  // Invia dati FFT al Slave
    CMD_STORAGE_SAVE_PRESET = 0x10,  // Salva preset DSP
    CMD_STORAGE_LOAD_PRESET = 0x11,  // Carica preset DSP
    CMD_STORAGE_SAVE_SCENE  = 0x12,  // Salva scena DMX
    CMD_STORAGE_LOAD_SCENE  = 0x13,  // Carica scena DMX
    CMD_PING                = 0xF0   // Heartbeat
};

// Risposte Slave → Master
enum class IPCResponse : uint8_t {
    RESP_ACK                = 0x80,
    RESP_NACK               = 0x81,
    RESP_STATUS_DMX         = 0x82,
    RESP_STATUS_WIRELESS    = 0x83,
    RESP_STORAGE_DATA       = 0x84,
    RESP_PONG               = 0xF0
};

#pragma pack(push, 1)
struct IPCFrame {
    uint8_t sync1;              // IPC_SYNC_BYTE_1 (0xAA)
    uint8_t sync2;              // IPC_SYNC_BYTE_2 (0x55)
    uint8_t command;            // IPCCommand o IPCResponse
    uint8_t payload_len;        // 0-128
    uint8_t payload[IPC_MAX_PAYLOAD];
    uint8_t crc8;               // CRC8 su command + payload_len + payload
};
#pragma pack(pop)

// ===== API Master =====
void ipc_master_init();
bool ipc_master_send_command(IPCCommand cmd, const uint8_t* payload, uint8_t len);
bool ipc_master_wait_ack(uint32_t timeoutMs = 100);
bool ipc_master_ping(uint32_t timeoutMs = 200);

// Storage delegation verso ESP32 #2
bool ipc_storage_save_preset(uint8_t slot, const void* data, size_t len);
bool ipc_storage_load_preset(uint8_t slot, void* data, size_t maxLen, size_t& outLen);

// DMX scene change
bool ipc_scene_change(uint8_t sceneIndex);
