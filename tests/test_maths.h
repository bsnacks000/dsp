#ifndef DSP_TEST_MATHS
#define DSP_TEST_MATHS

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/maths.h>
#include "munit.h"

MunitResult test_mult(const MunitParameter params[], void* data);

MunitResult test_add(const MunitParameter params[], void* data);

MunitResult test_subtract(const MunitParameter params[], void* data);

MunitResult test_div(const MunitParameter params[], void* data);

// register tests
static MunitTest test_maths_module[] = {
    {"/mult", test_mult, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/add", test_add, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/div", test_div, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/subtract", test_subtract, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
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
