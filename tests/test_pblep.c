#include "test_pblep.h"
#include <dsp/pblep.h>
#include "helpers.h"

#define NSMPS 64

MunitResult test_blepsaw(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    blepsaw osc;
    float sr = 48000.0;
    blepsaw_init(&osc, 120.0, 0.5, sr);

    float out[NSMPS] = {0.0};
    float freq[NSMPS] = {0.0};

    fill_dc(freq, 440.0, NSMPS);

    blepsaw_tick_block(&osc, out, freq, 0, NSMPS);

    return MUNIT_OK;
}

MunitResult test_blepsqr(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    blepsqr osc;
    float sr = 48000.0;
    blepsqr_init(&osc, 120.0, 0.1, 0.0, sr);

    float out[NSMPS] = {0.0};
    float freq[NSMPS] = {0.0};
    float duty[NSMPS] = {0.0};

    fill_dc(freq, 440.0, NSMPS);
    fill_dc(duty, 0.5, NSMPS);

    blepsqr_tick_block(&osc, out, freq, duty, 0, NSMPS);

    return MUNIT_OK;
}
