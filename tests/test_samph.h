
#ifndef DSP_TEST_SAMPH_H
#define DSP_TEST_SAMPH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_samph_init(const MunitParameter params[], void* data);

MunitResult test_samph_tick_block(const MunitParameter params[], void* data);

static MunitTest samph_test_module[] = {
    {"/init", test_samph_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/tick_block", test_samph_tick_block, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite samph_test_suite = {
    "/samph",               /* name */
    samph_test_module,      /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif
#
#endif
