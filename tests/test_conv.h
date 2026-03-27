
#ifndef DSP_TEST_CONV_H
#define DSP_TEST_CONV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/conv.h>
#include "munit.h"

MunitResult test_dconv(const MunitParameter params[], void* data);
MunitResult test_mpconv(const MunitParameter params[], void* data);
MunitResult test_zconv(const MunitParameter params[], void* data);

static MunitTest conv_test_module[] = {
    {"/dconv", test_dconv, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/mpconv", test_mpconv, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/zconv", test_zconv, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},

    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite conv_test_suite = {
    "/conv",                /* name */
    conv_test_module,       /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif
#
#endif
