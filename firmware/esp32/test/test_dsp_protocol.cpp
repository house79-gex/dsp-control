#include <unity.h>
#include <stdint.h>
#include <string.h>

// Mock DSP protocol structures
struct DspFrame {
    uint8_t sync;
    uint8_t id;
    uint8_t cmd;
    uint8_t data[8];
    uint8_t checksum;
};

uint8_t calculate_checksum(const DspFrame* frame) {
    uint8_t sum = frame->sync + frame->id + frame->cmd;
    for (int i = 0; i < 8; i++) {
        sum += frame->data[i];
    }
    return sum;
}

void test_dsp_frame_encoding() {
    DspFrame frame = {0xAA, 0x01, 0x10, {0}, 0};
    frame.checksum = calculate_checksum(&frame);
    
    TEST_ASSERT_EQUAL_UINT8(0xAA, frame.sync);
    TEST_ASSERT_EQUAL_UINT8(0x01, frame.id);
    TEST_ASSERT_EQUAL_UINT8(0x10, frame.cmd);
    TEST_ASSERT_EQUAL_UINT8(0xBB, frame.checksum); // 0xAA + 0x01 + 0x10 = 0xBB
}

void test_dsp_checksum_validation() {
    DspFrame frame = {0xAA, 0x02, 0x20, {1, 2, 3, 4, 5, 6, 7, 8}, 0};
    frame.checksum = calculate_checksum(&frame);
    
    uint8_t expected = 0xAA + 0x02 + 0x20 + 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8;
    TEST_ASSERT_EQUAL_UINT8(expected, frame.checksum);
}

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_dsp_frame_encoding);
    RUN_TEST(test_dsp_checksum_validation);
    return UNITY_END();
}
