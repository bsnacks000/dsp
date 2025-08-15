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

MunitResult test_qsinf(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    munit_assert_double_equal(fast_qsinf(0.0), 0.0, 6);
    munit_assert_double_equal(fast_qsinf(0.5), 0.4795828785982852, 6);
    munit_assert_double_equal(fast_qsinf(1.0), 0.8402181198803792, 6);
    munit_assert_double_equal(fast_qsinf(-0.5), -0.5144291038666055, 6);
    munit_assert_double_equal(fast_qsinf(-1.0), -1.0053264518366392, 6);

    return MUNIT_OK;
}

MunitResult test_qcosf(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    munit_assert_double_equal(fast_qcosf(0.0), 1.0, 6);
    munit_assert_double_equal(fast_qcosf(0.5), 0.8764773848407118, 6);
    munit_assert_double_equal(fast_qcosf(1.0), 0.5400016582481239, 6);
    munit_assert_double_equal(fast_qcosf(-0.5), 0.8764773848407118, 6);
    munit_assert_double_equal(fast_qcosf(-1.0), 0.5400016582481239, 6);

    return MUNIT_OK;
}

MunitResult test_hsinf(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    munit_assert_double_equal(fast_hsinf(0.0), 0.0, 6);
    munit_assert_double_equal(fast_hsinf(0.5), 1.0, 6);
    munit_assert_double_equal(fast_hsinf(1.0), 0.0, 6);
    munit_assert_double_equal(fast_hsinf(-0.5), 1.0, 6);
    munit_assert_double_equal(fast_hsinf(-1.0), 0.0, 6);

    return MUNIT_OK;
}

MunitResult test_hcosf(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    munit_assert_double_equal(fast_hcosf(0.0), 1.0, 6);
    munit_assert_double_equal(fast_hcosf(0.5), 0.0, 6);
    munit_assert_double_equal(fast_hcosf(1.0), 1.0, 6);
    munit_assert_double_equal(fast_hcosf(-0.5), 0.0, 6);
    munit_assert_double_equal(fast_hcosf(-1.0), 1.0, 6);

    return MUNIT_OK;
}

MunitResult test_sinf(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    munit_assert_double_equal(fast_sinf(0.0), 0.0, 5);
    munit_assert_double_equal(fast_sinf(0.5), 0.0, 6);
    munit_assert_double_equal(fast_sinf(1.0), 0.0, 6);
    munit_assert_double_equal(fast_sinf(-0.5), 0.0, 6);
    munit_assert_double_equal(fast_sinf(-1.0), 0.0, 6);

    return MUNIT_OK;
}

MunitResult test_cosf(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    munit_assert_double_equal(fast_cosf(0.0), 1.0, 5);
    munit_assert_double_equal(fast_cosf(0.5), -1.0, 6);
    munit_assert_double_equal(fast_cosf(1.0), 1.0, 6);
    munit_assert_double_equal(fast_cosf(-0.5), -1.0, 6);
    munit_assert_double_equal(fast_cosf(-1.0), 1.0, 6);

    return MUNIT_OK;
}
