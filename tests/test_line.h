
#ifndef DSP_TEST_LINE_H
#define DSP_TEST_LINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_line_init(const MunitParameter params[], void* data);
MunitResult test_line_tick_block(const MunitParameter params[], void* data);
MunitResult test_line_ar_init(const MunitParameter params[], void* data);
MunitResult test_line_ar_tick_block(const MunitParameter params[], void* data);
MunitResult test_line_adsr_init(const MunitParameter params[], void* data);
MunitResult test_line_adsr_tick_block(const MunitParameter params[], void* data);
MunitResult test_sampi(const MunitParameter params[], void* data);

static MunitTest line_test_module[] = {
    {"/line_init", test_line_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},

    {"/line_tick_block", test_line_tick_block, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/ar_init", test_line_ar_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/ar_tick_block", test_line_ar_tick_block, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/adsr_init", test_line_adsr_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/adsr_tick_block", test_line_adsr_tick_block, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/sampi", test_sampi, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},

    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite line_test_suite = {
    "/line",                /* name */
    line_test_module,       /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif
#
#endif
