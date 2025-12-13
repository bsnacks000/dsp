#include "test_curve.h"

#include <dsp/curve.h>
#include "helpers.h"

#define NSMPS 128

/**
 * TODO: add stability tests to these smoke tests
 */

static inline float ms_per_samps(float nsmps, float sr) {
    return nsmps / sr * 1000.0;
}

MunitResult test_curve_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float sr = 48000.0;
    float ms = ms_per_samps(NSMPS, sr);
    curve c;
    curve_init(&c, 0.0, 1.0, ms, 0.0, sr);

    return MUNIT_OK;
}

MunitResult test_curve_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float sr = 48000.0;
    float ms = ms_per_samps(NSMPS, sr);
    curve c;
    curve_init(&c, 0.0, 1.0, ms / 1000.0, 0.0, sr);

    float out[NSMPS] = {0};
    curve_tick_block(&c, out, NSMPS);

    return MUNIT_OK;
}

MunitResult test_ar_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float sr = 48000.0;
    curve_ar env;
    curve_ar_init(&env, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, sr);

    return MUNIT_OK;
}

MunitResult test_ar_tick_block(const MunitParameter params[], void* data) {
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

    float atk_crv[NSMPS] = {0};
    fill_dc(atk_crv, 0.5, NSMPS);

    float atk_level[NSMPS] = {0};
    fill_dc(atk_level, 1.0, NSMPS);

    float rel_sec[NSMPS] = {0};
    fill_dc(rel_sec, block_total_dur_sec / 2.0, NSMPS);

    float rel_crv[NSMPS] = {0};
    fill_dc(rel_crv, 0.5, NSMPS);

    float rel_level[NSMPS] = {0};
    fill_dc(rel_level, 0.0, NSMPS);

    curve_ar env;
    curve_ar_init(&env, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, sr);

    curve_ar_tick_block(&env, out, gate, gate_thresh, start_level, atk_sec, atk_crv,
                        atk_level, rel_sec, rel_crv, rel_level, NSMPS);

    return MUNIT_OK;
}

MunitResult test_adsr_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float sr = 48000.0;
    curve_adsr env;
    curve_adsr_init(&env, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, sr);

    return MUNIT_OK;
}

MunitResult test_adsr_tick_block(const MunitParameter params[], void* data) {
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

    float atk_crv[NSMPS] = {0};
    fill_dc(atk_crv, 0.5, NSMPS);

    float atk_level[NSMPS] = {0};
    fill_dc(atk_level, 1.0, NSMPS);

    float dcy_sec[NSMPS] = {0};
    fill_dc(dcy_sec, block_total_dur_sec / 4.0, NSMPS);

    float dcy_crv[NSMPS] = {0};
    fill_dc(dcy_crv, block_total_dur_sec / 4.0, NSMPS);

    float sustain_level[NSMPS] = {0};
    fill_dc(sustain_level, 0.75, NSMPS);

    float rel_sec[NSMPS] = {0};
    fill_dc(rel_sec, block_total_dur_sec / 2.0, NSMPS);

    float rel_crv[NSMPS] = {0};
    fill_dc(rel_crv, 0.5, NSMPS);

    float rel_level[NSMPS] = {0};
    fill_dc(rel_level, 0.0, NSMPS);

    curve_adsr env;
    curve_adsr_init(&env, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, sr);

    curve_adsr_tick_block(&env, out, gate, gate_thresh, start_level, atk_sec, atk_crv,
                          atk_level, dcy_sec, dcy_crv, sustain_level, rel_sec, rel_crv,
                          rel_level, NSMPS);

    return MUNIT_OK;
}
