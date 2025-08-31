/**
 * @file interpolation.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-02-16
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef DSP_INTERPOLATION_H
#define DSP_INTERPOLATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/assert.h>
#include <math.h>
#include <stdlib.h>

/**
 * TODO: block based stateful interpolation. For use with signals not tables.
 * Those should be broken out into a module but can use these one ticks under the
 * hood.
 */

/**
 * @brief Perform a linear interpolation for table lookup.
 *
 * @param a  y coordinate of first point
 * @param b  y coordinate of second point
 * @param frac  fractional distance between x1 and x2
 * @return float
 */
static inline float interpolate_linear(float a, float b, float frac) {
    return frac * a + (1.0 - frac) * b;
}

/**
 * @brief Cubic Interpolation scheme. From Lazzarinni in Audio Programming Book.
 *
 */
static inline float interpolate_cubic(float a, float b, float c, float d, float frac) {
    float tmp = d + 3.0 * b;
    float fracsq = frac * frac;
    float fracb = frac * fracsq;

    // # ax^3 + bx^2 + cx + d
    return (fracb * (-a - 3.0 * c + tmp) / 6.0 + fracsq * ((a + c) / 2.0 - b) +
            frac * (c + (-2.0 * a - tmp) / 6.0) + b);
}

/**
 * @brief linear interpolation based on numpy.interp.
 */
static inline void npinterp(float* out, size_t out_sz, const float* in, size_t in_sz) {
    float dx = 1.0f / (in_sz - 1);   // spacing between original points
    float dt = 1.0f / (out_sz - 1);  // spacing in target space

    for (size_t i = 0; i < out_sz; i++) {
        if (i == out_sz - 1) {
            out[i] = in[in_sz - 1];
        } else {
            float t = i * dt;  // interpolation factor

            // Handle bounds
            if (t <= 0.0f) {
                out[i] = in[0];
                continue;

            } else if (t >= 1.0f) {
                out[i] = in[in_sz - 1];
                continue;
            }

            float pos = t / dx;
            size_t idx = (size_t) floorf(pos);
            float frac = pos - idx;

            out[i] = (1.0f - frac) * in[idx] + frac * in[idx + 1];
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
