#include "test_rc.h"
#include "helpers.h"

#include <dsp/rc.h>

#define NSMPS 64

MunitResult test_rc_one_pole_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    rc_one_pole rc;

    float sr = 48000.0;
    float freq = 440.0;

    rc_one_pole_init(&rc, freq, sr);

    // check variables and initial update

    munit_assert_double_equal(rc.sr, sr, 7);
    munit_assert_double_equal(rc.t_, 1.0 / sr, 7);
    munit_assert_double_equal(rc.t_over_2_, (1.0 / sr) / 2.0, 7);
    munit_assert_double_equal(rc.two_over_t_, (2.0 / (1.0 / sr)), 7);

    // g should be set in update
    munit_assert_double(rc.g_, !=, 0.0);

    return MUNIT_OK;
}

MunitResult test_rc_one_pole_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    rc_one_pole rc;
    rc_one_pole_init(&rc, 440.0, 48000.0);

    float freq[NSMPS];
    for (int i = 0; i < NSMPS; i++)
        freq[i] = 440.0;

    float in[NSMPS];
    fill_impulse_response(in, NSMPS);

    float out_lp[NSMPS] = {0.0};
    float out_hp[NSMPS] = {0.0};
    float out_ap[NSMPS] = {0.0};

    rc_one_pole_tick_block(&rc, in, out_lp, out_hp, out_ap, freq, NSMPS);

    check_finite(out_lp, NSMPS);
    check_any_nonzero(out_lp, NSMPS);

    check_finite(out_hp, NSMPS);
    check_any_nonzero(out_hp, NSMPS);

    check_finite(out_ap, NSMPS);
    check_any_nonzero(out_ap, NSMPS);

    return MUNIT_OK;
}

MunitResult test_rc_ladder_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float sr = 48000.0;
    float freq = 440.0;
    float q = 0.707;

    rc_ladder lad;
    rc_ladder_init(&lad, freq, q, sr);

    munit_assert_double_equal(lad.sr, sr, 7);
    munit_assert_double_equal(lad.t_, 1.0 / sr, 7);
    munit_assert_double_equal(lad.t_over_2_, (1.0 / sr) / 2.0, 7);
    munit_assert_double_equal(lad.two_over_t_, (2.0 / (1.0 / sr)), 7);

    // assert the one pole bank is initialized
    munit_assert_double(lad.bank_[0].g_, !=, 0.0);
    munit_assert_double(lad.bank_[1].g_, !=, 0.0);
    munit_assert_double(lad.bank_[2].g_, !=, 0.0);
    munit_assert_double(lad.bank_[3].g_, !=, 0.0);

    return MUNIT_OK;
}

MunitResult test_rc_ladder_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    rc_ladder lad;
    rc_ladder_init(&lad, 440.0, 0.707, 48000.0);

    float in[NSMPS];
    fill_impulse_response(in, NSMPS);

    float freq[NSMPS];
    for (int i = 0; i < NSMPS; i++)
        freq[i] = 440.0;

    float q[NSMPS];
    for (int i = 0; i < NSMPS; i++)
        q[i] = 0.707;

    float out[NSMPS] = {0.0};

    rc_ladder_tick_block(&lad, out, in, freq, q, NSMPS);

    munit_assert_memory_not_equal(sizeof(float) * NSMPS, out, in);
    check_finite(out, NSMPS);
    check_energy(out, 10.0, NSMPS);
    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}
