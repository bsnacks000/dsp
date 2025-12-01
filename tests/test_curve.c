#include "test_curve.h"

#include <dsp/curve.h>

// static inline float ms_per_samps(float nsmps, float sr) {
//     return nsmps / sr * 1000.0;
// }

MunitResult test_curve_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    return MUNIT_OK;
}

MunitResult test_curve_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    return MUNIT_OK;
}

MunitResult test_ar_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    return MUNIT_OK;
}

MunitResult test_ar_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    return MUNIT_OK;
}

MunitResult test_adsr_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    return MUNIT_OK;
}

MunitResult test_adsr_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    return MUNIT_OK;
}
