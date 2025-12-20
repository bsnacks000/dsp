
#ifndef DSP_TEST_FOLLOW_H
#define DSP_TEST_FOLLOW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_follow_peak_tick_block(const MunitParameter params[], void* data);

MunitResult test_follow_init(const MunitParameter params[], void* data);

MunitResult test_follow_rms_tick_block(const MunitParameter params[], void* data);
static MunitTest follow_test_module[] = {
    {"/init", test_follow_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},

    {"/rms_tick_block", test_follow_rms_tick_block, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},

    {"/peak_tick_block", test_follow_peak_tick_block, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite follow_test_suite = {
    "/follow",              /* name */
    follow_test_module,     /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif
#
#endif
