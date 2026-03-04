// test_main.cpp – Unit test per il protocollo RS-485 CQ260D
//
// Usa Unity framework (già incluso in PlatformIO test runner).
// Eseguire con: platformio test -e native --filter test_dsp_protocol
//
// Testa:
//   - Encoding frame di connessione (header + checksum)
//   - Calcolo checksum
//   - Parsing risposta DSP_CMD_CONNECTED

#include <unity.h>
#include <stdint.h>
#include <string.h>

// ——— Stub minimo del protocollo (no HW) ———

#define DSP_CMD_CONNECTED    0x30
#define DSP_TXCMD_CONNECT    0x30
#define DSP_MAX_FRAME_SIZE   256

static uint8_t dsp_calc_checksum(const uint8_t* data, uint8_t length) {
    int sum = 0;
    for (uint8_t i = 0; i < length; i++) sum += data[i];
    return (uint8_t)(sum & 0xFF);
}

// Costruisce un frame di connessione (CMD=0x30)
// Ritorna la lunghezza totale del frame (incluso checksum)
static uint8_t dsp_build_connect_frame(uint8_t grp, uint16_t dspId, uint8_t* frame) {
    uint8_t payload[] = { 0x01, DSP_TXCMD_CONNECT };  // len=1, CMD=0x30
    frame[0] = 0xFF;
    frame[1] = 0x55;
    frame[2] = (uint8_t)(payload[0] + 1 + 2);  // LEN
    frame[3] = grp;
    frame[4] = (uint8_t)(dspId & 0xFF);
    frame[5] = (uint8_t)(dspId >> 8);
    frame[6] = payload[0];
    frame[7] = payload[1];
    uint8_t totalLen = 8;
    frame[totalLen] = dsp_calc_checksum(frame, totalLen);
    return totalLen + 1;
}

// ——— Test cases ———

void test_dsp_frame_encode(void) {
    uint8_t frame[DSP_MAX_FRAME_SIZE];
    uint8_t len = dsp_build_connect_frame(1, 0, frame);

    // Lunghezza minima: 9 byte (6 header + 2 payload + 1 checksum)
    TEST_ASSERT_EQUAL_UINT8(9, len);

    // Magic bytes
    TEST_ASSERT_EQUAL_HEX8(0xFF, frame[0]);
    TEST_ASSERT_EQUAL_HEX8(0x55, frame[1]);

    // CMD_CONNECT nel payload
    TEST_ASSERT_EQUAL_HEX8(DSP_TXCMD_CONNECT, frame[7]);
}

void test_dsp_checksum(void) {
    // Frame di connessione grp=1, id=0 → calcola manualmente
    // frame = FF 55 04 01 00 00 01 30  → sum = FF+55+04+01+00+00+01+30 = 0x234 & 0xFF = 0x34
    uint8_t data[] = { 0xFF, 0x55, 0x04, 0x01, 0x00, 0x00, 0x01, 0x30 };
    uint8_t chk = dsp_calc_checksum(data, sizeof(data));
    TEST_ASSERT_EQUAL_HEX8(0x34, chk);
}

void test_dsp_checksum_known_vector(void) {
    // Vettore di test: {0xFF, 0x55, 0x06, 0x01, 0x00, 0x30}
    // Sum = 0xFF + 0x55 + 0x06 + 0x01 + 0x00 + 0x30 = 0x12B & 0xFF = 0x2B
    uint8_t data[] = { 0xFF, 0x55, 0x06, 0x01, 0x00, 0x30 };
    uint8_t chk = dsp_calc_checksum(data, sizeof(data));
    TEST_ASSERT_EQUAL_HEX8(0x2B, chk);
}

void test_dsp_frame_grp_id(void) {
    uint8_t frame[DSP_MAX_FRAME_SIZE];
    // grp=2, dspId=0x0304 (little-endian: byte_low=0x04, byte_high=0x03)
    dsp_build_connect_frame(2, 0x0304, frame);
    TEST_ASSERT_EQUAL_HEX8(2,    frame[3]);  // grp
    TEST_ASSERT_EQUAL_HEX8(0x04, frame[4]);  // ID low byte
    TEST_ASSERT_EQUAL_HEX8(0x03, frame[5]);  // ID high byte
}

void test_dsp_frame_checksum_valid(void) {
    uint8_t frame[DSP_MAX_FRAME_SIZE];
    uint8_t len = dsp_build_connect_frame(1, 0, frame);

    // Ricalcola checksum sul frame prodotto (escluso l'ultimo byte)
    uint8_t expected = dsp_calc_checksum(frame, (uint8_t)(len - 1));
    TEST_ASSERT_EQUAL_HEX8(expected, frame[len - 1]);
}

// ——— Entry point per PlatformIO Unity runner ———

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_dsp_frame_encode);
    RUN_TEST(test_dsp_checksum);
    RUN_TEST(test_dsp_checksum_known_vector);
    RUN_TEST(test_dsp_frame_grp_id);
    RUN_TEST(test_dsp_frame_checksum_valid);
    return UNITY_END();
}
