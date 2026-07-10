#include "test_line.h"

#include <dsp/line.h>
#include "helpers.h"

#define NSMPS 128

static inline float ms_per_samps(float nsmps, float sr) {
    return nsmps / sr * 1000.0;
}

MunitResult test_line_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float sr = 48000.0;
    float ms = ms_per_samps(NSMPS, sr);
    line l;
    line_init(&l, 0.0, 1.0, ms, sr);

    return MUNIT_OK;
}

MunitResult test_line_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float sr = 48000;
    float ms = ms_per_samps(NSMPS, sr);
    line l;
    line_init(&l, 0.0, 1.0, ms / 1000.0, sr);

    float out[NSMPS] = {0};
    line_tick_block(&l, out, 0, NSMPS);

    return MUNIT_OK;
}

MunitResult test_line_ar_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float sr = 48000.0;
    line_ar env;
    line_ar_init(&env, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, sr);

    return MUNIT_OK;
}

MunitResult test_line_ar_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float sr = NSMPS;  // set this equal
    float block_total_dur_sec = ms_per_samps(NSMPS, sr) / 1000.0;

    float out[NSMPS] = {0};

    float gate[NSMPS] = {0};
    fill_dc(gate, 0.5, NSMPS);

    float gate_thresh[NSMPS] = {0};
    fill_dc(gate_thresh, 0.5, NSMPS);

    float start_level[NSMPS] = {0};

    float atk_sec[NSMPS] = {0};
    fill_dc(atk_sec, block_total_dur_sec / 2.0, NSMPS);

    float atk_level[NSMPS] = {0};
    fill_dc(atk_level, 1.0, NSMPS);

    float rel_sec[NSMPS] = {0};
    fill_dc(rel_sec, block_total_dur_sec / 2.0, NSMPS);

    float rel_level[NSMPS] = {0};
    fill_dc(rel_level, 0.0, NSMPS);

    line_ar env;
    line_ar_init(&env, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, sr);

    line_ar_tick_block(&env, out, gate, gate_thresh, start_level, atk_sec, atk_level,
                       rel_sec, rel_level, 0, NSMPS);

    return MUNIT_OK;
}

MunitResult test_line_adsr_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float sr = 48000.0;
    line_adsr env;
    line_adsr_init(&env, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, sr);

    return MUNIT_OK;
}

MunitResult test_line_adsr_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float sr = NSMPS;  // set this equal
    float block_total_dur_sec = ms_per_samps(NSMPS, sr) / 1000.0;

    float out[NSMPS] = {0};

    float gate[NSMPS] = {0};
    fill_dc(gate, 0.5, NSMPS);

    float gate_thresh[NSMPS] = {0};
    fill_dc(gate_thresh, 0.5, NSMPS);

    float start_level[NSMPS] = {0};

    float atk_sec[NSMPS] = {0};
    fill_dc(atk_sec, block_total_dur_sec / 4.0, NSMPS);

    float atk_level[NSMPS] = {0};
    fill_dc(atk_level, 1.0, NSMPS);

    float dcy_sec[NSMPS] = {0};
    fill_dc(dcy_sec, block_total_dur_sec / 4.0, NSMPS);

    float sustain_level[NSMPS] = {0};
    fill_dc(sustain_level, 0.75, NSMPS);

    float rel_sec[NSMPS] = {0};
    fill_dc(rel_sec, block_total_dur_sec / 2.0, NSMPS);

    float rel_level[NSMPS] = {0};
    fill_dc(rel_level, 0.0, NSMPS);

    line_adsr env;
    line_adsr_init(&env, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, sr);

    // NOTE: 50 here will capture the release stage but we need seperate test
    // cases to capture retrigger events
    for (int i = 0; i < 100; i++) {
        if (i >= 50) {
            fill_dc(gate, 0.0, NSMPS);
        }
        line_adsr_tick_block(&env, out, gate, gate_thresh, start_level, atk_sec,
                             atk_level, dcy_sec, sustain_level, rel_sec, rel_level, 0,
                             NSMPS);
    }

    return MUNIT_OK;
}

MunitResult test_sampi(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    sampi state;
    float sr = 4800.0f;
    float start = -1.0f;
    float stop = 1.0f;
    float dur_sec_start = 0.01f;

    float block_total_dur_sec = ms_per_samps(NSMPS, sr) / 1000.0;

    // inits a line under the hood
    sampi_init(&state, start, stop, dur_sec_start, sr);

    float gate[NSMPS] = {0};
    fill_dc(gate, 0.5, NSMPS);

    float gate_thresh[NSMPS] = {0};
    fill_dc(gate_thresh, 0.5, NSMPS);

    float out[NSMPS] = {0};
    fill_dc(out, 0.0, NSMPS);

    float in[NSMPS] = {0};
    fill_random_bipolar_signal(in, NSMPS);

    float dur_sec[NSMPS] = {0};
    fill_dc(dur_sec, block_total_dur_sec / 2.0, NSMPS);

    sampi_tick_block(&state, out, in, dur_sec, gate, gate_thresh, 0, NSMPS);

    return MUNIT_OK;
}
