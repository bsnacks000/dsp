#include "munit.h"

#include "test_balance.h"
#include "test_bq.h"
#include "test_conversions.h"
#include "test_curve.h"
#include "test_delay.h"
#include "test_follow.h"
#include "test_interpolate.h"
#include "test_lag.h"
#include "test_line.h"
#include "test_maths.h"
#include "test_rc.h"
#include "test_stft.h"
#include "test_svf.h"
#include "test_utils.h"
#include "test_wavetable.h"
#include "test_xfade.h"
/// see: https://github.com/nemequ/munit/blob/master/example.c

int main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)]) {

    // register test_suites
    // sentinel
    MunitSuite all_test_suites[] = {
        maths_test_suite,
        utils_test_suite,
        interpolate_test_suite,
        wavetable_test_suite,
        conversions_test_suite,
        balance_test_suite,
        bq_test_suite,
        curve_test_suite,
        line_test_suite,
        rc_test_suite,
        svf_test_suite,
        xfade_test_suite,
        delay_test_suite,
        stft_test_suite,
        lag_test_suite,
        follow_test_suite,
        {NULL, NULL, NULL, 1, MUNIT_SUITE_OPTION_NONE},
    };

    MunitSuite test_suite_main = {
        "/dsp",                 /* name */
        NULL,                   /* tests */
        all_test_suites,        /* suites */
        1,                      /* iterations */
        MUNIT_SUITE_OPTION_NONE /* options */
    };

    return munit_suite_main(&test_suite_main, NULL, argc, argv);
}
