#include "test_bq.h"
#include <dsp/bq.h>
#include "helpers.h"

#include <stdio.h>

#define NSMPS 64

MunitResult test_dfII_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    dfII d;
    dfII_init(&d, 48000.0);

    munit_assert_float(d.sr, ==, 48000.0);
    munit_assert_float(d.a0, ==, 0.0);
    munit_assert_float(d.a1, ==, 0.0);
    munit_assert_float(d.a2, ==, 0.0);
    munit_assert_float(d.b1, ==, 0.0);
    munit_assert_float(d.b2, ==, 0.0);
    munit_assert_float(d.c0, ==, 1.0);
    munit_assert_float(d.d0, ==, 0.0);
    munit_assert_float(d.x_z1, ==, 0.0);
    munit_assert_float(d.x_z2, ==, 0.0);

    return MUNIT_OK;
}

MunitResult test_dfII_tick(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    dfII d;
    dfII_init(&d, 44100.0);

    // basic lowpass from https://www.earlevel.com/main/2021/09/02/biquad-calculator-v3/
    // fc=100 Q=0.70716  dbGain=6

    d.a0 = 0.00005024141818873903;
    d.a1 = 0.00010048283637747806;
    d.a2 = 0.00005024141818873903;

    d.b1 = -1.979851353142371;
    d.b2 = 0.9800523188151258;

    float sig_in[NSMPS];

    fill_impulse_response(sig_in, NSMPS);

    float sig_out[NSMPS] = {0.0};

    for (uint32_t i = 0; i < NSMPS; i++) {
        sig_out[i] = dfII_tick(&d, sig_in[i]);
    }

    munit_assert_memory_not_equal(sizeof(float) * NSMPS, sig_out, sig_in);

    check_finite(sig_out, NSMPS);
    check_energy(sig_out, 10.0, NSMPS);
    check_any_nonzero(sig_out, NSMPS);

    // for (int i = 0; i < NSMPS; i++)
    //     printf("%.6f, ", sig_out[i]);

    return MUNIT_OK;
}

// bq nonresonant - first order filters / butterworth / linkwitz-riley
MunitResult test_bq_non_resonant_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    bq_non_resonant f;

    bq_non_resonant_init(&f, BQ_FIRST_ORDER_LPF, 100.0, 48000.0);

    munit_assert_float(f.sr, ==, 48000.0);
    munit_assert_double_equal(f.state_.a0, 0.006502518, 5);
    munit_assert_double_equal(f.state_.a1, 0.006502518, 5);
    munit_assert_float(f.state_.a2, ==, 0.0);
    munit_assert_double_equal(f.state_.b1, -0.986994962, 5);
    munit_assert_float(f.state_.b2, ==, 0.0);
    munit_assert_float(f.state_.c0, ==, 1.0);
    munit_assert_float(f.state_.d0, ==, 0.0);
    munit_assert_float(f.state_.x_z1, ==, 0.0);
    munit_assert_float(f.state_.x_z2, ==, 0.0);

    return MUNIT_OK;
}

// private smoke test wrappers
// simple stability tests against impulse response

static void smoke_test_non_resonant_bq(bq_non_resonant* f, bq_non_resonant_type t) {

    bq_non_resonant_init(f, t, 100.0, 48000.0);

    float in[NSMPS];

    fill_impulse_response(in, NSMPS);

    float out[NSMPS] = {0.0};

    bq_non_resonant_tick_block(f, out, in, 100.0, NSMPS);

    check_finite(out, NSMPS);
    check_any_nonzero(out, NSMPS);
}

static void smoke_test_resonant_bq(bq_resonant* f, bq_resonant_type t) {

    bq_resonant_init(f, t, 100.0, 0.707, 48000.0);

    float in[NSMPS];

    fill_impulse_response(in, NSMPS);

    float out[NSMPS] = {0.0};

    bq_resonant_tick_block(f, out, in, 100.0, 0.707, NSMPS);

    check_finite(out, NSMPS);
    check_any_nonzero(out, NSMPS);
}

static void smoke_test_bq_para_eq(bq_para_eq* f, bq_para_eq_type t) {
    bq_para_eq_init(f, t, 100.0, 0.707, 1.0, 48000.0);
    float in[NSMPS];

    fill_impulse_response(in, NSMPS);

    float out[NSMPS] = {0.0};

    bq_para_eq_tick_block(f, out, in, 100.0, 0.707, 1.0, NSMPS);

    check_finite(out, NSMPS);
    check_any_nonzero(out, NSMPS);
}

MunitResult test_bq_non_resonant_tick_block_lpf(const MunitParameter params[],
                                                void* data) {
    (void) params;
    (void) data;

    bq_non_resonant f;
    smoke_test_non_resonant_bq(&f, BQ_FIRST_ORDER_LPF);

    return MUNIT_OK;
}

MunitResult test_bq_non_resonant_tick_block_hpf(const MunitParameter params[],
                                                void* data) {
    (void) params;
    (void) data;

    bq_non_resonant f;
    smoke_test_non_resonant_bq(&f, BQ_FIRST_ORDER_HPF);

    return MUNIT_OK;
}

