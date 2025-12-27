
#ifndef DSP_TEST_FT_CHEBPOLY_H
#define DSP_TEST_FT_CHEBPOLY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_chebpoly(const MunitParameter params[], void* data);
MunitResult test_chebpoly_deck_generate(const MunitParameter params[], void* data);

static MunitTest ft_chebpoly_test_module[] = {
    {"/chebpoly", test_chebpoly, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},

    {"/deck_generate", test_chebpoly_deck_generate, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite ft_chebpoly_test_suite = {
    "/ft_chebpoly",          /* name */
    ft_chebpoly_test_module, /* tests */
    NULL,                    /* suites */
    1,                       /* iterations */
    MUNIT_SUITE_OPTION_NONE  /* options */
};

#ifdef __cplusplus
}
#endif
#
#endif
