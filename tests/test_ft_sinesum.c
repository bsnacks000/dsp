

#include "test_ft_sinesum.h"

#include <dsp/sinesum.h>

#define AMPS_SZ 64
#define BUF_SZ 64

MunitResult test_sinesum(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf[BUF_SZ] = {0};
    float amps[AMPS_SZ] = {0};

    // make a sine
    amps[0] = 1.0f;
    sinesum(buf, BUF_SZ, amps, 1, 0, true);

    // TODO: assert sine data

    return MUNIT_OK;
}

MunitResult test_amps(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float amps[AMPS_SZ] = {0};

    // TODO: assert data for all of these:

    sine_amps(amps, AMPS_SZ);
    buzz_amps(amps, AMPS_SZ);
    saw_amps(amps, AMPS_SZ);
    sqr_amps(amps, AMPS_SZ);
    tri_amps(amps, AMPS_SZ);

    return MUNIT_OK;
}

MunitResult test_harmonics(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float mf = max_fundamental(64, 48000.0, 1.0);
    munit_assert_double_equal(375.0, mf, 7);

    float mh = max_harmonic(440.0, 48000.0);
    munit_assert_uint32(54, ==, mh);

    uint32_t nbands = calculate_n_bands(64);
    munit_assert_uint32(7, ==, nbands);

    return MUNIT_OK;
}
