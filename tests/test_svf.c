#include "test_svf.h"
#include <dsp/svf.h>
#include "helpers.h"

#define NSMPS 64

MunitResult test_svf_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float freq = 440.0;
    float drive = 0.0;
    float q = 0.707;
    float sr = 48000.0;

    svf f;
    svf_init(&f, freq, q, drive, sr);

    // check freq warping calcs
    munit_assert_double_equal(f.sr, sr, 7);
    munit_assert_double_equal(f.t_, 1.0 / sr, 7);
    munit_assert_double_equal(f.t_ovr_2_, (1.0 / sr) / 2.0, 7);
    munit_assert_double_equal(f.two_ovr_t_, (2.0 / (1.0 / sr)), 7);

    munit_assert_double(f.g_, !=, 0.0);

    munit_assert_double(f.g1_, !=, 0.0);
    munit_assert_double(f.two_r_, !=, 0.0);
    munit_assert_double(f.four_r_, !=, 0.0);
    munit_assert_double(f.two_r_plus_g_, !=, 0.0);

    return MUNIT_OK;
}

MunitResult test_svf_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    svf f;
    svf_init(&f, 440.0, 0.707, 0.2, 48000.0);

    float freq[NSMPS];
    for (int i = 0; i < NSMPS; i++)
        freq[i] = 440.0;

    float q[NSMPS];
    for (int i = 0; i < NSMPS; i++)
        q[i] = 0.707;

    float drive[NSMPS];
    for (int i = 0; i < NSMPS; i++)
        drive[i] = 0.2;

    float in[NSMPS];
    fill_impulse_response(in, NSMPS);

    float out_lp[NSMPS] = {0.0};
    float out_hp[NSMPS] = {0.0};
    float out_bp[NSMPS] = {0.0};
    float out_bs[NSMPS] = {0.0};
    float out_ap[NSMPS] = {0.0};

    svf_tick_block(&f, out_lp, out_hp, out_bp, out_bs, out_ap, in, freq, q, drive,
                   NSMPS);

    check_finite(out_lp, NSMPS);
    check_energy(out_lp, 10.0, NSMPS);
    check_any_nonzero(out_lp, NSMPS);

    check_finite(out_bs, NSMPS);
    check_energy(out_bs, 10.0, NSMPS);
    check_any_nonzero(out_bs, NSMPS);

    check_finite(out_bp, NSMPS);
    check_energy(out_bp, 10.0, NSMPS);
    check_any_nonzero(out_bp, NSMPS);

    check_finite(out_hp, NSMPS);
    check_energy(out_hp, 10.0, NSMPS);
    check_any_nonzero(out_hp, NSMPS);

    check_finite(out_ap, NSMPS);
    check_energy(out_ap, 10.0, NSMPS);
    check_any_nonzero(out_ap, NSMPS);

    return MUNIT_OK;
}
