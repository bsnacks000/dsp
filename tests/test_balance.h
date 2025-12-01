#ifndef DSP_TEST_BALANCE_H
#define DSP_TEST_BALANCE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_balance_init(const MunitParameter params[], void* data);

MunitResult test_balance_tick_block(const MunitParameter params[], void* data);

static MunitTest test_balance_module[] = {
    {"/balance_init", test_balance_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/balance_tick_block", test_balance_tick_block, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},

    // need this NULL stub or we segfault
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

// entrypoint
static const MunitSuite balance_test_suite = {
    "/balance",             /* name */
    test_balance_module,    /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif

#endif
