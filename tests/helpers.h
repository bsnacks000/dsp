
#ifndef DSP_TEST_HELPERS_H
#define DSP_TEST_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "munit.h"

static inline void fill_random_bipolar_signal(float* buf, uint32_t nsmps) {
    for (uint32_t i = 0; i < nsmps; i++) {
        float r = (float) munit_rand_double();
        buf[i] = 2.0 * r - 1.0;
    }
}

static inline void fill_impulse_response(float* buf, uint32_t nsmps) {
    buf[0] = 1.0;
    for (uint32_t i = 1; i < nsmps; i++) {
        buf[i] = 0.0;
    }
}

static inline void copy_buf(float* dest, float* src, uint32_t nsmps) {
    memcpy(dest, src, sizeof(float) * nsmps);
}

static inline void scale_buf(float* buf, float val, uint32_t nsmps) {
    for (uint32_t i = 0; i < nsmps; i++) {
        buf[i] *= val;
    }
}

static inline void finite_check(float* buf, uint32_t nsmps) {
    // check finite (no NaN/Inf)
    for (uint32_t i = 0; i < nsmps; i++) {
        munit_assert(!isnan(buf[i]));
        munit_assert(!isinf(buf[i]));
    }
}

static inline void energy_check(float* buf, float upper_bound, uint32_t nsmps) {
    // assert there are no explosions in the buffer by checking against some upper
    // bound.
    float energy = 0.0f;
    for (uint32_t i = 0; i < nsmps; i++)
        energy += buf[i] * buf[i];
    munit_assert_double(energy, <, upper_bound);
}

static inline void decay_check(float* buf, uint32_t nsmps) {
    // assert the signal decays
    float eps = 0.1 + 1e-6;  // hardcoding for now
    float peak_first = fabs(buf[0]);
    float peak_last = fabs(buf[nsmps - 1]);
    munit_assert_double(peak_last, <, peak_first * eps);
}

static inline void check_any_nonzero(float* buf, uint32_t nsmps) {
    bool any_nonzero = false;
    for (uint32_t i = 0; i < nsmps; i++) {
        if (fabs(buf[i]) > 1e-7f) {
            any_nonzero = true;
            break;
        }
    }
    munit_assert_true(any_nonzero);
}

#ifdef __cplusplus
}
#endif

#endif
