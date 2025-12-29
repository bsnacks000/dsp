
#ifndef DSP_TEST_XFADE_H
#define DSP_TEST_XFADE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_xfade_sin(const MunitParameter params[], void* data);

MunitResult test_xfade_from_pos(const MunitParameter params[], void* data);

MunitResult test_xfade_init(const MunitParameter params[], void* data);

MunitResult test_xfade_tick_block(const MunitParameter params[], void* data);

static MunitTest xfade_test_module[] = {
    {"/xfade_sin", test_xfade_sin, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/xfade_from_pos", test_xfade_from_pos, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/xfade_init", test_xfade_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/xfade_tick_block", test_xfade_tick_block, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite xfade_test_suite = {
    "/xfade",               /* name */
    xfade_test_module,      /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif
#
#endif
