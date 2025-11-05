#include <dsp/balance.h>

#include "munit.h"

#include <math.h>
#include <string.h>

#define NSMPS 64

static void fill_random_bipolar_signal(float* buf, uint32_t nsmps) {
    for (uint32_t i = 0; i < nsmps; i++) {
        float r = (float) munit_rand_double();
        buf[i] = 2.0 * r - 1.0;
    }
}

static void copy_buf(float* dest, float* src, uint32_t nsmps) {
    memcpy(dest, src, sizeof(float) * nsmps);
}

static void scale_buf(float* buf, float val, uint32_t nsmps) {
    for (uint32_t i = 0; i < nsmps; i++) {
        buf[i] *= val;
    }
}

MunitResult test_balance_init(const MunitParameter params[], void* data) {

    (void) params;
    (void) data;

    balance b;
    balance_init(&b, 48000.0f);

    // check all values are set and update is run
    munit_assert_float(b.sr, ==, 48000.0f);
    munit_assert_float(b.d0_, ==, 0.0);
    munit_assert_float(b.d1_, ==, 0.0);
    munit_assert_float(b.scale_, ==, 0.0);

    // update should trigger this calc
    munit_assert_double_equal(b.coef_, -0.998692, 5);

    // assert negative sr is handled on init
    balance_init(&b, -48000.0f);
    munit_assert_float(b.sr, ==, 48000.0f);

    // test reinit
    b.d0_ = 42.0;
    b.d1_ = 42.0;
    b.scale_ = 42.0;
    b.coef_ = 42.0;

    balance_init(&b, 48000.0);

    // reset handled ..
    munit_assert_float(b.sr, ==, 48000.0f);
    munit_assert_float(b.d0_, ==, 0.0);
    munit_assert_float(b.d1_, ==, 0.0);
    munit_assert_float(b.scale_, ==, 0.0);

    munit_assert_double_equal(b.coef_, -0.998692, 5);

    return MUNIT_OK;
}

MunitResult test_balance_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float out[NSMPS];
    float out_copy[NSMPS];
    float cmp[NSMPS];

    fill_random_bipolar_signal(out, NSMPS);
    scale_buf(out, 0.5, NSMPS);
    copy_buf(out_copy, out, NSMPS);
    fill_random_bipolar_signal(cmp, NSMPS);

    balance b;
    balance_init(&b, 48000.0f);

    balance_tick_block(&b, out, cmp, NSMPS);

    // check output was processed
    munit_assert_memory_not_equal(sizeof(float) * NSMPS, out_copy, out);

    // check for gremlins
    for (uint32_t i = 0; i < NSMPS; i++) {
        munit_assert(!isnan(out[i]));
        munit_assert(!isinf(out[i]));
    }

    return MUNIT_OK;
}
