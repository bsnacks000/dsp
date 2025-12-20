
#ifndef DSP_TEST_LAG_H
#define DSP_TEST_LAG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_lag_init(const MunitParameter params[], void* data);

MunitResult test_lag_tick_block(const MunitParameter params[], void* data);

static MunitTest lag_test_module[] = {
    {"/init", test_lag_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/tick_block", test_lag_tick_block, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite lag_test_suite = {
    "/lag",                 /* name */
    lag_test_module,        /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif
#
#endif
