#include "test_lag.h"
#include "helpers.h"

#include <dsp/lag.h>

#define NSMPS 64

MunitResult test_lag_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    lag filt;
    float sr = 48000.0;
    lag_init(&filt, 0.0, sr);

    // alpha should be calculated safely even if seconds = 0.0;
    munit_assert_double(filt.alpha_, !=, 0.0);
    return MUNIT_OK;
}

MunitResult test_lag_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float out[NSMPS] = {0.0};
    float in[NSMPS] = {0.0};
    float t_sec[NSMPS] = {0.0};

    fill_dc(t_sec, 0.1, NSMPS);
    fill_impulse_response(in, NSMPS);

    float sr = 48000.0;
    lag filt;
    lag_init(&filt, 0.0, sr);

    lag_tick_block(&filt, out, in, t_sec, 0, NSMPS);

    check_energy(out, 1.0, NSMPS);

    return MUNIT_OK;
}
