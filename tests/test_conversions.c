#include "test_conversions.h"
#include "dsp/conversions.h"

MunitResult test_samps_per_ms(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float s = samps_per_ms(1000.0);
    munit_assert_float(s, ==, 1.0);

    return MUNIT_OK;
}

MunitResult test_db_to_amp(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float a = db_to_amp(0.0);
    munit_assert_float(a, ==, 1.0);

    return MUNIT_OK;
}

MunitResult test_db_to_power(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float p = db_to_power(0.0);
    munit_assert_float(p, ==, 1.0);

    return MUNIT_OK;
}

MunitResult test_amp_to_db(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float d = amp_to_db(1.0);
    munit_assert_float(d, ==, 0.0);

    return MUNIT_OK;
}

MunitResult test_power_to_db(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float x = power_to_db(1.0);
    munit_assert_float(x, ==, 0.0);

    return MUNIT_OK;
}

MunitResult test_peak_gain_for_q(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float a = peak_gain_for_q(1.0);  // 1.1547005383792517
    munit_assert_double_equal(a, 1.1547005383792517, 6);

    float b = peak_gain_for_q(0.0);
    munit_assert_float(b, ==, 1.0);

    return MUNIT_OK;
}

MunitResult test_db_peak_gain_for_q(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float a = db_peak_gain_for_q(1.0);  // 1.2493873660830006
    munit_assert_double_equal(a, 1.2493873660830006, 6);
    return MUNIT_OK;
}

MunitResult test_unipolar_to_bipolar(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float x = unipolar_to_bipolar(1.0);
    munit_assert_float(x, ==, 1.0);

    float y = unipolar_to_bipolar(0.0);
    munit_assert_float(y, ==, -1.0);

    return MUNIT_OK;
}

MunitResult test_bipolar_to_unipolar(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float x = bipolar_to_unipolar(1.0);
    munit_assert_float(x, ==, 1.0);

    float y = bipolar_to_unipolar(-1.0);
    munit_assert_float(y, ==, 0.0);

    return MUNIT_OK;
}
