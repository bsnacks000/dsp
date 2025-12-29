
#ifndef DSP_TEST_RC_H
#define DSP_TEST_RC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_rc_one_pole_init(const MunitParameter params[], void* data);

MunitResult test_rc_one_pole_tick_block(const MunitParameter params[], void* data);

MunitResult test_rc_ladder_init(const MunitParameter params[], void* data);

MunitResult test_rc_ladder_tick_block(const MunitParameter params[], void* data);

static MunitTest rc_test_module[] = {
    {"/rc_one_pole_init", test_rc_one_pole_init, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/rc_one_pole_tick_block", test_rc_one_pole_tick_block, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/rc_ladder_init", test_rc_ladder_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/rc_ladder_tick_block", test_rc_ladder_tick_block, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite rc_test_suite = {
    "/rc",                  /* name */
    rc_test_module,         /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif
#
#endif
