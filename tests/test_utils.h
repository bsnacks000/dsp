#ifndef DSP_TEST_UTILS
#define DSP_TEST_UTILS

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/utils.h>
#include <dsp/wavetable/normalize.h>

#include <stdbool.h>
#include "munit.h"

MunitResult test_float_underflow(const MunitParameter params[], void* data);

MunitResult test_branchless_float_wrap_range(const MunitParameter params[], void* data);

MunitResult test_branchless_float_wrap_positive(const MunitParameter params[],
                                                void* data);

MunitResult test_ceiling_pow2(const MunitParameter params[], void* data);

MunitResult test_check_float_equal(const MunitParameter params[], void* data);

MunitResult test_has_fractional_part(const MunitParameter params[], void* data);

// register tests
static MunitTest test_utils_module[] = {
    {"/branchless_float_wrap_range", test_branchless_float_wrap_range, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/check_float_underflow", test_float_underflow, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/branchless_float_wrap_positive", test_branchless_float_wrap_positive, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/ceiling_pow2", test_ceiling_pow2, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/check_float_equal", test_check_float_equal, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/has_fractional_part", test_has_fractional_part, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    // need this NULL stub or we segfault
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

// entrypoint
static const MunitSuite utils_test_suite = {
    "/utils",               /* name */
    test_utils_module,      /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif

#endif
