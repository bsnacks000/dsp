/**
 * @file rmap.h
 * @brief range mapping functions
 *
 */

#ifndef DSP_RMAP_H
#define DSP_RMAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>

// TODO: block based versions: float* x

/**
 * @brief map a value x from a linear range to a linear range.
 */
static inline float linlin(float x,
                           float src_lo,
                           float src_hi,
                           float dst_lo,
                           float dst_hi) {
    return dst_lo + (x - src_lo) / (src_hi - src_lo) * (dst_hi - dst_lo);
}

/**
 * @brief map a value x from a linear range to an exponential range.
 */
static inline float linexp(float x,
                           float src_lo,
                           float src_hi,
                           float dst_lo,
                           float dst_hi) {
    // TODO: assertions
    float norm = (x - src_lo) / (src_hi - src_lo);
    return dst_lo * powf((dst_hi / dst_lo), norm);
}

/**
 * @brief map a value x from an exponential range to a linear range.
 */
static inline float explin(float x,
                           float src_lo,
                           float src_hi,
                           float dst_lo,
                           float dst_hi) {
    // TODO: assertions
    float norm = logf(x / src_lo) / logf(src_hi / src_lo);
    return dst_lo + norm * (dst_hi - dst_lo);
}

/**
 * @brief map a value x from an exponential range to an exponential range.
 */
static inline float expexp(float x,
                           float src_lo,
                           float src_hi,
                           float dst_lo,
                           float dst_hi) {
    // TODO: assertions
    float norm = logf(x / src_lo) / logf(src_hi / src_lo);
    return dst_lo * powf((dst_hi / dst_lo), norm);
}

#ifdef __cplusplus
}
#endif

#endif
