
#ifndef DSP_TEST_SVF_H
#define DSP_TEST_SVF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_svf_init(const MunitParameter params[], void* data);

MunitResult test_svf_tick_block(const MunitParameter params[], void* data);

static MunitTest svf_test_module[] = {
    {"/svf_init", test_svf_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},

    {"/svf_tick_block", test_svf_tick_block, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite svf_test_suite = {
    "/svf",                 /* name */
    svf_test_module,        /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif

#endif
