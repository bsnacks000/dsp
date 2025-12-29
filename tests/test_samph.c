#include "test_samph.h"
#include <dsp/samph.h>

#include <stdio.h>
#include "helpers.h"

#define NSMPS 64

MunitResult test_samph_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    samph sh;
    samph_init(&sh, 0.5);

    return MUNIT_OK;
}

MunitResult test_samph_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    samph sh;
    samph_init(&sh, 0.5);

    float out[NSMPS] = {0.0};
    float in[NSMPS] = {0.0};
    float gate[NSMPS] = {0.0};
    float gate_thresh[NSMPS] = {0.0};

    fill_random_bipolar_signal(in, NSMPS);
    fill_dc(gate_thresh, 0.5, NSMPS);

    // trigger gate half way through cycle
    for (int i = NSMPS / 2; i < NSMPS; i++)
        gate[i] = 1.0;

    samph_tick_block(&sh, out, in, gate, gate_thresh, 0, NSMPS);

    for (int i = 0; i < NSMPS / 2; i++)
        munit_assert_double_equal(out[i], 0.0, 6);

    for (int i = NSMPS / 2; i < NSMPS; i++)
        munit_assert_double(out[i], !=, 0.0);

    return MUNIT_OK;
}
