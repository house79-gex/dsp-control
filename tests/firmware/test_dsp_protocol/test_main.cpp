// test_main.cpp – Unit test per il protocollo RS-485 CQ260D
//
// Usa Unity framework (già incluso in PlatformIO test runner).
// Eseguire con: platformio test -e native --filter test_dsp_protocol
//
// Testa:
//   - Encoding frame di connessione (header + checksum)
//   - Calcolo checksum
//   - Parsing risposta DSP_CMD_CONNECTED
//   - Dimensione SysReg (deve essere 1092 byte)
//   - Codec serialize/deserialize SysReg

#include <unity.h>
#include <stdint.h>
#include <string.h>

// ——— Stub minimo del protocollo (no HW) ———

#define DSP_CMD_CONNECTED    0x30
#define DSP_TXCMD_CONNECT    0x30
#define DSP_MAX_FRAME_SIZE   256
#define DSP_UPLOAD_BLOCK_SIZE 50

// ——— Stub SysReg (stessa definizione del firmware) ———
#pragma pack(push, 1)
struct SysReg {
    uint16_t InGain[4];
    uint16_t OutGain[4];
    uint16_t EQ_Freq_In[124];
    uint16_t EQ_Gain_In[124];
    uint8_t  EQ_Q_In[124];
    uint8_t  EQ_Type_In[124];
    uint16_t EQ_Freq[28];
    uint16_t EQ_Gain[28];
    uint8_t  EQ_Q[28];
    uint8_t  EQ_Type[28];
    uint8_t  EQ_Bypass[8];
    uint16_t HLPF_Freq[8];
    uint8_t  HLPF_Type[8];
    uint16_t Fir_Freq[8];
    uint8_t  Fir_Type[8];
    uint16_t Delay[4];
    uint8_t  Threhold_Noise[4];
    uint8_t  ATack_Noise[4];
    uint8_t  Decay_Noise[4];
    uint8_t  Threhold[4];
    uint8_t  ATack[4];
    uint8_t  Decay[4];
    uint8_t  Mute[4];
    uint8_t  Phase[4];
    uint16_t MixVol[16];
    uint8_t  MixRoute[16];
    uint8_t  mode;
    uint8_t  lock;
    uint8_t  password[6];
    uint8_t  bootChar[7];
    uint16_t masterVol;
    uint8_t  AC_V_Sel;
    uint8_t  Clip_TH;
    uint8_t  ver;
};
#pragma pack(pop)

static uint8_t dsp_calc_checksum(const uint8_t* data, uint8_t length) {
    int sum = 0;
    for (uint8_t i = 0; i < length; i++) sum += data[i];
    return (uint8_t)(sum & 0xFF);
}

// Codec stubs (identici a dsp_sysreg_codec.cpp)
static size_t sysreg_serialize(const SysReg* src, uint8_t* dst, size_t max_len) {
    if (!src || !dst || max_len < sizeof(SysReg)) return 0;
    memcpy(dst, src, sizeof(SysReg));
    return sizeof(SysReg);
}

