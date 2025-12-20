#include <dsp/phasor.h>

#include "helpers.h"
#include "test_phasor.h"

#define NSMPS 64

#include <stdio.h>

MunitResult test_phasor_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    phasor ph;
    phasor_init(&ph, 440.0, 0.0, 48000.0);

    float out[NSMPS] = {0.0};
    float freq[NSMPS] = {0.0};

    fill_dc(freq, 120.0, NSMPS);

    phasor_tick_block(&ph, out, freq, NSMPS);

    check_any_nonzero(out, NSMPS);
    check_energy(out, 1.2, NSMPS);
    return MUNIT_OK;
}

MunitResult test_impulse_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    phasor ph;
    phasor_init(&ph, 440.0, 0.0, 48000.0);

    float out[NSMPS] = {0.0};
    float freq[NSMPS] = {0.0};

    fill_dc(freq, 120.0, NSMPS);

    // for (int i = 0; i < 10; i++)
    impulse_tick_block(&ph, out, freq, NSMPS);

    // for (int i = 0; i < NSMPS; i++)
    //     printf("%.3f", out[i]);

    // first sample should always be 1
    munit_assert_double_equal(out[0], 1.0, 6);

    for (int i = 1; i < NSMPS; i++)
        munit_assert_double_equal(out[i], 0.0, 6);
    return MUNIT_OK;
}

MunitResult test_rand_impulse_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    phasor ph;
    phasor_init(&ph, 440.0, 0.0, 48000.0);

    float out[NSMPS] = {0.0};
    float freq[NSMPS] = {0.0};

    fill_dc(freq, 880.0, NSMPS);

    rand_impulse_tick_block(&ph, out, freq, NSMPS);

    check_energy(out, 1.2, NSMPS);
    return MUNIT_OK;
}
