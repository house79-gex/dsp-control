#include "ipc_master.h"
#include "config.h"
#include <HardwareSerial.h>

// Istanza UART IPC (UART1)
static HardwareSerial s_ipcSerial(IPC_UART_NUM);

// Buffer risposta in arrivo
static uint8_t  s_rxBuf[sizeof(IPCFrame)];
static uint8_t  s_rxIdx = 0;
static bool     s_awaitingResponse = false;
static IPCFrame s_lastResponse = {};

// ===== CRC8 (polynomial 0x07, init 0x00) =====
static uint8_t crc8_compute(const uint8_t* data, size_t len) {
    uint8_t crc = 0x00;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

void ipc_master_init() {
    s_ipcSerial.begin(IPC_UART_BAUD, SERIAL_8N1, IPC_UART_RX, IPC_UART_TX);
    Serial.printf("[IPC_MASTER] UART%d inizializzato @ %d baud (TX=GPIO%d, RX=GPIO%d)\n",
                  IPC_UART_NUM, IPC_UART_BAUD, IPC_UART_TX, IPC_UART_RX);
}

bool ipc_master_send_command(IPCCommand cmd, const uint8_t* payload, uint8_t len) {
    if (len > IPC_MAX_PAYLOAD) {
        Serial.printf("[IPC_MASTER] Payload troppo grande: %d > %d\n", len, IPC_MAX_PAYLOAD);
        return false;
    }

    IPCFrame frame;
    frame.sync1       = IPC_SYNC_BYTE_1;
    frame.sync2       = IPC_SYNC_BYTE_2;
    frame.command     = static_cast<uint8_t>(cmd);
    frame.payload_len = len;
    if (len > 0 && payload != nullptr) {
        memcpy(frame.payload, payload, len);
    }

    // CRC8 su command + payload_len + payload
    uint8_t crc_data[2 + IPC_MAX_PAYLOAD];
    crc_data[0] = frame.command;
    crc_data[1] = frame.payload_len;
    if (len > 0) memcpy(&crc_data[2], frame.payload, len);
    frame.crc8 = crc8_compute(crc_data, 2 + len);

    // Scrivi frame su UART
    size_t frameLen = 4 + len + 1;  // sync1+sync2+cmd+len + payload + crc
    uint8_t rawBuf[4 + IPC_MAX_PAYLOAD + 1];
    rawBuf[0] = frame.sync1;
    rawBuf[1] = frame.sync2;
    rawBuf[2] = frame.command;
    rawBuf[3] = frame.payload_len;
    if (len > 0) memcpy(&rawBuf[4], frame.payload, len);
    rawBuf[4 + len] = frame.crc8;

    s_ipcSerial.write(rawBuf, frameLen);
    s_ipcSerial.flush();

    return true;
}

bool ipc_master_wait_ack(uint32_t timeoutMs) {
    uint32_t start = millis();
    uint8_t rxBuf[4 + IPC_MAX_PAYLOAD + 1];
    uint8_t rxIdx = 0;

    while ((millis() - start) < timeoutMs) {
        if (s_ipcSerial.available()) {
            uint8_t b = s_ipcSerial.read();
            if (rxIdx == 0 && b != IPC_SYNC_BYTE_1) continue;
            if (rxIdx == 1 && b != IPC_SYNC_BYTE_2) { rxIdx = 0; continue; }
            rxBuf[rxIdx++] = b;

            // Hai ricevuto sync1 + sync2 + cmd + len?
            if (rxIdx >= 4) {
                uint8_t payLen = rxBuf[3];
                if (payLen > IPC_MAX_PAYLOAD) {
                    // Frame non valido, reset
                    rxIdx = 0;
                    continue;
                }
                // Aspetta payload + crc
                size_t totalLen = 4 + payLen + 1;
                if (rxIdx >= totalLen) {
                    // Verifica CRC
                    uint8_t crc_data[2 + IPC_MAX_PAYLOAD];
                    crc_data[0] = rxBuf[2];
                    crc_data[1] = rxBuf[3];
                    if (payLen > 0) memcpy(&crc_data[2], &rxBuf[4], payLen);
                    uint8_t calcCrc = crc8_compute(crc_data, 2 + payLen);

                    if (calcCrc != rxBuf[4 + payLen]) {
                        Serial.println("[IPC_MASTER] CRC error in risposta");
                        return false;
                    }

                    IPCResponse resp = static_cast<IPCResponse>(rxBuf[2]);
                    if (resp == IPCResponse::RESP_ACK || resp == IPCResponse::RESP_PONG) {
                        // Salva l'ultima risposta (payload incluso)
                        s_lastResponse.sync1       = rxBuf[0];
                        s_lastResponse.sync2       = rxBuf[1];
                        s_lastResponse.command     = rxBuf[2];
                        s_lastResponse.payload_len = rxBuf[3];
                        if (payLen > 0) memcpy(s_lastResponse.payload, &rxBuf[4], payLen);
                        s_lastResponse.crc8        = rxBuf[4 + payLen];
                        return true;
                    } else if (resp == IPCResponse::RESP_STORAGE_DATA) {
                        // Salva risposta con payload (per storage load)
                        s_lastResponse.sync1       = rxBuf[0];
                        s_lastResponse.sync2       = rxBuf[1];
                        s_lastResponse.command     = rxBuf[2];
                        s_lastResponse.payload_len = rxBuf[3];
                        if (payLen > 0) memcpy(s_lastResponse.payload, &rxBuf[4], payLen);
                        s_lastResponse.crc8        = rxBuf[4 + payLen];
                        return true;
                    } else {
                        // RESP_NACK o sconosciuto
                        Serial.printf("[IPC_MASTER] Risposta NACK o sconosciuta: 0x%02X\n", rxBuf[2]);
                        return false;
                    }
                }
            }
        }
        delay(1);
    }

    Serial.printf("[IPC_MASTER] Timeout attesa ACK (%u ms)\n", timeoutMs);
    return false;
}

bool ipc_master_ping(uint32_t timeoutMs) {
    bool ok = ipc_master_send_command(IPCCommand::CMD_PING, nullptr, 0);
    if (!ok) return false;
    return ipc_master_wait_ack(timeoutMs);
}

bool ipc_storage_save_preset(uint8_t slot, const void* data, size_t len) {
    if (len > IPC_MAX_PAYLOAD - 1) {
        Serial.printf("[IPC_MASTER] Preset troppo grande per IPC: %zu > %d\n", len, IPC_MAX_PAYLOAD - 1);
        return false;
    }

    // Payload: [slot(1)] + [data(len)]
    uint8_t payload[IPC_MAX_PAYLOAD];
    payload[0] = slot;
    memcpy(&payload[1], data, len);

    const int maxRetries = 3;
    for (int attempt = 0; attempt < maxRetries; attempt++) {
        if (ipc_master_send_command(IPCCommand::CMD_STORAGE_SAVE_PRESET, payload, (uint8_t)(1 + len))) {
            if (ipc_master_wait_ack(100)) {
                return true;
            }
        }
        Serial.printf("[IPC_MASTER] Retry %d/3 salvataggio preset slot %d\n", attempt + 1, slot);
        delay(10);
    }
    return false;
}

bool ipc_storage_load_preset(uint8_t slot, void* data, size_t maxLen, size_t& outLen) {
    uint8_t payload[1] = { slot };

    const int maxRetries = 3;
    for (int attempt = 0; attempt < maxRetries; attempt++) {
        if (ipc_master_send_command(IPCCommand::CMD_STORAGE_LOAD_PRESET, payload, 1)) {
            if (ipc_master_wait_ack(150)) {
                // La risposta è in s_lastResponse
                if (s_lastResponse.command == static_cast<uint8_t>(IPCResponse::RESP_STORAGE_DATA)) {
                    outLen = s_lastResponse.payload_len;
                    if (outLen > maxLen) outLen = maxLen;
                    memcpy(data, s_lastResponse.payload, outLen);
                    return true;
                } else {
                    return true;  // ACK senza dati
                }
            }
        }
        Serial.printf("[IPC_MASTER] Retry %d/3 caricamento preset slot %d\n", attempt + 1, slot);
        delay(10);
    }
    return false;
}

bool ipc_scene_change(uint8_t sceneIndex) {
    uint8_t payload[1] = { sceneIndex };
    if (!ipc_master_send_command(IPCCommand::CMD_SCENE_CHANGE, payload, 1)) {
        return false;
    }
    return ipc_master_wait_ack(100);
}
