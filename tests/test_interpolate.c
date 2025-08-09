#include "test_interpolate.h"

MunitResult test_interpolate_linear(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;
    float x = interpolate_linear(1.0, 2.0, 0.5);
    munit_assert_float(x, ==, 1.5);

    return MUNIT_OK;
}

MunitResult test_interpolate_cubic(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float x = interpolate_cubic(1.0, 2.0, 3.0, 4.0, 0.5);
    munit_assert_float(x, ==, 2.5);

    return MUNIT_OK;
}
