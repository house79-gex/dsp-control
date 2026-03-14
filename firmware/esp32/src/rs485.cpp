#include "rs485.h"
#include "config.h"
#include "dsp_protocol.h"

// UART dedicata per RS-485
#define RS485_UART      Serial2
#define RS485_BAUD      115200
#define RS485_TIMEOUT   100  // ms attesa risposta

// ======= Funzioni interne DE/RE (driver MAX485) =======

static void rs485_tx_mode() {
#if RS485_DE_RE >= 0
    digitalWrite(RS485_DE, HIGH);
    digitalWrite(RS485_RE, HIGH);
    delayMicroseconds(10);
#endif
}

static void rs485_rx_mode() {
    RS485_UART.flush();
#if RS485_DE_RE >= 0
    digitalWrite(RS485_DE, LOW);
    digitalWrite(RS485_RE, LOW);
    delayMicroseconds(10);
#endif
}

void rs485_init() {
#if RS485_DE_RE >= 0
    pinMode(RS485_DE, OUTPUT);
    pinMode(RS485_RE, OUTPUT);
    rs485_rx_mode();
#endif

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

void rs485_send_beep(uint8_t compactId, BeepPattern pattern) {
    uint8_t count = 1;
    switch (pattern) {
        case BeepPattern::Single: count = 1; break;
        case BeepPattern::Double: count = 2; break;
        case BeepPattern::Triple: count = 3; break;
    }

    /* Il CQ260D non espone un comando "beep" documentato. Identificazione sonora:
     * impulsi di OutGain (SET_PARAM 0x40, tipo 0x01) → picco udibile sull'amplificatore.
     * Richiede segnale audio in ingresso al modulo o rumore di fondo per essere percettibile.
     * Dopo l'impulso si ripristina un gain centrale (~0 dB scala lineare). */
    DspId dspId;
    dspId.grp   = (uint8_t)(compactId >> 4);
    dspId.dspID = (uint16_t)(compactId & 0x0F);

    const uint16_t kRestore = 0x8000u; /* ~lineare metà scala → vicino 0 dB */
    const uint16_t kBoost   = 0xF000u; /* picco verso +12 dB zona */

    Serial.printf("[RS485] Identificazione cassa GRP=%u ID=%u (%d impulsi gain)\n",
                  (unsigned)dspId.grp, (unsigned)dspId.dspID, count);

    for (uint8_t n = 0; n < count; n++) {
        uint8_t up[] = { 4, DSP_TXCMD_PARAM_SET, 0x01, 0,
                         (uint8_t)(kBoost & 0xFF), (uint8_t)(kBoost >> 8) };
        dsp_command_send(up, sizeof(up), dspId);
        delay(320);
        uint8_t down[] = { 4, DSP_TXCMD_PARAM_SET, 0x01, 0,
                           (uint8_t)(kRestore & 0xFF), (uint8_t)(kRestore >> 8) };
        dsp_command_send(down, sizeof(down), dspId);
        delay(180);
    }
}
