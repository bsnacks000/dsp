#ifndef DSP_TEST_CRV_H
#define DSP_TEST_CRV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_curve_init(const MunitParameter params[], void* data);

MunitResult test_curve_tick_block(const MunitParameter params[], void* data);

MunitResult test_ar_init(const MunitParameter params[], void* data);

MunitResult test_ar_tick_block(const MunitParameter params[], void* data);

MunitResult test_adsr_init(const MunitParameter params[], void* data);

MunitResult test_adsr_tick_block(const MunitParameter params[], void* data);

// register tests
static MunitTest curve_test_module[] = {
    {"/init", test_curve_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},

    {"/tick_block", test_curve_tick_block, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/ar_init", test_ar_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/ar_tick_block", test_ar_tick_block, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/adsr_init", test_adsr_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/adsr_tick_block", test_adsr_tick_block, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    // need this NULL stub or we segfault
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

// entrypoint
static const MunitSuite curve_test_suite = {
    "/curve",               /* name */
    curve_test_module,      /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif

#endif
