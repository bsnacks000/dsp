#include "test_maths.h"

MunitResult test_mult(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float x[] = {0.0, 1.0, 2.0, 3.0, 4.0};
    float y[] = {0.0, 1.0, 2.0, 3.0, 4.0};

    float z[5] = {0.0};

    mult_block(z, x, y, 5);

    float expected[] = {0.0, 1.0, 4.0, 9.0, 16.0};

    munit_assert_memory_equal(sizeof(float) * 5, z, expected);

    return MUNIT_OK;
}

MunitResult test_add(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float x[] = {0.0, 1.0, 2.0, 3.0, 4.0};
    float y[] = {0.0, 1.0, 2.0, 3.0, 4.0};

    float z[5] = {0.0};

    add_block(z, x, y, 5);

    float expected[] = {0.0, 2.0, 4.0, 6.0, 8.0};

    munit_assert_memory_equal(sizeof(float) * 5, z, expected);

    return MUNIT_OK;
}

MunitResult test_subtract(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float x[] = {0.0, 1.0, 2.0, 3.0, 4.0};
    float y[] = {0.0, 1.0, 2.0, 3.0, 4.0};

    float z[5] = {0.0};

    subtract_block(z, x, y, 5);

    float expected[] = {0.0, 0.0, 0.0, 0.0, 0.0};

    munit_assert_memory_equal(sizeof(float) * 5, z, expected);

    return MUNIT_OK;
}

MunitResult test_div(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float x[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    float y[] = {1.0, 2.0, 3.0, 4.0, 5.0};

    float z[5] = {0.0};

    div_block(z, x, y, 5);

    float expected[5] = {1.0, 1.0, 1.0, 1.0, 1.0};

    munit_assert_memory_equal(sizeof(float) * 5, z, expected);

    return MUNIT_OK;
}
