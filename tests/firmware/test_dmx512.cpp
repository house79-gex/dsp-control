// test_dmx512.cpp – Unit test per il layer DMX512
//
// Usa Unity framework (già incluso in PlatformIO test runner).
// Eseguire con: platformio test -e native --filter test_dmx512
//
// Testa:
//   - Aggiunta fixture e accesso canali
//   - Impostazione colore RGB su fixture PAR
//   - Generazione buffer DMX (canali 1-512)
//   - DMX master dimmer scaling

#include <unity.h>
#include <stdint.h>
#include <string.h>

// ——— Stub minimo del modulo DMX (no HW) ———

#define DMX_CHANNELS              512
#define DMX_MAX_CHANNELS_PER_FIXTURE 32
#define DMX_TYPE_PAR_RGB          "PAR_RGB"
#define DMX_TYPE_STROBE           "STROBE"

struct DmxFixture {
    uint8_t  id;
    char     name[32];
    char     type[20];
    uint16_t startAddress;
    uint8_t  numChannels;
    uint8_t  groupId;
    char     groupName[32];
    uint8_t  channels[DMX_MAX_CHANNELS_PER_FIXTURE];
    char     channelNames[DMX_MAX_CHANNELS_PER_FIXTURE][16];
    bool     online;
};

static uint8_t s_dmxBuffer[DMX_CHANNELS + 1] = { 0 };
static uint8_t s_masterDimmer = 255;
static bool    s_blackout = false;

// Fixture storage semplificato
static DmxFixture s_fixtures[32];
static int        s_fixtureCount = 0;

static DmxFixture* get_fixture(uint8_t id) {
    for (int i = 0; i < s_fixtureCount; i++)
        if (s_fixtures[i].id == id) return &s_fixtures[i];
    return nullptr;
}

static void dmx_add_fixture(const DmxFixture& fx) {
    for (int i = 0; i < s_fixtureCount; i++) {
        if (s_fixtures[i].id == fx.id) { s_fixtures[i] = fx; return; }
    }
    if (s_fixtureCount < 32) s_fixtures[s_fixtureCount++] = fx;
}

static void dmx_set_channel(uint16_t channel, uint8_t value) {
    if (channel < 1 || channel > DMX_CHANNELS) return;
    s_dmxBuffer[channel] = s_blackout ? 0 : (uint8_t)((value * s_masterDimmer) / 255);
}

static void dmx_set_fixture_channel(uint8_t fixtureId, uint8_t chIdx, uint8_t value) {
    DmxFixture* fx = get_fixture(fixtureId);
    if (!fx || chIdx >= fx->numChannels) return;
    fx->channels[chIdx] = value;
    dmx_set_channel(fx->startAddress + chIdx, value);
}

static void dmx_set_fixture_color(uint8_t fixtureId, uint8_t r, uint8_t g, uint8_t b) {
    DmxFixture* fx = get_fixture(fixtureId);
    if (!fx || fx->numChannels < 3) return;
    dmx_set_fixture_channel(fixtureId, 0, r);
    dmx_set_fixture_channel(fixtureId, 1, g);
    dmx_set_fixture_channel(fixtureId, 2, b);
}

static void dmx_reset() {
    memset(s_dmxBuffer, 0, sizeof(s_dmxBuffer));
    s_fixtureCount = 0;
    s_masterDimmer = 255;
    s_blackout = false;
}

// ——— Test cases ———

void setUp(void) { dmx_reset(); }
void tearDown(void) {}

void test_dmx_add_fixture(void) {
    DmxFixture fx = {};
    fx.id = 1; fx.startAddress = 1; fx.numChannels = 3;
    strncpy(fx.name, "PAR 1", sizeof(fx.name) - 1);
    strncpy(fx.type, DMX_TYPE_PAR_RGB, sizeof(fx.type) - 1);
    dmx_add_fixture(fx);
    TEST_ASSERT_NOT_NULL(get_fixture(1));
    TEST_ASSERT_EQUAL_STRING("PAR 1", get_fixture(1)->name);
}

void test_dmx_frame_generation(void) {
    DmxFixture fx = {};
    fx.id = 1; fx.startAddress = 1; fx.numChannels = 3;
    strncpy(fx.name, "PAR 1", sizeof(fx.name) - 1);
    strncpy(fx.type, DMX_TYPE_PAR_RGB, sizeof(fx.type) - 1);
    dmx_add_fixture(fx);

    // Orange: R=255, G=128, B=0
    dmx_set_fixture_color(1, 255, 128, 0);

    TEST_ASSERT_EQUAL_UINT8(255, s_dmxBuffer[1]);  // Red   @ addr 1
    TEST_ASSERT_EQUAL_UINT8(128, s_dmxBuffer[2]);  // Green @ addr 2
    TEST_ASSERT_EQUAL_UINT8(0,   s_dmxBuffer[3]);  // Blue  @ addr 3
}

void test_dmx_master_dimmer(void) {
    DmxFixture fx = {};
    fx.id = 2; fx.startAddress = 10; fx.numChannels = 3;
    strncpy(fx.type, DMX_TYPE_PAR_RGB, sizeof(fx.type) - 1);
    dmx_add_fixture(fx);

    // Master dimmer al 50% (128)
    s_masterDimmer = 128;
    dmx_set_fixture_color(2, 255, 255, 255);

    // 255 * 128 / 255 = 128
    TEST_ASSERT_EQUAL_UINT8(128, s_dmxBuffer[10]);
    TEST_ASSERT_EQUAL_UINT8(128, s_dmxBuffer[11]);
    TEST_ASSERT_EQUAL_UINT8(128, s_dmxBuffer[12]);
}

void test_dmx_blackout(void) {
    DmxFixture fx = {};
    fx.id = 3; fx.startAddress = 20; fx.numChannels = 3;
    strncpy(fx.type, DMX_TYPE_PAR_RGB, sizeof(fx.type) - 1);
    dmx_add_fixture(fx);
    dmx_set_fixture_color(3, 255, 255, 255);

    // Attiva blackout
    s_blackout = true;
    dmx_set_fixture_color(3, 255, 255, 255);  // Tentativo di impostare colore

    TEST_ASSERT_EQUAL_UINT8(0, s_dmxBuffer[20]);
    TEST_ASSERT_EQUAL_UINT8(0, s_dmxBuffer[21]);
    TEST_ASSERT_EQUAL_UINT8(0, s_dmxBuffer[22]);
}

void test_dmx_channel_bounds(void) {
    // Canale 0: non valido (start code DMX)
    dmx_set_channel(0, 255);
    TEST_ASSERT_EQUAL_UINT8(0, s_dmxBuffer[0]);

    // Canale 512: valido (ultimo canale DMX)
    dmx_set_channel(512, 200);
    TEST_ASSERT_EQUAL_UINT8(200, s_dmxBuffer[512]);

    // Canale 513: fuori range, non deve modificare il buffer
    uint8_t before = s_dmxBuffer[512];
    dmx_set_channel(513, 100);
    TEST_ASSERT_EQUAL_UINT8(before, s_dmxBuffer[512]);
}

// ——— Entry point per PlatformIO Unity runner ———

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_dmx_add_fixture);
    RUN_TEST(test_dmx_frame_generation);
    RUN_TEST(test_dmx_master_dimmer);
    RUN_TEST(test_dmx_blackout);
    RUN_TEST(test_dmx_channel_bounds);
    return UNITY_END();
}
