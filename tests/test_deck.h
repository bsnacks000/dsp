
#ifndef DSP_TEST_DECK_H
#define DSP_TEST_DECK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_deck_matrix_fill(const MunitParameter params[], void* data);
MunitResult test_deck_frames_equal(const MunitParameter params[], void* data);

static MunitTest deck_test_module[] = {
    {"/matrix_fill", test_deck_matrix_fill, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},

    {"/frames_equal", test_deck_matrix_fill, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite deck_test_suite = {
    "/deck",                /* name */
    deck_test_module,       /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif
#
#endif
