#ifndef DSP_TEST_CONVERSIONS
#define DSP_TEST_CONVERSIONS

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_samps_per_ms(const MunitParameter params[], void* data);

MunitResult test_db_to_amp(const MunitParameter params[], void* data);

MunitResult test_db_to_power(const MunitParameter params[], void* data);

MunitResult test_amp_to_db(const MunitParameter params[], void* data);

MunitResult test_power_to_db(const MunitParameter params[], void* data);

MunitResult test_peak_gain_for_q(const MunitParameter params[], void* data);

MunitResult test_db_peak_gain_for_q(const MunitParameter params[], void* data);

MunitResult test_unipolar_to_bipolar(const MunitParameter params[], void* data);

MunitResult test_bipolar_to_unipolar(const MunitParameter params[], void* data);

// register tests
static MunitTest test_conversions_module[] = {
    {"/samps_per_ms", test_samps_per_ms, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/db_to_amp", test_db_to_amp, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/db_to_power", test_db_to_power, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/amp_to_db", test_amp_to_db, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/power_to_db", test_power_to_db, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/peak_gain_for_q", test_peak_gain_for_q, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/db_peak_gain_for_q", test_db_peak_gain_for_q, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/unipolar_bipolar", test_unipolar_to_bipolar, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/bipolar_to_unipolar", test_bipolar_to_unipolar, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    // need this NULL stub or we segfault
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

// entrypoint
static const MunitSuite conversions_test_suite = {
    "/conversions",          /* name */
    test_conversions_module, /* tests */
    NULL,                    /* suites */
    1,                       /* iterations */
    MUNIT_SUITE_OPTION_NONE  /* options */
};

#ifdef __cplusplus
}
#endif

#endif