MunitResult test_bq_non_resonant_tick_block_apf(const MunitParameter params[],
                                                void* data) {
    (void) params;
    (void) data;

    bq_non_resonant f;
    smoke_test_non_resonant_bq(&f, BQ_FIRST_ORDER_APF);

    return MUNIT_OK;
}

MunitResult test_bq_non_resonant_tick_block_butterworth(const MunitParameter params[],
                                                        void* data) {
    (void) params;
    (void) data;

    bq_non_resonant f;
    smoke_test_non_resonant_bq(&f, BQ_BUTTERWORTH);

    return MUNIT_OK;
}

MunitResult test_bq_non_resonant_tick_block_linkwitzriley(const MunitParameter params[],
                                                          void* data) {
    (void) params;
    (void) data;

    bq_non_resonant f;
    smoke_test_non_resonant_bq(&f, BQ_LINKWITZ_RILEY);

    return MUNIT_OK;
}

// // bq resonant - 2nd order filters + smith-angell reson
MunitResult test_bq_resonant_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    bq_resonant f;

    bq_resonant_init(&f, BQ_SECOND_ORDER_LPF, 100.0, 0.707, 48000.0);

    munit_assert_float(f.sr, ==, 48000.0);
    munit_assert_double_equal(f.state_.a0, 0.000042443309351420545, 7);
    munit_assert_double_equal(f.state_.a1, 0.00008488661870284109, 7);
    munit_assert_double_equal(f.state_.a2, 0.000042443309351420545, 7);
    munit_assert_double_equal(f.state_.b1, -1.9814857645620922, 7);
    munit_assert_double_equal(f.state_.b2, 0.9816555377994975, 7);
    munit_assert_float(f.state_.c0, ==, 1.0);
    munit_assert_float(f.state_.d0, ==, 0.0);
    munit_assert_float(f.state_.x_z1, ==, 0.0);
    munit_assert_float(f.state_.x_z2, ==, 0.0);

    return MUNIT_OK;
}

MunitResult test_bq_resonant_tick_block_lpf(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    bq_resonant f;
    smoke_test_resonant_bq(&f, BQ_SECOND_ORDER_LPF);

    return MUNIT_OK;
}

MunitResult test_bq_resonant_tick_block_hpf(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    bq_resonant f;
    smoke_test_resonant_bq(&f, BQ_SECOND_ORDER_HPF);

    return MUNIT_OK;
}

MunitResult test_bq_resonant_tick_block_bpf(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    bq_resonant f;
    smoke_test_resonant_bq(&f, BQ_SECOND_ORDER_BPF);

    return MUNIT_OK;
}

MunitResult test_bq_resonant_tick_block_bsf(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    bq_resonant f;
    smoke_test_resonant_bq(&f, BQ_SECOND_ORDER_BSF);

    return MUNIT_OK;
}

MunitResult test_bq_resonant_tick_block_reson(const MunitParameter params[],
                                              void* data) {
    (void) params;
    (void) data;

    bq_resonant f;
    smoke_test_resonant_bq(&f, BQ_SECOND_ORDER_RESON);

    return MUNIT_OK;
}

MunitResult test_bq_resonant_tick_block_apf(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    bq_resonant f;
    smoke_test_resonant_bq(&f, BQ_SECOND_ORDER_APF);

    return MUNIT_OK;
}

// // parametric filters
MunitResult test_bq_para_eq_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    bq_para_eq f;

    bq_para_eq_init(&f, BQ_PARA_LSHLF, 100.0, 0.7071, 6.0, 48000.0);

    // TODO: need to check these coeffs ... :(
    munit_assert_float(f.sr, ==, 48000.0);
    // munit_assert_double_equal(f.state_.a0, 1.0064717117739082, 7);
    // munit_assert_double_equal(f.state_.a1, -0.9805232509076435, 7);
    // munit_assert_double_equal(f.state_.a2, 0.0, 7);
    // munit_assert_double_equal(f.state_.b1, -0.9869949626815515, 7);
    // munit_assert_double_equal(f.state_.b2, 0.0, 7);
    // munit_assert_float(f.state_.c0, ==, 1.0);
    // munit_assert_float(f.state_.d0, ==, 0.0);
    // munit_assert_float(f.state_.x_z1, ==, 0.0);
    // munit_assert_float(f.state_.x_z2, ==, 0.0);

    return MUNIT_OK;
}

MunitResult test_bq_para_eq_tick_block_lshlf(const MunitParameter params[],
                                             void* data) {
    (void) params;
    (void) data;

    bq_para_eq f;
    smoke_test_bq_para_eq(&f, BQ_PARA_LSHLF);

    return MUNIT_OK;
}

MunitResult test_bq_para_eq_tick_block_hshlf(const MunitParameter params[],
                                             void* data) {
    (void) params;
    (void) data;

    bq_para_eq f;
    smoke_test_bq_para_eq(&f, BQ_PARA_HSHLF);

    return MUNIT_OK;
}

MunitResult test_bq_para_eq_tick_block_peak(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    bq_para_eq f;
    smoke_test_bq_para_eq(&f, BQ_PARA_PEAK);

    return MUNIT_OK;
}
