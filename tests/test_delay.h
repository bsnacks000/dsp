
#ifndef DSP_TEST_DELAY_H
#define DSP_TEST_DELAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_delay_line_calculate_buf_sz(const MunitParameter params[], void* data);

MunitResult test_delay_line_init(const MunitParameter params[], void* data);

MunitResult test_delay_line_advance(const MunitParameter params[], void* data);

MunitResult test_delay_line_write(const MunitParameter params[], void* data);

MunitResult test_delay_line_tapi(const MunitParameter params[], void* data);

MunitResult test_delay_line_tap3(const MunitParameter params[], void* data);

MunitResult test_z1_delay(const MunitParameter params[], void* data);

static MunitTest delay_test_module[] = {
    {"/delay_line_calculate_buf_sz", test_delay_line_calculate_buf_sz, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/delay_line_init", test_delay_line_init, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/delay_line_advance", test_delay_line_advance, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/delay_line_write", test_delay_line_write, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/delay_line_tapi", test_delay_line_tapi, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/delay_line_tap3", test_delay_line_tap3, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},

    {"/z1_delay", test_z1_delay, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},

    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite delay_test_suite = {
    "/delay",               /* name */
    delay_test_module,      /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif
#
#endif
