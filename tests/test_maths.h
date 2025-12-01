#ifndef DSP_TEST_MATHS
#define DSP_TEST_MATHS

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_mult(const MunitParameter params[], void* data);

MunitResult test_add(const MunitParameter params[], void* data);

MunitResult test_subtract(const MunitParameter params[], void* data);

MunitResult test_div(const MunitParameter params[], void* data);

MunitResult test_qsinf(const MunitParameter params[], void* data);

MunitResult test_qcosf(const MunitParameter params[], void* data);

MunitResult test_hsinf(const MunitParameter params[], void* data);

MunitResult test_hcosf(const MunitParameter params[], void* data);

MunitResult test_sinf(const MunitParameter params[], void* data);

MunitResult test_cosf(const MunitParameter params[], void* data);

MunitResult test_sign_of(const MunitParameter params[], void* data);

MunitResult test_hard_clip(const MunitParameter params[], void* data);

MunitResult test_exp_clip(const MunitParameter params[], void* data);

MunitResult test_clamp(const MunitParameter params[], void* data);

MunitResult test_tanh_clip(const MunitParameter params[], void* data);

MunitResult test_atan_clip(const MunitParameter params[], void* data);

MunitResult test_noise_block(const MunitParameter params[], void* data);

MunitResult test_scale(const MunitParameter params[], void* data);

MunitResult test_dc_offset(const MunitParameter params[], void* data);

MunitResult test_matrix_init(const MunitParameter params[], void* data);

MunitResult test_matrix_set_and_at(const MunitParameter params[], void* data);

MunitResult test_matrix_get_row(const MunitParameter params[], void* data);

MunitResult test_matrix_set_row(const MunitParameter params[], void* data);

MunitResult test_matrix_transpose(const MunitParameter params[], void* data);

// register tests
static MunitTest test_maths_module[] = {
    {"/mult", test_mult, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/add", test_add, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/div", test_div, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/subtract", test_subtract, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/qsinf", test_qsinf, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/qcosf", test_qcosf, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/hsinf", test_hsinf, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/hcosf", test_hcosf, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/sinf", test_sinf, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/cosf", test_cosf, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},

    {"/flip", test_sign_of, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/hard_clip", test_hard_clip, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/exp_clip", test_exp_clip, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/clamp", test_clamp, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/tanh_clip", test_tanh_clip, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/atan_clip", test_atan_clip, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/noise_block", test_noise_block, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/scale", test_scale, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/dc_offset", test_dc_offset, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/matrix_init", test_matrix_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/dc_offset", test_matrix_get_row, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/dc_offset", test_matrix_set_and_at, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/dc_offset", test_matrix_set_row, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/dc_offset", test_matrix_transpose, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},

    // need this NULL stub or we segfault
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

// entrypoint
static const MunitSuite maths_test_suite = {
    "/maths",               /* name */
    test_maths_module,      /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif

#endif
