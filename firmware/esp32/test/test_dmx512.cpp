#include <unity.h>
#include <stdint.h>

// Mock DMX structures
#define DMX_PACKET_SIZE 512
uint8_t dmx_buffer[DMX_PACKET_SIZE];

void dmx_set_channel(uint16_t channel, uint8_t value) {
    if (channel > 0 && channel <= DMX_PACKET_SIZE) {
        dmx_buffer[channel - 1] = value;
    }
}

uint8_t dmx_get_channel(uint16_t channel) {
    if (channel > 0 && channel <= DMX_PACKET_SIZE) {
        return dmx_buffer[channel - 1];
    }
    return 0;
}

void dmx_blackout() {
    for (int i = 0; i < DMX_PACKET_SIZE; i++) {
        dmx_buffer[i] = 0;
    }
}

void test_dmx_buffer_write() {
    dmx_set_channel(1, 255);
    TEST_ASSERT_EQUAL_UINT8(255, dmx_buffer[0]);
}

void test_dmx_buffer_read() {
    dmx_buffer[10] = 128;
    TEST_ASSERT_EQUAL_UINT8(128, dmx_get_channel(11));
}

void test_dmx_blackout() {
    dmx_buffer[0] = 255;
    dmx_buffer[100] = 128;
    dmx_blackout();
    
    TEST_ASSERT_EQUAL_UINT8(0, dmx_buffer[0]);
    TEST_ASSERT_EQUAL_UINT8(0, dmx_buffer[100]);
}

void test_dmx_bounds() {
    // Test out of bounds (channel 0)
    dmx_set_channel(0, 255);
    TEST_ASSERT_EQUAL_UINT8(0, dmx_get_channel(0));
    TEST_ASSERT_EQUAL_UINT8(0, dmx_buffer[0]); // buffer must not be modified
    
    // Test valid boundary
    dmx_set_channel(512, 200);
    TEST_ASSERT_EQUAL_UINT8(200, dmx_get_channel(512));
    
    // Test out of bounds (channel 513)
    dmx_set_channel(513, 100);
    TEST_ASSERT_EQUAL_UINT8(0, dmx_get_channel(513));
}

void setUp(void) {
    dmx_blackout();
}

void tearDown(void) {}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_dmx_buffer_write);
    RUN_TEST(test_dmx_buffer_read);
    RUN_TEST(test_dmx_blackout);
    RUN_TEST(test_dmx_bounds);
    return UNITY_END();
}
