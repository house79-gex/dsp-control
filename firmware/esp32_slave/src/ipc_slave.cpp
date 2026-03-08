#include "ipc_slave.h"
#include "config.h"
#include "storage_slave.h"
#include "dmx512_slave.h"

// CRC8 (polynomial 0x07, init 0x00) – identico all'implementazione Master
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

void ipc_slave_init() {
    Serial1.begin(IPC_UART_BAUD, SERIAL_8N1, IPC_UART_RX, IPC_UART_TX);
    Serial.printf("[IPC_SLAVE] UART%d inizializzato @ %d baud (RX=GPIO%d, TX=GPIO%d)\n",
                  IPC_UART_NUM, IPC_UART_BAUD, IPC_UART_RX, IPC_UART_TX);
}

void ipc_slave_process() {
    // Accumula byte in arrivo cercando sync + frame completo
    static uint8_t rxBuf[4 + IPC_MAX_PAYLOAD + 1];
    static uint8_t rxIdx = 0;

    while (Serial1.available()) {
        uint8_t b = Serial1.read();

        // Attesa sync byte 1
        if (rxIdx == 0) {
            if (b != IPC_SYNC_BYTE_1) continue;
        }
        // Attesa sync byte 2
        else if (rxIdx == 1) {
            if (b != IPC_SYNC_BYTE_2) {
                rxIdx = 0;
                continue;
            }
        }

        rxBuf[rxIdx++] = b;

        // Abbiamo sync1 + sync2 + cmd + len?
        if (rxIdx >= 4) {
            uint8_t payLen = rxBuf[3];
            if (payLen > IPC_MAX_PAYLOAD) {
                // Frame non valido, reset
                Serial.printf("[IPC_SLAVE] payload_len invalido: %d\n", payLen);
                rxIdx = 0;
                continue;
            }

            size_t totalLen = 4 + payLen + 1;
            if (rxIdx >= totalLen) {
                // Frame completo – verifica CRC
                uint8_t crc_data[2 + IPC_MAX_PAYLOAD];
                crc_data[0] = rxBuf[2];  // command
                crc_data[1] = rxBuf[3];  // payload_len
                if (payLen > 0) memcpy(&crc_data[2], &rxBuf[4], payLen);
                uint8_t calcCrc = crc8_compute(crc_data, 2 + payLen);

                if (calcCrc != rxBuf[4 + payLen]) {
                    Serial.println("[IPC_SLAVE] CRC error");
                    ipc_slave_send_response(IPCResponse::RESP_NACK, nullptr, 0);
                    rxIdx = 0;
                    continue;
                }

                // Frame valido: copia in IPCFrame e processa
                IPCFrame frame;
                frame.sync1       = rxBuf[0];
                frame.sync2       = rxBuf[1];
                frame.command     = rxBuf[2];
                frame.payload_len = rxBuf[3];
                if (payLen > 0) memcpy(frame.payload, &rxBuf[4], payLen);
                frame.crc8        = rxBuf[4 + payLen];

                rxIdx = 0;  // Reset buffer per prossimo frame

                // Processa comando
                switch (static_cast<IPCCommand>(frame.command)) {
                    case IPCCommand::CMD_PING:
                        ipc_slave_send_response(IPCResponse::RESP_PONG, nullptr, 0);
                        break;

                    case IPCCommand::CMD_SCENE_CHANGE:
                        if (frame.payload_len >= 1) {
                            dmx_slave_set_scene(frame.payload[0]);
                        }
                        ipc_slave_send_response(IPCResponse::RESP_ACK, nullptr, 0);
                        break;

                    case IPCCommand::CMD_DMX_RAW:
                        if (frame.payload_len >= 2) {
                            uint8_t startCh = frame.payload[0];
                            uint8_t count   = frame.payload[1];
                            if (count > 0 && frame.payload_len >= (2 + count)) {
                                dmx_slave_set_channels(startCh, &frame.payload[2], count);
                            }
                        }
                        ipc_slave_send_response(IPCResponse::RESP_ACK, nullptr, 0);
                        break;

                    case IPCCommand::CMD_STORAGE_SAVE_PRESET: {
                        if (frame.payload_len >= 1) {
                            uint8_t slot = frame.payload[0];
                            const uint8_t* data = &frame.payload[1];
                            size_t len = frame.payload_len - 1;
                            char key[32];
                            snprintf(key, sizeof(key), "preset_%u", slot);
                            bool ok = storage_slave_save(key, data, len);
                            ipc_slave_send_response(ok ? IPCResponse::RESP_ACK : IPCResponse::RESP_NACK,
                                                    nullptr, 0);
                        } else {
                            ipc_slave_send_response(IPCResponse::RESP_NACK, nullptr, 0);
                        }
                        break;
                    }

                    case IPCCommand::CMD_STORAGE_LOAD_PRESET: {
                        if (frame.payload_len >= 1) {
                            uint8_t slot = frame.payload[0];
                            char key[32];
                            snprintf(key, sizeof(key), "preset_%u", slot);
                            uint8_t buf[IPC_MAX_PAYLOAD];
                            size_t outLen = 0;
                            bool ok = storage_slave_load(key, buf, sizeof(buf), outLen);
                            if (ok && outLen > 0) {
                                ipc_slave_send_response(IPCResponse::RESP_STORAGE_DATA, buf,
                                                        static_cast<uint8_t>(outLen));
                            } else {
                                ipc_slave_send_response(IPCResponse::RESP_NACK, nullptr, 0);
                            }
                        } else {
                            ipc_slave_send_response(IPCResponse::RESP_NACK, nullptr, 0);
                        }
                        break;
                    }

                    case IPCCommand::CMD_STORAGE_SAVE_SCENE: {
                        if (frame.payload_len >= 1) {
                            uint8_t slot = frame.payload[0];
                            const uint8_t* data = &frame.payload[1];
                            size_t len = frame.payload_len - 1;
                            char key[32];
                            snprintf(key, sizeof(key), "scene_%u", slot);
                            bool ok = storage_slave_save(key, data, len);
                            ipc_slave_send_response(ok ? IPCResponse::RESP_ACK : IPCResponse::RESP_NACK,
                                                    nullptr, 0);
                        } else {
                            ipc_slave_send_response(IPCResponse::RESP_NACK, nullptr, 0);
                        }
                        break;
                    }

                    case IPCCommand::CMD_STORAGE_LOAD_SCENE: {
                        if (frame.payload_len >= 1) {
                            uint8_t slot = frame.payload[0];
                            char key[32];
                            snprintf(key, sizeof(key), "scene_%u", slot);
                            uint8_t buf[IPC_MAX_PAYLOAD];
                            size_t outLen = 0;
                            bool ok = storage_slave_load(key, buf, sizeof(buf), outLen);
                            if (ok && outLen > 0) {
                                ipc_slave_send_response(IPCResponse::RESP_STORAGE_DATA, buf,
                                                        static_cast<uint8_t>(outLen));
                            } else {
                                ipc_slave_send_response(IPCResponse::RESP_NACK, nullptr, 0);
                            }
                        } else {
                            ipc_slave_send_response(IPCResponse::RESP_NACK, nullptr, 0);
                        }
                        break;
                    }

                    default:
                        Serial.printf("[IPC_SLAVE] Comando sconosciuto: 0x%02X\n", frame.command);
                        ipc_slave_send_response(IPCResponse::RESP_NACK, nullptr, 0);
                        break;
                }
            }
        }
    }
}

void ipc_slave_send_response(IPCResponse resp, const uint8_t* data, uint8_t len) {
    if (len > IPC_MAX_PAYLOAD) len = IPC_MAX_PAYLOAD;

    // Calcola CRC
    uint8_t crc_data[2 + IPC_MAX_PAYLOAD];
    crc_data[0] = static_cast<uint8_t>(resp);
    crc_data[1] = len;
    if (len > 0 && data != nullptr) memcpy(&crc_data[2], data, len);
    uint8_t crc = crc8_compute(crc_data, 2 + len);

    // Scrivi frame su UART
    uint8_t rawBuf[4 + IPC_MAX_PAYLOAD + 1];
    rawBuf[0] = IPC_SYNC_BYTE_1;
    rawBuf[1] = IPC_SYNC_BYTE_2;
    rawBuf[2] = static_cast<uint8_t>(resp);
    rawBuf[3] = len;
    if (len > 0 && data != nullptr) memcpy(&rawBuf[4], data, len);
    rawBuf[4 + len] = crc;

    Serial1.write(rawBuf, 4 + len + 1);
    Serial1.flush();
}
