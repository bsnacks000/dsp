
#ifndef DSP_TEST_OSCIL_H
#define DSP_TEST_OSCIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_osciln_tick_block(const MunitParameter params[], void* data);
MunitResult test_oscili_tick_block(const MunitParameter params[], void* data);
MunitResult test_oscil3_tick_block(const MunitParameter params[], void* data);
MunitResult test_oscili_pm_tick_block(const MunitParameter params[], void* data);
MunitResult test_oscil3_pm_tick_block(const MunitParameter params[], void* data);
MunitResult test_blxoscili_tick_block(const MunitParameter params[], void* data);
MunitResult test_blxoscil3_tick_block(const MunitParameter params[], void* data);
MunitResult test_xoscili_tick_block(const MunitParameter params[], void* data);
MunitResult test_xoscil3_tick_block(const MunitParameter params[], void* data);

static MunitTest oscil_test_module[] = {
    {"/osciln_tick_block", test_osciln_tick_block, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/oscili_tick_block", test_oscili_tick_block, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/oscil3_tick_block", test_oscil3_tick_block, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/oscili_pm_tick_block", test_oscili_pm_tick_block, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/oscil3_pm_tick_block", test_oscil3_pm_tick_block, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/blxoscili_tick_block", test_blxoscili_tick_block, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/blxoscil3_tick_block", test_blxoscil3_tick_block, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/xoscili_tick_block", test_xoscili_tick_block, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/xoscil3_tick_block", test_xoscil3_tick_block, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite oscil_test_suite = {
    "/oscil",               /* name */
    oscil_test_module,      /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif
#
#endif
