#include "test_utils.h"
#include <dsp/utils.h>

MunitResult test_wavetable_cubic_guardpoint(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf[6] = {1.0, 2.0, 3.0, 4.0, 0.0, 0.0};
    wavetable_cubic_guardpoint(buf, 4);

    float expected[6] = {1.0, 2.0, 3.0, 4.0, 1.0, 2.0};
    munit_assert_memory_equal(6, buf, expected);
    return MUNIT_OK;
}

MunitResult test_branchless_float_wrap_range(const MunitParameter params[],
                                             void* data) {
    (void) params;
    (void) data;

    float x = wrap_float_range(-42.12345, -1.0, 1.0);
    munit_assert_float(x, <=, 1.0);
    munit_assert_float(x, >=, -1.0);

    return MUNIT_OK;
}

MunitResult test_branchless_float_wrap_positive(const MunitParameter params[],
                                                void* data) {
    (void) params;
    (void) data;
    float x = wrap_float_positive(-42.12345, 1.0);
    munit_assert_float(x, <=, 1.0);
    munit_assert_float(x, >=, 0.0);

    return MUNIT_OK;
}

MunitResult test_ceiling_pow2(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;
    float x = ceiling_pow2(7.0);
    munit_assert_float(x, ==, 8.0);

    x = ceiling_pow2(513.0);
    munit_assert_float(x, ==, 1024);

    return MUNIT_OK;
}

MunitResult test_check_float_equal(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    bool result = check_float_equal(1.0, 2.0);
    munit_assert_false(result);

    result = check_float_equal(1.0, 1.0);
    munit_assert_true(result);

    return MUNIT_OK;
}

MunitResult test_has_fractional_part(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    bool result = has_fractional_part(1.0);
    munit_assert_false(result);

    result = has_fractional_part(1.123);
    munit_assert_true(result);

    return MUNIT_OK;
}

MunitResult test_nsmps_dur(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float sr = 48000.0f;
    float dur = 1.5f;
    float expected = sr * dur;

    float result = nsmps_dur(sr, dur);
    munit_assert_float(result, ==, expected);

    // negative duration should yield same positive result
    float result_neg = nsmps_dur(sr, -dur);
    munit_assert_float(result_neg, ==, expected);

    return MUNIT_OK;
}

MunitResult test_copy_nsmps(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    uint32_t nsmps = 4;
    float in[4];
    for (uint32_t i = 0; i < nsmps; i++) {
        in[i] = (float) (i + 1);
    }
    float out[4] = {0};

    copy_nsmps(out, in, 0, nsmps);

    for (uint32_t i = 0; i < nsmps; i++) {
        munit_assert_float(out[i], ==, in[i]);
    }

    return MUNIT_OK;
}

MunitResult test_set_nsmps(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    uint32_t nsmps = 5;
    float buf[5];
    float val = -3.25f;

    set_nsmps(buf, val, nsmps);

    for (uint32_t i = 0; i < nsmps; i++) {
        munit_assert_float(buf[i], ==, val);
    }

    return MUNIT_OK;
}

MunitResult test_zero_buf(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    uint32_t sz = 6;
    float buf[6];
    for (uint32_t i = 0; i < sz; i++) {
        buf[i] = (float) (i + 10);
    }

    zero_buf(buf, sz);

    for (uint32_t i = 0; i < sz; i++) {
        munit_assert_float(buf[i], ==, 0.0f);
    }

    return MUNIT_OK;
}
