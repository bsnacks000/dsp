#include "test_wavetable.h"
#include "dsp/wavetable/normalize.h"
#include "dsp/wavetable/wavetable.h"

MunitResult test_wavetable_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;
    wavetable wt;

    float x[10] = {0.0};

    wavetable_init(&wt, x, 10);

    munit_assert(wt.is_pow2 == 1);
    munit_assert(wt.len == 8);
    munit_assert(wt.mask == 7);

    return MUNIT_OK;
}

MunitResult test_normalize(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float x[10] = {1.0, -2.0, 3.0, 4.0, 5.0, 6.0, -7.0, 8.0, -9.0, 10.0};

    normalize(x, 10);

    for (int i = 0; i < 10; i++) {
        munit_assert_float(x[i], <=, 1.0);
        munit_assert_float(x[i], >=, -1.0);
    }

    return MUNIT_OK;
}
