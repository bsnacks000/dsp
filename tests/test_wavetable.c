#include <dsp/utils.h>
#include <dsp/wavetable/wavetable.h>

#include "test_wavetable.h"

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

MunitResult test_wavetable_shallow_copy(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    wavetable wt1;
    wavetable wt2;

    float x[5] = {0.0};
    float y[5] = {0.0};

    wavetable_init(&wt1, x, 5);
    wavetable_init(&wt2, y, 5);

    wavetable_shallow_copy(&wt2, &wt1);
    // assert that the pointers are the same
    munit_assert_ptr_equal(wt1.buf, wt2.buf);

    return MUNIT_OK;
}

MunitResult test_wavetable_deep_copy(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    (void) params;
    (void) data;

    wavetable wt1;
    wavetable wt2;

    float x[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    float y[5] = {0.0};

    wavetable_init(&wt1, x, 5);
    wavetable_init(&wt2, y, 5);

    wavetable_deep_copy(&wt2, &wt1);
    // assert we did not copy the buf pointer explicitly (like shallow copy)
    munit_assert_ptr_not_equal(wt1.buf, wt2.buf);

    // assert that the data was correctly copied.
    munit_assert_memory_equal(5, x, wt2.buf);

    return MUNIT_OK;
}

MunitResult test_wavetable_write(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float x[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    float y[10] = {0.0};

    wavetable wt1;
    wavetable_init(&wt1, y, 10);

    wavetable_write(&wt1, x, 5, 2);

    return MUNIT_OK;
}

static dsp_err wt_test_func(wavetable* wt, void* args) {
    (void) args;
    // fill table with 3 values
    float x = 1.0;
    for (int i = 0; i < 3; i++) {
        wt->buf[i] = x;
        x += 1.0;
    }
    return DSP_OK;
}

MunitResult test_wavetable_func(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    wavetable wt1;

    float x[5] = {0.0};

    wavetable_init(&wt1, x, 5);

    wavetable_func(&wt1, wt_test_func, NULL);

    return MUNIT_OK;
}

MunitResult test_wavetable_add_guard_point(const MunitParameter* params, void* data) {
    (void) params;
    (void) data;

    float x[5] = {1.0, 2.0, 3.0, 0.0, 0.0};

    wavetable wt1;
    wavetable_init(&wt1, x, 5);

    wavetable_add_guard_point(&wt1);
    munit_assert_float(x[4], ==, 2.0);
    munit_assert_float(x[3], ==, 1.0);

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
