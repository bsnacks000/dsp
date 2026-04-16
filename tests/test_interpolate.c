#include "test_interpolate.h"
#include <dsp/interpolate.h>

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

MunitResult test_lerp_endpoints(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    // checks we preserve the correct endpoints
    float in[3] = {1.0f, 2.0f, 5.0f};
    size_t in_sz = 3;
    size_t out_sz = 7;
    float out[7];

    table_lerp(out, out_sz, in, in_sz);

    munit_assert_float(out[0], ==, in[0]);
    munit_assert_float(out[out_sz - 1], ==, in[in_sz - 1]);

    return MUNIT_OK;
}

MunitResult test_lerp_exact_matches(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    // assure that we have exact matches at the correct indexes
    float in[4] = {0.0f, 10.0f, 20.0f, 30.0f};
    size_t in_sz = 4;
    size_t out_sz = 7;
    float out[7];

    table_lerp(out, out_sz, in, in_sz);

    // NOTE: Only when out_sz-1 is a multiple of in_sz-1 will the grids line up exactly.
    // in this case alignment is staggered.
    // alignment: out[0] == in[0], out[2] == in[1], out[6] == in[3]
    munit_assert_float(out[0], ==, in[0]);
    munit_assert_float(out[2], ==, in[1]);
    munit_assert_float(out[6], ==, in[3]);

    return MUNIT_OK;
}

MunitResult test_lerp_linear_segments(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    // linear values correctly spaced
    float in[2] = {2.0f, 6.0f};
    size_t in_sz = 2;
    size_t out_sz = 5;
    float out[5];

    table_lerp(out, out_sz, in, in_sz);

    float expected[5] = {2.0f, 3.0f, 4.0f, 5.0f, 6.0f};

    for (size_t i = 0; i < out_sz; i++) {
        munit_assert_double_equal(out[i], expected[i], 6);
    }

    return MUNIT_OK;
}

MunitResult test_lerp_monotonic(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    // assures we are moving in the correct direction
    float in[3] = {1.0f, 4.0f, 9.0f};
    size_t in_sz = 3;
    size_t out_sz = 11;
    float out[11];

    table_lerp(out, out_sz, in, in_sz);

    for (size_t i = 1; i < out_sz; i++) {
        munit_assert_double(out[i], >=, out[i - 1]);
    }

    return MUNIT_OK;
}
