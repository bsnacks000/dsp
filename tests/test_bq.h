#ifndef DSP_TEST_BQ_H
#define DSP_TEST_BQ_H

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_dfII_init(const MunitParameter params[], void* data);
MunitResult test_dfII_tick(const MunitParameter params[], void* data);

// bq nonresonant - first order filters / butterworth / linkwitz-riley
MunitResult test_bq_non_resonant_init(const MunitParameter params[], void* data);

MunitResult test_bq_non_resonant_tick_block_lpf(const MunitParameter params[],
                                                void* data);

MunitResult test_bq_non_resonant_tick_block_hpf(const MunitParameter params[],
                                                void* data);

MunitResult test_bq_non_resonant_tick_block_apf(const MunitParameter params[],
                                                void* data);

MunitResult test_bq_non_resonant_tick_block_butterworth(const MunitParameter params[],
                                                        void* data);

MunitResult test_bq_non_resonant_tick_block_linkwitzriley(const MunitParameter params[],
                                                          void* data);

// bq resonant - 2nd order filters + smith-angell reson
MunitResult test_bq_resonant_init(const MunitParameter params[], void* data);

MunitResult test_bq_resonant_tick_block_lpf(const MunitParameter params[], void* data);

MunitResult test_bq_resonant_tick_block_hpf(const MunitParameter params[], void* data);

MunitResult test_bq_resonant_tick_block_bpf(const MunitParameter params[], void* data);

MunitResult test_bq_resonant_tick_block_bsf(const MunitParameter params[], void* data);

MunitResult test_bq_resonant_tick_block_reson(const MunitParameter params[],
                                              void* data);

MunitResult test_bq_resonant_tick_block_apf(const MunitParameter params[], void* data);

// parametric filters
MunitResult test_bq_para_eq_init(const MunitParameter params[], void* data);

MunitResult test_bq_para_eq_tick_block_lshlf(const MunitParameter params[], void* data);

MunitResult test_bq_para_eq_tick_block_hshlf(const MunitParameter params[], void* data);

MunitResult test_bq_para_eq_tick_block_peak(const MunitParameter params[], void* data);

// register tests
static MunitTest test_bq_module[] = {
    {"/dfII_init", test_dfII_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/dfII_tick", test_dfII_tick, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},

    {"/non_resonant_init", test_bq_non_resonant_init, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/non_resonant_tick_block_lpf", test_bq_non_resonant_tick_block_lpf, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/non_resonant_tick_block_hpf", test_bq_non_resonant_tick_block_hpf, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/non_resonant_tick_block_apf", test_bq_non_resonant_tick_block_apf, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/non_resonant_tick_block_butterworth",
     test_bq_non_resonant_tick_block_butterworth, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/non_resonant_tick_block_linkwitzriley",
     test_bq_non_resonant_tick_block_linkwitzriley, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/dfII_tick", test_bq_resonant_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/dfII_tick", test_bq_resonant_tick_block_lpf, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/dfII_tick", test_bq_resonant_tick_block_hpf, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/dfII_tick", test_bq_resonant_tick_block_bpf, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/dfII_tick", test_bq_resonant_tick_block_bsf, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/dfII_tick", test_bq_resonant_tick_block_reson, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/dfII_tick", test_bq_resonant_tick_block_apf, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/dfII_tick", test_bq_para_eq_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/dfII_tick", test_bq_para_eq_tick_block_lshlf, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/dfII_tick", test_bq_para_eq_tick_block_hshlf, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/dfII_tick", test_bq_para_eq_tick_block_peak, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},

    // need this NULL stub or we segfault
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

// entrypoint
static const MunitSuite bq_test_suite = {
    "/bq",                  /* name */
    test_bq_module,         /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif

#endif
