#ifndef DSP_CHEBPOLY_H
#define DSP_CHEBPOLY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/assert.h>
#include <dsp/utils.h>
#include <stdint.h>

/**
 * Calculate the chebyshev polynomial of x using harmonic weights h using the
 * standard recurrence relation. Assumes at least h_sz == 1
 */
static inline float chebyshev_polynomial(float x, const float* h, uint32_t h_sz) {
    dsp_assert(h_sz >= 1, "h_sz must be >= 1");

    uint32_t n = h_sz - 1;
    float s = 0.5f * h[0];
    if (n == 0) {
        return s;
    }

    float t_nm1 = 1.0;
    float t_n = x;

    s += h[1] * t_n;

    for (uint32_t k = 2; k < n + 1; k++) {
        float t_k = 2.0f * x * t_n - t_nm1;
        s += h[k] * t_k;
        t_nm1 = t_n;
        t_n = t_k;
    }
    return s;
}

// XXX: refactor so with better semantics ... should take an in_sig instead of
//  writing out in place...

/**
 * @brief fill an out buf of arbitrary size with a chebyshev polynomial calculation
 */
static inline void chebyshev_fill(float* out,
                                  uint32_t out_sz,
                                  float* h,
                                  uint32_t h_sz) {

    for (uint32_t i = 0; i < out_sz; i++) {
        out[i] = chebyshev_polynomial(out[i], h, h_sz);
    }

    normalize(out, out_sz);
}

#ifdef __cplusplus
}
#endif

#endif
