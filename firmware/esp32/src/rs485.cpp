#include "rs485.h"
#include "config.h"
#include "dsp_protocol.h"

// UART dedicata per RS-485
#define RS485_UART      Serial2
#define RS485_BAUD      115200
#define RS485_TIMEOUT   100  // ms attesa risposta

// Byte comandi aggiuntivi lato host
#define CMD_BEEP        0x02

// ======= Funzioni interne DE/RE (driver MAX485) =======

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
    Serial.println("[RS485] Avvio scansione dispositivi CQ260D...");
    std::vector<SpeakerDevice> devices;

    // Scan gruppi 0-7, ID logici 0-15 (max 128 indirizzi)
    // Usa il protocollo ufficiale CQ260D (dsp_connect) per verificare la presenza.
    for (uint8_t grp = 0; grp < 8; grp++) {
        for (uint8_t logicalId = 0; logicalId < 16; logicalId++) {
            DspId dspId;
            dspId.grp   = grp;
            dspId.dspID = logicalId;

            Serial.printf("[RS485] Probe GRP=%u ID=%u...\n", grp, logicalId);

            if (dsp_connect(dspId)) {
                // Usa lo stato DSP per eventuali informazioni aggiuntive
                DspStatus* st = dsp_get_status();

                SpeakerDevice dev;
                dev.id     = (uint8_t)((grp << 4) | logicalId);
                dev.type   = "GENERIC";
                dev.online = (st != nullptr) ? st->connected : true;

                devices.push_back(dev);

                Serial.printf("[RS485] Trovato dispositivo: GRP=%u ID=%u\n",
                              grp, logicalId);
            }

            vTaskDelay(pdMS_TO_TICKS(10));  // Pausa tra probe per non saturare il bus
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
