
#ifndef DSP_TEST_PHASOR_H
#define DSP_TEST_PHASOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_phasor_tick_block(const MunitParameter params[], void* data);
MunitResult test_impulse_tick_block(const MunitParameter params[], void* data);
MunitResult test_rand_impulse_tick_block(const MunitParameter params[], void* data);

static MunitTest phasor_test_module[] = {

    {"/tick_block", test_phasor_tick_block, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/impulse_tick_block", test_impulse_tick_block, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/rand_impulse_tick_block", test_rand_impulse_tick_block, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},

    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite phasor_test_suite = {
    "/phasor",              /* name */
    phasor_test_module,     /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif
#
#endif
