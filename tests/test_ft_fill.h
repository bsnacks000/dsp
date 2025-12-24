
#ifndef DSP_TEST_FT_FILL_H
#define DSP_TEST_FT_FILL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_ft_fill(const MunitParameter params[], void* data);

static MunitTest ft_fill_test_module[] = {
    {"/fill", test_ft_fill, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite ft_fill_test_suite = {
    "/ft_fill",             /* name */
    ft_fill_test_module,    /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif
#
#endif
