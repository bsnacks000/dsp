
#ifndef SPCTRL_TEST_STFT_H
#define SPCTRL_TEST_STFT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_stft_engine(const MunitParameter params[], void* data);

static MunitTest stft_test_module[] = {
    {"/stft_engine", test_stft_engine, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite stft_test_suite = {
    "/stft",                /* name */
    stft_test_module,       /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif
#
#endif
