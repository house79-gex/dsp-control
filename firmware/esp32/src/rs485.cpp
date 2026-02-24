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

    // Invia broadcast ping a tutti gli indirizzi 1–127
    // TODO: sostituire con il protocollo reale PDA1001
    for (uint8_t addr = 1; addr <= 127; addr++) {
        uint8_t pingCmd[3] = { 0xFF, addr, CMD_PING };
        rs485_send_raw(pingCmd, sizeof(pingCmd));

        uint8_t respBuf[8];
        size_t respLen = 0;
        if (rs485_receive(respBuf, sizeof(respBuf), respLen, RS485_TIMEOUT)) {
            // TODO: decodificare risposta reale per tipo dispositivo
            // Per ora simula tre dispositivi fissi se riceve qualcosa
        }
    }

    // ===== STUB: simula 3 dispositivi =====
    // Rimuovere quando il protocollo reale è implementato
    Serial.println("[RS485] STUB: restituzione 3 dispositivi simulati");
    devices.push_back({ 1, "2WAY", true });
    devices.push_back({ 2, "3WAY", true });
    devices.push_back({ 3, "SUB",  true });
    // ===== FINE STUB =====

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
