#include "test_maths.h"

#include <dsp/maths.h>
#include <dsp/matrix.h>
#include <dsp/shape.h>

MunitResult test_mult(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float x[] = {0.0, 1.0, 2.0, 3.0, 4.0};
    float y[] = {0.0, 1.0, 2.0, 3.0, 4.0};

    float z[5] = {0.0};

    mult_block(z, x, y, 0, 5);

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

    add_block(z, x, y, 0, 5);

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

    subtract_block(z, x, y, 0, 5);

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

    div_block(z, x, y, 0, 5);

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

MunitResult test_sign_of(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float a = sign_of(-123.123);
    munit_assert_float(a, ==, -1.0);

    a = sign_of(123.123);
    munit_assert_float(a, ==, 1.0);

    return MUNIT_OK;
}

MunitResult test_hard_clip(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float a = hard_clip(2.0, 1.0);
    munit_assert_float(a, ==, 1.0);

    return MUNIT_OK;
}

MunitResult test_exp_clip(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float a = exp_clip(0.5, 0.5);  // 0.22119921692859512
    munit_assert_double_equal(a, 0.22119921692859512, 6);

    return MUNIT_OK;
}

MunitResult test_clamp(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float a = clamp(1.0, 0.3, 0.7);
    munit_assert_float(a, ==, 0.7);

    a = clamp(0.0, 0.3, 0.7);
    munit_assert_float(a, ==, 0.3);

    a = clamp(0.5, 0.3, 0.7);
    munit_assert_float(a, ==, 0.5);

    return MUNIT_OK;
}

MunitResult test_tanh_clip(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float a = tanh_clip(0.5, 0.5);  // 0.5299937267423438
    munit_assert_double_equal(a, 0.5299937267423438, 4);

    return MUNIT_OK;
}

MunitResult test_atan_clip(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float a = atan_clip(0.5, 0.5);  // 0.5283735692285408
    munit_assert_double_equal(a, 0.5283735692285408, 4);

    return MUNIT_OK;
}

MunitResult test_noise_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf[128];
    noise_block(buf, 0, 128);

    // Check values are within [-1, 1]
    for (uint32_t i = 0; i < 128; i++) {
        munit_assert_double(buf[i], <=, 1.0);
        munit_assert_double(buf[i], >=, -1.0);
    }

    return MUNIT_OK;
}

MunitResult test_scale(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float in[4] = {1.0f, -2.0f, 0.5f, 0.0f};
    float out[4];
    const float factor = 2.0f;

    scale_block(out, in, factor, 0, 4);

    munit_assert_float(out[0], ==, 2.0f);
    munit_assert_float(out[1], ==, -4.0f);
    munit_assert_float(out[2], ==, 1.0f);
    munit_assert_float(out[3], ==, 0.0f);

    return MUNIT_OK;
}

MunitResult test_dc_offset(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float in[4] = {1.0f, -2.0f, 0.5f, 0.0f};
    float out[4];
    const float offset = 1.0f;

    dc_block(out, in, offset, 0, 4);

    munit_assert_float(out[0], ==, 2.0f);
    munit_assert_float(out[1], ==, -1.0f);
    munit_assert_float(out[2], ==, 1.5f);
    munit_assert_float(out[3], ==, 1.0f);

    return MUNIT_OK;
}

MunitResult test_matrix_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    size_t rows = 2;
    size_t cols = 3;
    float buf[6];
    matrix m;
    matrix_init(&m, buf, rows, cols);

    munit_assert_ptr_equal(m.data, buf);
    munit_assert_size(m.n_rows, ==, rows);
    munit_assert_size(m.n_cols, ==, cols);

    return MUNIT_OK;
}

MunitResult test_matrix_set_and_at(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf[4];
    matrix m;
    matrix_init(&m, buf, 2, 2);

    float v0 = 1.25f;
    float v1 = -2.5f;
    matrix_set(&m, v0, 0, 0);
    matrix_set(&m, v1, 1, 1);

    float r0 = matrix_at(&m, 0, 0);
    float r1 = matrix_at(&m, 1, 1);

    munit_assert_float(r0, ==, v0);
    munit_assert_float(r1, ==, v1);

    return MUNIT_OK;
}

MunitResult test_matrix_get_row(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf[6];
    for (size_t i = 0; i < 6; i++)
        buf[i] = (float) i;

    matrix m;
    matrix_init(&m, buf, 2, 3);

    float* row1 = matrix_get_row(&m, 1);

    munit_assert_ptr_equal(row1, &buf[3]);
    munit_assert_float(row1[0], ==, buf[3]);
    munit_assert_float(row1[1], ==, buf[4]);
    munit_assert_float(row1[2], ==, buf[5]);

    return MUNIT_OK;
}

MunitResult test_matrix_set_row(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf[6] = {0};
    matrix m;
    matrix_init(&m, buf, 2, 3);

    float src[3];
    for (size_t i = 0; i < 3; i++)
        src[i] = (float) (i + 10);

    matrix_set_row(&m, 1, src, 3);

    for (size_t j = 0; j < 3; j++) {
        float v = matrix_at(&m, 1, j);
        munit_assert_float(v, ==, src[j]);
    }

    return MUNIT_OK;
}

MunitResult test_matrix_transpose(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float bufA[6];
    for (size_t i = 0; i < 6; i++)
        bufA[i] = (float) (i + 1);

    float bufB[6] = {0};

    matrix a, b;
    matrix_init(&a, bufA, 2, 3);
    matrix_init(&b, bufB, 3, 2);

    matrix_transpose(&b, &a);

    for (size_t i = 0; i < a.n_rows; i++) {
        for (size_t j = 0; j < a.n_cols; j++) {
            float v_a = matrix_at(&a, i, j);
            float v_b = matrix_at(&b, j, i);
            munit_assert_float(v_b, ==, v_a);
        }
    }

    return MUNIT_OK;
}
