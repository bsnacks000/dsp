
#ifndef DSP_TEST_FT_WINDOW_H
#define DSP_TEST_FT_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "munit.h"

MunitResult test_ft_window_hamming(const MunitParameter params[], void* data);
MunitResult test_ft_window_hanning(const MunitParameter params[], void* data);
MunitResult test_ft_window_blackman(const MunitParameter params[], void* data);
MunitResult test_ft_window_blackman_harris(const MunitParameter params[], void* data);
MunitResult test_ft_window_gauss(const MunitParameter params[], void* data);
MunitResult test_ft_window_sinc(const MunitParameter params[], void* data);

static MunitTest ft_window_test_module[] = {
    {"/hamming", test_ft_window_hamming, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/hanning", test_ft_window_hanning, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/blackman", test_ft_window_blackman, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/blackman_harris", test_ft_window_blackman_harris, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/gauss", test_ft_window_gauss, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/sinc", test_ft_window_sinc, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite ft_window_test_suite = {
    "/ft_window",           /* name */
    ft_window_test_module,  /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif
#
#endif
