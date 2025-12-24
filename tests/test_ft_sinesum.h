
#ifndef DSP_TEST_FT_SINESUM_H
#define DSP_TEST_FT_SINESUM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_sinesum(const MunitParameter params[], void* data);
MunitResult test_sinesum_deck_generate(const MunitParameter params[], void* data);

static MunitTest ft_sinesum_test_module[] = {
    {"/sinesum", test_sinesum, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/deck_generate", test_sinesum_deck_generate, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite ft_sinesum_test_suite = {
    "/ft_sinesum",          /* name */
    ft_sinesum_test_module, /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif
#
#endif
