#include "test_xfade.h"
#include <dsp/xfade.h>
#include "helpers.h"

#define NSMPS 64

MunitResult test_xfade_sin(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    // test clamp
    xfade_pair x = xfade_sin(42.0);
    munit_assert_double(x.right, <=, 1.0);

    x = xfade_sin(-42.0);
    munit_assert_double(x.left, >=, 0.0);

    // note the fast versions are a little off precision wise but thats ok
    x = xfade_sin(0.5);
    munit_assert_double_equal(x.left, 0.7, 1);
    munit_assert_double_equal(x.right, 0.7, 1);

    return MUNIT_OK;
}

MunitResult test_xfade_from_pos(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    xfade_pair x = xfade_from_pos(0.5, 100);
    munit_assert_double_equal(x.left, 0.7, 1);
    munit_assert_double_equal(x.right, 0.7, 1);

    return MUNIT_OK;
}

MunitResult test_xfade_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    xfade x;

    xfade_init(&x, 0.5);
    munit_assert_double_equal(x.l_coeff_, 0.7, 1);
    munit_assert_double_equal(x.r_coeff_, 0.7, 1);

    return MUNIT_OK;
}

MunitResult test_xfade_tick_block(const MunitParameter params[], void* data) {

    (void) params;
    (void) data;

    xfade x;
    xfade_init(&x, 0.5);

    float in_l[NSMPS];
    float in_r[NSMPS];
    float pos[NSMPS];

    float out[NSMPS] = {0.0};

    for (int i = 0; i < NSMPS; i++) {
        in_l[i] = 0.5;
        in_r[i] = 0.5;
        pos[i] = 0.51;
    }

    xfade_tick_block(&x, out, in_l, in_r, pos, NSMPS);

    check_finite(out, NSMPS);

    return MUNIT_OK;
}
