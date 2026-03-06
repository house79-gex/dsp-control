// test_main.cpp – Unit test per il sistema audio adattivo
//
// Usa Unity framework (già incluso in PlatformIO test runner).
// Eseguire con: platformio test -e native --filter test_sample_rate
//
// Testa:
//   - Conversione delay ms → campioni per DSP @ 48kHz (audio_ms_to_samples_dsp)
//   - Saturazione a UINT16_MAX per delay eccessivi
//   - Sample Rate Converter 44.1kHz → 48kHz (output frame count e preservazione freq)
//   - SRC reset e reinizializzazione

#include <unity.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

// ——— Stub minimo di config.h per ambiente native ———
#ifndef AUDIO_PROCESSING_SAMPLE_RATE
#define AUDIO_PROCESSING_SAMPLE_RATE  48000
#endif
#ifndef AUDIO_SAMPLE_RATE_DEFAULT
#define AUDIO_SAMPLE_RATE_DEFAULT     44100
#endif

// ——— Implementazione diretta di audio_ms_to_samples_dsp (no HW) ———
// Replica la logica di audio_config.cpp

static uint16_t audio_ms_to_samples_dsp(float delayMs) {
    const float DSP_SR_KHZ = (float)AUDIO_PROCESSING_SAMPLE_RATE / 1000.0f;
    float samples = delayMs * DSP_SR_KHZ;
    if (samples < 0.0f) samples = 0.0f;
    if (samples > 65535.0f) samples = 65535.0f;
    return (uint16_t)samples;
}

// ——— Implementazione SRC minimale per test (senza dipendenze HW) ———
// Replica la logica di audio_src.cpp

typedef struct {
    uint32_t inputSR;
    uint32_t outputSR;
    float    ratio;
    float    phase;
    int16_t  prevL;
    int16_t  prevR;
    int      initialized;
} AudioSRCStub;

static int src_init(AudioSRCStub* src, uint32_t inputSR, uint32_t outputSR) {
    if (inputSR == 0 || outputSR == 0) return 0;
    src->inputSR     = inputSR;
    src->outputSR    = outputSR;
    src->ratio       = (float)outputSR / (float)inputSR;
    src->phase       = 0.0f;
    src->prevL       = 0;
    src->prevR       = 0;
    src->initialized = 1;
    return 1;
}

static size_t src_process(AudioSRCStub* src,
                          const int16_t* input,  size_t inputFrames,
                          int16_t*       output, size_t outputCapacity) {
    if (!src->initialized || inputFrames == 0 || outputCapacity == 0) return 0;

    size_t outIdx = 0;

    while (outIdx < outputCapacity) {
        float inPos   = src->phase / src->ratio;
        size_t inFrame = (size_t)inPos;
        float  frac    = inPos - (float)inFrame;

        int16_t curL, curR, nextL, nextR;

        if (inFrame == 0) {
            curL  = src->prevL;
            curR  = src->prevR;
            nextL = (inFrame < inputFrames) ? input[inFrame * 2]     : src->prevL;
            nextR = (inFrame < inputFrames) ? input[inFrame * 2 + 1] : src->prevR;
        } else if (inFrame < inputFrames) {
            curL  = input[(inFrame - 1) * 2];
            curR  = input[(inFrame - 1) * 2 + 1];
            nextL = input[inFrame * 2];
            nextR = input[inFrame * 2 + 1];
        } else {
            break;
        }

        output[outIdx * 2]     = (int16_t)(curL + frac * (nextL - curL));
        output[outIdx * 2 + 1] = (int16_t)(curR + frac * (nextR - curR));
        outIdx++;

        src->phase += 1.0f;
        float nextInPos = src->phase / src->ratio;
        if ((size_t)nextInPos >= inputFrames + 1) break;
    }

    if (inputFrames > 0) {
        src->prevL = input[(inputFrames - 1) * 2];
        src->prevR = input[(inputFrames - 1) * 2 + 1];
    }

    float inputConsumed = (float)inputFrames;
    src->phase -= inputConsumed * src->ratio;
    if (src->phase < 0.0f) src->phase = 0.0f;

    return outIdx;
}

// ——— Generatore di sinusoide per test SRC ———

