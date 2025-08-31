#ifndef DSP_TEST_INTERPOLATION_H
#define DSP_TEST_INTERPOLATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/interpolate.h>
#include "munit.h"

MunitResult test_interpolate_linear(const MunitParameter params[], void* data);

MunitResult test_interpolate_cubic(const MunitParameter params[], void* data);

MunitResult test_npinterp_endpoints(const MunitParameter params[], void* data);

MunitResult test_npinterp_exact_matches(const MunitParameter params[], void* data);

MunitResult test_npinterp_linear_segments(const MunitParameter params[], void* data);

MunitResult test_npinterp_monotonic(const MunitParameter params[], void* data);

// register tests
static MunitTest test_interpolate_module[] = {
    {"/cubic", test_interpolate_cubic, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/linear", test_interpolate_linear, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/npinterp/endpoints", test_npinterp_endpoints, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/npinterp/exact", test_npinterp_exact_matches, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/npinterp/linear_segments", test_npinterp_linear_segments, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/npinterp/monotonic", test_npinterp_monotonic, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
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
