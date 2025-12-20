
#ifndef DSP_TEST_PBLEP_H
#define DSP_TEST_PBLEP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_blepsaw(const MunitParameter params[], void* data);

MunitResult test_blepsqr(const MunitParameter params[], void* data);

static MunitTest pblep_test_module[] = {
    {"/blepsaw", test_blepsaw, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},

    {"/blepsqr", test_blepsqr, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite pblep_test_suite = {
    "/pblep",               /* name */
    pblep_test_module,      /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif
#
#endif