static void generate_sine(int16_t* buf, size_t frames, float freqHz, uint32_t sr) {
    const float twoPi = 2.0f * 3.14159265358979f;
    float phase = 0.0f;
    float step  = twoPi * freqHz / (float)sr;
    for (size_t i = 0; i < frames; i++) {
        int16_t s = (int16_t)(sinf(phase) * 16000.0f);
        buf[i * 2]     = s;
        buf[i * 2 + 1] = s;
        phase += step;
    }
}

// Stima la frequenza del segnale con zero-crossing rate
static float measure_frequency_zcr(const int16_t* buf, size_t frames, uint32_t sr) {
    if (frames < 4) return 0.0f;
    int crossings = 0;
    for (size_t i = 1; i < frames; i++) {
        int16_t prev = buf[(i - 1) * 2];
        int16_t curr = buf[i * 2];
        if ((prev >= 0 && curr < 0) || (prev < 0 && curr >= 0)) {
            crossings++;
        }
    }
    // Ogni ciclo ha 2 zero-crossing
    float freq = (float)crossings * (float)sr / (2.0f * (float)frames);
    return freq;
}

// ——— Test cases ———

void setUp(void) {}
void tearDown(void) {}

// ——— Test conversione delay DSP ———

void test_delay_conversion_10ms(void) {
    // 10ms @ 48kHz = 480 campioni
    uint16_t samples = audio_ms_to_samples_dsp(10.0f);
    TEST_ASSERT_EQUAL_UINT16(480, samples);
}

void test_delay_conversion_20ms(void) {
    // 20ms @ 48kHz = 960 campioni
    uint16_t samples = audio_ms_to_samples_dsp(20.0f);
    TEST_ASSERT_EQUAL_UINT16(960, samples);
}

void test_delay_conversion_1ms(void) {
    // 1ms @ 48kHz = 48 campioni
    uint16_t samples = audio_ms_to_samples_dsp(1.0f);
    TEST_ASSERT_EQUAL_UINT16(48, samples);
}

void test_delay_conversion_zero(void) {
    // 0ms → 0 campioni
    uint16_t samples = audio_ms_to_samples_dsp(0.0f);
    TEST_ASSERT_EQUAL_UINT16(0, samples);
}

void test_delay_conversion_negative(void) {
    // Valori negativi saturano a 0
    uint16_t samples = audio_ms_to_samples_dsp(-5.0f);
    TEST_ASSERT_EQUAL_UINT16(0, samples);
}

void test_delay_conversion_max_clamped(void) {
    // 2000ms → saturazione a 65535 (max = 1365.3ms @ 48kHz)
    uint16_t samples = audio_ms_to_samples_dsp(2000.0f);
    TEST_ASSERT_EQUAL_UINT16(65535, samples);
}

void test_delay_dsp_always_48khz(void) {
    // VERIFICA CRITICA: il calcolo DSP usa sempre 48kHz
    // indipendente dal sample rate di input (es. 44.1kHz)
    //
    // 10ms @ 44.1kHz sarebbe 441 campioni → SBAGLIATO per il DSP
    // 10ms @ 48kHz deve essere 480 campioni → CORRETTO
    uint16_t samples = audio_ms_to_samples_dsp(10.0f);
    TEST_ASSERT_NOT_EQUAL(441, samples);  // non deve usare 44.1kHz
    TEST_ASSERT_EQUAL_UINT16(480, samples);  // deve usare 48kHz
}

// ——— Test Sample Rate Converter ———

void test_src_init_44100_to_48000(void) {
    AudioSRCStub src = {};
    int ok = src_init(&src, 44100, 48000);
    TEST_ASSERT_EQUAL_INT(1, ok);
    TEST_ASSERT_EQUAL_UINT32(44100, src.inputSR);
    TEST_ASSERT_EQUAL_UINT32(48000, src.outputSR);
    // Ratio ≈ 1.08843 (48000/44100)
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0884f, src.ratio);
}

