#include "test_follow.h"

#include "dsp/maths.h"
#include "helpers.h"

#include <dsp/follow.h>

#define NSMPS 64

MunitResult test_follow_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    follow filt;

    follow_init(&filt, 0.0, 0.0, 48000.0);

    return MUNIT_OK;
}
MunitResult test_follow_peak_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float out[NSMPS] = {0.0};
    float in[NSMPS] = {0.0};
    float atk_ms[NSMPS] = {0.0};
    float rel_ms[NSMPS] = {0.0};

    fill_impulse_response(in, NSMPS);
    fill_dc(atk_ms, 10.0, NSMPS);
    fill_dc(rel_ms, 10.0, NSMPS);

    follow filt;
    follow_init(&filt, 0.0, 0.0, 48000.0);
    follow_peak_tick_block(&filt, out, in, atk_ms, rel_ms, 0, NSMPS);

    check_energy(out, 10.0, NSMPS);

    return MUNIT_OK;
}

MunitResult test_follow_rms_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float out[NSMPS] = {0.0};
    float in[NSMPS] = {0.0};
    float atk_ms[NSMPS] = {0.0};
    float rel_ms[NSMPS] = {0.0};

    fill_impulse_response(in, NSMPS);
    fill_dc(atk_ms, 10.0, NSMPS);
    fill_dc(rel_ms, 10.0, NSMPS);

    follow filt;
    follow_init(&filt, 0.0, 0.0, 48000.0);
    follow_rms_tick_block(&filt, out, in, atk_ms, rel_ms, 0, NSMPS);

    check_energy(out, 10.0, NSMPS);

    return MUNIT_OK;
}
