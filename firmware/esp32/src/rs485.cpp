#include "rs485.h"
#include "config.h"

// UART dedicata per RS-485
#define RS485_UART      Serial2
#define RS485_BAUD      115200
#define RS485_TIMEOUT   100  // ms attesa risposta

// Byte comandi (protocollo placeholder – adattare al PDA1001/CQ260D reale)
#define CMD_PING        0x01
#define CMD_BEEP        0x02
#define CMD_PONG        0x81
#define CMD_DEVICE_INFO 0x60  // Richiesta informazioni dispositivo

// Frame header RS-485
#define FRAME_HEADER_0  0xFF
#define FRAME_HEADER_1  0x55

// ======= Funzioni interne DE/RE =======

static void rs485_tx_mode() {
    digitalWrite(RS485_DE, HIGH);
    digitalWrite(RS485_RE, HIGH);
    delayMicroseconds(10);
}

static void rs485_rx_mode() {
    RS485_UART.flush();
    digitalWrite(RS485_DE, LOW);
    digitalWrite(RS485_RE, LOW);
    delayMicroseconds(10);
}

// Calcola XOR checksum dei primi len byte del frame
static uint8_t calculate_checksum(const uint8_t* frame, size_t len) {
    uint8_t cs = 0;
    for (size_t i = 0; i < len; i++) cs ^= frame[i];
    return cs;
}

// Invia un frame RS-485 (include checksum come ultimo byte)
static void rs485_send_frame(const uint8_t* frame, size_t len) {
    rs485_tx_mode();
    RS485_UART.write(frame, len);
    RS485_UART.flush();
    rs485_rx_mode();
}

// Attende e riceve un frame RS-485 con header 0xFF 0x55
// Restituisce true se un frame valido è stato ricevuto
static bool rs485_receive_frame(uint8_t* buf, size_t* outLen, uint32_t timeoutMs) {
    uint32_t start = millis();
    size_t pos = 0;
    bool gotHeader = false;

    while ((millis() - start) < timeoutMs) {
        if (!RS485_UART.available()) {
            yield();
            continue;
        }
        uint8_t b = (uint8_t)RS485_UART.read();
        if (!gotHeader) {
            if (pos == 0 && b == FRAME_HEADER_0) {
                buf[pos++] = b;
            } else if (pos == 1 && b == FRAME_HEADER_1) {
                buf[pos++] = b;
                gotHeader = true;
            } else {
                pos = 0;  // reset
            }
        } else {
            buf[pos++] = b;
            // Byte 2 è la lunghezza del payload totale
            if (pos >= 3) {
                uint8_t frameLen = buf[2];
                if (pos >= (size_t)(frameLen + 1)) {  // +1 per checksum
                    *outLen = pos;
                    return true;
                }
            }
            if (pos >= 64) break;  // overflow guard
        }
    }
    *outLen = 0;
    return false;
}

// Ricava il tipo di dispositivo dalla risposta CMD_DEVICE_INFO
static std::string parse_model_from_response(const uint8_t* response, size_t len) {
    if (len < 10) return "UNKNOWN";
    uint8_t typeCode = response[8];
    switch (typeCode) {
        case 0x01: return "2WAY";
        case 0x02: return "3WAY";
        case 0x03: return "SUB";
        default:   return "GENERIC";
    }
}

// ======= API pubblica =======

void rs485_init() {
    pinMode(RS485_DE, OUTPUT);
    pinMode(RS485_RE, OUTPUT);
    rs485_rx_mode();

    RS485_UART.begin(RS485_BAUD, SERIAL_8N1, RS485_RX, RS485_TX);
    Serial.println("[RS485] Inizializzato");
}

void rs485_send_raw(const uint8_t* data, size_t len) {
    rs485_tx_mode();
    RS485_UART.write(data, len);
    RS485_UART.flush();
    rs485_rx_mode();
}

bool rs485_receive(uint8_t* buf, size_t maxLen, size_t& received, uint32_t timeoutMs) {
    uint32_t start = millis();
    received = 0;

    while ((millis() - start) < timeoutMs) {
        if (RS485_UART.available()) {
            int b = RS485_UART.read();
            if (b >= 0 && received < maxLen) {
                buf[received++] = (uint8_t)b;
            }
        }
        yield();
    }
    return (received > 0);
}

std::vector<SpeakerDevice> rs485_scan_devices() {
    Serial.println("[RS485] Avvio scansione dispositivi...");
    std::vector<SpeakerDevice> devices;

    // Scan gruppi 0-7, ID 0-15 (max 128 indirizzi)
    for (uint8_t grp = 0; grp < 8; grp++) {
        for (uint8_t id = 0; id < 16; id++) {
            // Costruisci frame CMD_DEVICE_INFO
            // Frame: [0xFF][0x55][len=5][grp][id][cmd][data][checksum]
            // len = numero byte dopo il campo len fino al checksum (escluso) = 4
            uint8_t frame[8];
            frame[0] = FRAME_HEADER_0;
            frame[1] = FRAME_HEADER_1;
            frame[2] = 0x06;          // Lunghezza payload (grp+id+cmd+data = 4 byte + 2 header)
            frame[3] = grp;
            frame[4] = id;
            frame[5] = CMD_DEVICE_INFO;
            frame[6] = 0x00;          // Nessun dato aggiuntivo
            frame[7] = calculate_checksum(frame, 7);

            rs485_send_frame(frame, 8);

            // Attendi risposta
            uint8_t response[64];
            size_t  respLen = 0;

            if (rs485_receive_frame(response, &respLen, 50)) {
                // Verifica header e comando risposta
                if (response[0] == FRAME_HEADER_0 &&
                    response[1] == FRAME_HEADER_1 &&
                    respLen >= 6 && response[5] == CMD_DEVICE_INFO) {

                    std::string model = parse_model_from_response(response, respLen);

                    SpeakerDevice dev;
                    dev.id     = (uint8_t)((grp << 4) | id);
                    dev.type   = model;
                    dev.online = true;
                    devices.push_back(dev);

                    Serial.printf("[RS485] Trovato dispositivo: GRP=%d ID=%d Tipo=%s\n",
                                  grp, id, model.c_str());
                }
            }

            vTaskDelay(pdMS_TO_TICKS(10));  // Pausa tra probe
        }
    }

    Serial.printf("[RS485] Scansione completata: %d dispositivi trovati\n", (int)devices.size());
    return devices;
}

void rs485_send_beep(uint8_t id, BeepPattern pattern) {
    uint8_t count = 0;
    switch (pattern) {
        case BeepPattern::Single: count = 1; break;
        case BeepPattern::Double: count = 2; break;
        case BeepPattern::Triple: count = 3; break;
    }

    // TODO: adattare al comando beep reale del PDA1001/CQ260D
    uint8_t cmd[4] = { 0xFF, id, CMD_BEEP, count };
    rs485_send_raw(cmd, sizeof(cmd));

    Serial.printf("[RS485] Beep inviato a ID %d: %d beep\n", id, count);
}
