/**
 * @brief chebpoly - Fill a table with chebyshev polynomials of the Nth degree.
 *  - primarily for waveshaping synthesis purposes.
 *  - Based on various sources - csound / sc3 / Dodge + Jerse
 *
 */

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

/**
 * @brief fill an out buf of arbitrary size with a chebyshev polynomial calculation.
 *  - h provides coefficients of degree N.
 *  - in can be any waveform but is typically a straight line.
 *
 *  NOTE: that out is overwritten and is therefore not cleared before a write.
 */
static inline void chebyshev_fill(float* out,
                                  float* in,
                                  uint32_t buf_sz,
                                  float* h,
                                  uint32_t h_sz) {

    for (uint32_t i = 0; i < buf_sz; i++) {
        out[i] = chebyshev_polynomial(in[i], h, h_sz);
    }

    normalize(out, buf_sz);
}

#ifdef __cplusplus
}
#endif

#endif