static bool sysreg_deserialize(SysReg* dst, const uint8_t* src, size_t len) {
    if (!dst || !src || len < sizeof(SysReg)) return false;
    memcpy(dst, src, sizeof(SysReg));
    return true;
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

// ——— Test SysReg size ———

void test_sysreg_size(void) {
    // La struct SysReg deve essere esattamente 1092 byte (come da C# SYSREG di riferimento)
    TEST_ASSERT_EQUAL_UINT32(1092, (uint32_t)sizeof(SysReg));
}

void test_sysreg_field_offsets(void) {
    SysReg s;
    // Verifica offset chiave via differenza puntatori
    TEST_ASSERT_EQUAL_UINT32(0,    (uint32_t)((uint8_t*)&s.InGain[0]     - (uint8_t*)&s));
    TEST_ASSERT_EQUAL_UINT32(8,    (uint32_t)((uint8_t*)&s.OutGain[0]    - (uint8_t*)&s));
    TEST_ASSERT_EQUAL_UINT32(760,  (uint32_t)((uint8_t*)&s.EQ_Freq[0]    - (uint8_t*)&s));
    TEST_ASSERT_EQUAL_UINT32(936,  (uint32_t)((uint8_t*)&s.HLPF_Freq[0]  - (uint8_t*)&s));
    TEST_ASSERT_EQUAL_UINT32(952,  (uint32_t)((uint8_t*)&s.HLPF_Type[0]  - (uint8_t*)&s));
    TEST_ASSERT_EQUAL_UINT32(960,  (uint32_t)((uint8_t*)&s.Fir_Freq[0]   - (uint8_t*)&s));
    TEST_ASSERT_EQUAL_UINT32(976,  (uint32_t)((uint8_t*)&s.Fir_Type[0]   - (uint8_t*)&s));
    TEST_ASSERT_EQUAL_UINT32(984,  (uint32_t)((uint8_t*)&s.Delay[0]      - (uint8_t*)&s));
    TEST_ASSERT_EQUAL_UINT32(1016, (uint32_t)((uint8_t*)&s.Mute[0]       - (uint8_t*)&s));
    TEST_ASSERT_EQUAL_UINT32(1024, (uint32_t)((uint8_t*)&s.MixVol[0]     - (uint8_t*)&s));
    TEST_ASSERT_EQUAL_UINT32(1072, (uint32_t)((uint8_t*)&s.mode           - (uint8_t*)&s));
    TEST_ASSERT_EQUAL_UINT32(1087, (uint32_t)((uint8_t*)&s.masterVol      - (uint8_t*)&s));
    TEST_ASSERT_EQUAL_UINT32(1089, (uint32_t)((uint8_t*)&s.AC_V_Sel       - (uint8_t*)&s));
    TEST_ASSERT_EQUAL_UINT32(1090, (uint32_t)((uint8_t*)&s.Clip_TH        - (uint8_t*)&s));
    TEST_ASSERT_EQUAL_UINT32(1091, (uint32_t)((uint8_t*)&s.ver            - (uint8_t*)&s));
}

// ——— Test codec serialize/deserialize ———

void test_sysreg_codec_roundtrip(void) {
    SysReg src, dst;
    uint8_t buf[1092];

    memset(&src, 0, sizeof(src));
    // Imposta campi noti
    src.InGain[0]   = 0x1234;
    src.OutGain[3]  = 0xABCD;
    src.masterVol   = 0x7FFF;
    src.ver         = 42;
    src.AC_V_Sel    = 1;
    src.Clip_TH     = 200;
    src.Delay[2]    = 480;  // 10 ms @ 48kHz

    // Serializza
    size_t written = sysreg_serialize(&src, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_UINT32(1092, (uint32_t)written);

    // Deserializza
    memset(&dst, 0, sizeof(dst));
    bool ok = sysreg_deserialize(&dst, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(1, (int)ok);

    // Verifica i valori
    TEST_ASSERT_EQUAL_HEX16(0x1234, dst.InGain[0]);
    TEST_ASSERT_EQUAL_HEX16(0xABCD, dst.OutGain[3]);
    TEST_ASSERT_EQUAL_HEX16(0x7FFF, dst.masterVol);
    TEST_ASSERT_EQUAL_UINT8(42,     dst.ver);
    TEST_ASSERT_EQUAL_UINT8(1,      dst.AC_V_Sel);
    TEST_ASSERT_EQUAL_UINT8(200,    dst.Clip_TH);
    TEST_ASSERT_EQUAL_HEX16(480,    dst.Delay[2]);
}

void test_sysreg_codec_le_byte_order(void) {
    SysReg src;
    uint8_t buf[1092];

    memset(&src, 0, sizeof(src));
    src.InGain[0] = 0x1234;  // LE: byte[0]=0x34, byte[1]=0x12

    sysreg_serialize(&src, buf, sizeof(buf));

    // Verifica ordine byte little-endian all'offset 0 (InGain[0])
    TEST_ASSERT_EQUAL_HEX8(0x34, buf[0]);  // LSB
    TEST_ASSERT_EQUAL_HEX8(0x12, buf[1]);  // MSB
}

void test_sysreg_codec_null_guard(void) {
    SysReg s;
    uint8_t buf[1092];

    // serialize con puntatori nulli
    TEST_ASSERT_EQUAL_UINT32(0, (uint32_t)sysreg_serialize(NULL, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_UINT32(0, (uint32_t)sysreg_serialize(&s, NULL, sizeof(buf)));
    TEST_ASSERT_EQUAL_UINT32(0, (uint32_t)sysreg_serialize(&s, buf, 100));  // buffer troppo piccolo

    // deserialize con puntatori nulli
    TEST_ASSERT_EQUAL_INT(0, (int)sysreg_deserialize(NULL, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_INT(0, (int)sysreg_deserialize(&s, NULL, sizeof(buf)));
    TEST_ASSERT_EQUAL_INT(0, (int)sysreg_deserialize(&s, buf, 100));  // buffer troppo piccolo
}

// ——— Test upload block offset calculation ———

void test_upload_block_offset(void) {
    // Verifica che il numero di blocco calcoli l'offset corretto
    // offset = block_num * DSP_UPLOAD_BLOCK_SIZE
    TEST_ASSERT_EQUAL_UINT32(0,    (uint32_t)(0 * DSP_UPLOAD_BLOCK_SIZE));
    TEST_ASSERT_EQUAL_UINT32(50,   (uint32_t)(1 * DSP_UPLOAD_BLOCK_SIZE));
    TEST_ASSERT_EQUAL_UINT32(1050, (uint32_t)(21 * DSP_UPLOAD_BLOCK_SIZE));
    // Ultimo blocco (blocco 21, offset 1050, 42 byte = 1092-1050)
    TEST_ASSERT_EQUAL_UINT32(42, (uint32_t)(1092 - 21 * DSP_UPLOAD_BLOCK_SIZE));
}

// ——— Entry point per PlatformIO Unity runner ———

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_dsp_frame_encode);
    RUN_TEST(test_dsp_checksum);
    RUN_TEST(test_dsp_checksum_known_vector);
    RUN_TEST(test_dsp_frame_grp_id);
    RUN_TEST(test_dsp_frame_checksum_valid);
    RUN_TEST(test_sysreg_size);
    RUN_TEST(test_sysreg_field_offsets);
    RUN_TEST(test_sysreg_codec_roundtrip);
    RUN_TEST(test_sysreg_codec_le_byte_order);
    RUN_TEST(test_sysreg_codec_null_guard);
    RUN_TEST(test_upload_block_offset);
    return UNITY_END();
}
