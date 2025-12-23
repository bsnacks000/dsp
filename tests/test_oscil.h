
#ifndef DSP_TEST_OSCIL_H
#define DSP_TEST_OSCIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_something(const MunitParameter params[], void* data);


static MunitTest oscil_test_module[] = {
    {"/something", test_something, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
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
