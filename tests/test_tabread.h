
#ifndef DSP_TEST_TABREAD_H
#define DSP_TEST_TABREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_tabread_init(const MunitParameter params[], void* data);
MunitResult test_tabreadn_tick_block(const MunitParameter params[], void* data);
MunitResult test_tabreadi_tick_block(const MunitParameter params[], void* data);
MunitResult test_tabread3_tick_block(const MunitParameter params[], void* data);
MunitResult test_xtabread_init(const MunitParameter params[], void* data);
MunitResult test_xtabreadi_tick_block(const MunitParameter params[], void* data);
MunitResult test_xtabread3_tick_block(const MunitParameter params[], void* data);

static MunitTest tabread_test_module[] = {
    {"/tabread_init", test_tabread_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},

    {"/tabreadn_tick_block", test_tabreadn_tick_block, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/tabreadi_tick_block", test_tabreadi_tick_block, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/tabread3_tick_block", test_tabread3_tick_block, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/xtabread_init", test_xtabread_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/xtabreadi_tick_block", test_xtabreadi_tick_block, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/xtabread3_tick_block", test_xtabread3_tick_block, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},

    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite tabread_test_suite = {
    "/tabread",             /* name */
    tabread_test_module,    /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif
#
#endif