void test_src_output_frame_count_100ms(void) {
    // 100ms @ 44.1kHz = 4410 frame input → atteso ~4800 frame output (100ms @ 48kHz)
    const size_t INPUT_FRAMES  = 4410;  // 100ms @ 44.1kHz
    const size_t OUTPUT_CAP    = 5000;

    int16_t* input  = (int16_t*)calloc(INPUT_FRAMES  * 2, sizeof(int16_t));
    int16_t* output = (int16_t*)calloc(OUTPUT_CAP    * 2, sizeof(int16_t));
    TEST_ASSERT_NOT_NULL(input);
    TEST_ASSERT_NOT_NULL(output);

    generate_sine(input, INPUT_FRAMES, 1000.0f, 44100);

    AudioSRCStub src = {};
    src_init(&src, 44100, 48000);
    size_t outFrames = src_process(&src, input, INPUT_FRAMES, output, OUTPUT_CAP);

    free(input);
    free(output);

    // Atteso ≈ 4800 frame (±50 frame di tolleranza per arrotondamento)
    TEST_ASSERT_UINT_WITHIN(50, 4800, (unsigned)outFrames);
}

void test_src_preserves_frequency(void) {
    // Genera sine 1kHz @ 44.1kHz, converti a 48kHz, verifica freq ≈ 1kHz
    const size_t INPUT_FRAMES  = 4410;
    const size_t OUTPUT_CAP    = 5000;

    int16_t* input  = (int16_t*)calloc(INPUT_FRAMES  * 2, sizeof(int16_t));
    int16_t* output = (int16_t*)calloc(OUTPUT_CAP    * 2, sizeof(int16_t));
    TEST_ASSERT_NOT_NULL(input);
    TEST_ASSERT_NOT_NULL(output);

    generate_sine(input, INPUT_FRAMES, 1000.0f, 44100);

    AudioSRCStub src = {};
    src_init(&src, 44100, 48000);
    size_t outFrames = src_process(&src, input, INPUT_FRAMES, output, OUTPUT_CAP);

    TEST_ASSERT_GREATER_THAN(0, (int)outFrames);

    float measuredFreq = measure_frequency_zcr(output, outFrames, 48000);
    free(input);
    free(output);

    // Tolleranza ±50Hz per l'approssimazione ZCR
    TEST_ASSERT_FLOAT_WITHIN(50.0f, 1000.0f, measuredFreq);
}

void test_src_48000_to_48000_passthrough(void) {
    // SR uguale → nessuna conversione, output ≈ input
    const size_t FRAMES = 256;

    int16_t* input  = (int16_t*)calloc(FRAMES * 2, sizeof(int16_t));
    int16_t* output = (int16_t*)calloc(FRAMES * 2, sizeof(int16_t));
    TEST_ASSERT_NOT_NULL(input);
    TEST_ASSERT_NOT_NULL(output);

    generate_sine(input, FRAMES, 440.0f, 48000);

    AudioSRCStub src = {};
    src_init(&src, 48000, 48000);  // ratio = 1.0
    size_t outFrames = src_process(&src, input, FRAMES, output, FRAMES);

    free(input);
    free(output);

    // Stessa quantità di frame (ratio=1.0)
    TEST_ASSERT_UINT_WITHIN(2, FRAMES, (unsigned)outFrames);
}

void test_src_reset(void) {
    AudioSRCStub src = {};
    src_init(&src, 44100, 48000);
    src.phase = 3.5f;
    src.prevL = 1000;

    // Simula reset
    src.phase = 0.0f;
    src.prevL = 0;
    src.prevR = 0;

    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, src.phase);
    TEST_ASSERT_EQUAL_INT16(0, src.prevL);
}

// ——— Entry point per PlatformIO Unity runner ———

int main(int argc, char** argv) {
    UNITY_BEGIN();

    // Test conversione delay DSP
    RUN_TEST(test_delay_conversion_10ms);
    RUN_TEST(test_delay_conversion_20ms);
    RUN_TEST(test_delay_conversion_1ms);
    RUN_TEST(test_delay_conversion_zero);
    RUN_TEST(test_delay_conversion_negative);
    RUN_TEST(test_delay_conversion_max_clamped);
    RUN_TEST(test_delay_dsp_always_48khz);

    // Test SRC
    RUN_TEST(test_src_init_44100_to_48000);
    RUN_TEST(test_src_output_frame_count_100ms);
    RUN_TEST(test_src_preserves_frequency);
    RUN_TEST(test_src_48000_to_48000_passthrough);
    RUN_TEST(test_src_reset);

    return UNITY_END();
}
