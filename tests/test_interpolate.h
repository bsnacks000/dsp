#ifndef DSP_TEST_INTERPOLATION_H
#define DSP_TEST_INTERPOLATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/interpolate.h>
#include "munit.h"

MunitResult test_interpolate_linear(const MunitParameter params[], void* data);

MunitResult test_interpolate_cubic(const MunitParameter params[], void* data);

// register tests
static MunitTest test_interpolate_module[] = {
    {"/cubic", test_interpolate_cubic, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/linear", test_interpolate_linear, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    // need this NULL stub or we segfault
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

// entrypoint
static const MunitSuite interpolate_test_suite = {
    "/interpolate",          /* name */
    test_interpolate_module, /* tests */
    NULL,                    /* suites */
    1,                       /* iterations */
    MUNIT_SUITE_OPTION_NONE  /* options */
};

#ifdef __cplusplus
}
#endif

#endif
