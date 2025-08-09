#include "test_utils.h"

MunitResult test_float_underflow(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;
    float x = check_float_underflow(0.01);
    munit_assert_float(x, ==, 0.01);

    x = check_float_underflow(1e-21f);
    munit_assert_float(x, ==, 0.0);

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
