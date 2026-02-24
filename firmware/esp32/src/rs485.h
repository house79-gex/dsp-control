#pragma once
#include <Arduino.h>
#include <vector>
#include <string>

enum class BeepPattern {
    Single,   // 1 beep: cassa identificata
    Double,   // 2 beep: cassa selezionata
    Triple    // 3 beep: conferma assegnazione
};

struct SpeakerDevice {
    uint8_t id;
    std::string type;  // "2WAY", "3WAY", "SUB"
    bool online;
};

enum class SpeakerRole { SX, DX, SUB, MONO };

struct SpeakerAssignment {
    uint8_t deviceId;
    std::string position;  // "SX1", "DX1", "SUB1", ecc.
    SpeakerRole role;
    std::string type;      // "2WAY", "3WAY", "SUB"
    std::string name;      // nome personalizzato
};

void rs485_init();
std::vector<SpeakerDevice> rs485_scan_devices();
void rs485_send_beep(uint8_t id, BeepPattern pattern);
void rs485_send_raw(const uint8_t* data, size_t len);
bool rs485_receive(uint8_t* buf, size_t maxLen, size_t& received, uint32_t timeoutMs = 100);
