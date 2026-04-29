/**
 * @file ceiling_pow2.h
 * @author bsnacks000
 * @brief helper functions
 * @version 0.1
 * @date 2025-02-16
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef DSP_UTILS_H
#define DSP_UTILS_H

#ifdef __cpluplus
extern "C" {
#endif

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define dsp_min(x, y) ((x) < (y) ? (x) : (y))
#define dsp_max(x, y) ((x) > (y) ? (x) : (y))

/**
 * @brief stages for an AR envelope.
 */
typedef enum {
    AR_IDLE = 0,
    AR_ATK,
    AR_REL,
} ar_stage;

/**
 * @brief stages for an ADSR envelope.
 */
typedef enum {
    ADSR_IDLE = 0,
    ADSR_ATK,
    ADSR_DCY,
    ADSR_SUS,
    ADSR_REL,
} adsr_stage;

/**
 * @brief library error codes.
 */
typedef enum { DSP_OK = 0, DSP_ERR = 1 } dsp_err;

/**
 * @brief copy nsmps of in to out (memcpy)
 */
static inline void copy_nsmps(float* out,
                              const float* in,
                              uint32_t start,
                              uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++)
        out[i] = in[i];
}

/**
 * @brief sets nsmps of x into the buffer.
 */
static inline void set_nsmps(float* out, float x, uint32_t nsmps) {
    for (uint32_t i = 0; i < nsmps; i++)
        out[i] = x;
}

/**
 * @brief zero out the x buffer (memset)
 */
static inline void zero_buf(float* buf, uint32_t buf_sz) {
    memset(buf, 0, buf_sz * sizeof(float));
}

/**
 * @brief branchless wrap float over range.
 */
static inline float wrap_float_range(float x, float min, float max) {
    float range = max - min;
    float wrapped = fmodf(x - min, range);
    return wrapped + range * (wrapped < 0.0f) + min;
}

/**
 * @ brief branchless wrap float over 0 <= x < n
 */
static inline float wrap_float_positive(float x, float n) {
    // TODO: assert 0 <= x < n
    return fmodf(fmodf(x, n) + n, n);
}

// TODO implement branched wrap using likely/unlikely. Better for parameter checks when
// values are likely to always be in range.

/**
 * @brief Given n return the uint32_t next highest power of 2
 */
static inline float_t ceiling_pow2(float n) {
    return powf(2.0f, ceilf(log2f(n)));
}

/**
 * @brief Check if n is a power of 2
 */
static inline bool is_pow2(uint32_t n) {
    return (n & (n - 1)) == 0;
}

/**
 * @brief Check these two floats are appx equal to a tol of 1e-5f.
 *
 */
static inline bool check_float_equal(float a, float b) {
    float x = a - b;
    return (x <= 1e-5f) && (x >= -1e-5f);
}

/**
 * @brief Determine if the float has a fractional part
 */
static inline bool has_fractional_part(float x) {
    float int_part;
    return modff(x, &int_part) != 0.0f;
}

/**
 * @brief calculate the fractional number of samples given the duration
 * and sample rate.
 */
static inline float nsmps_dur(float sr, float dur_sec) {
    return sr * fabsf(dur_sec);
}

/**
 * @brief a signal guard for exponentials. Adds a small epsilon in case we hit zero.
 */
static inline float zero_guard(float xn) {
    return xn + 1e-9f;
}

/**
 * @brief calculate a semitone ratio
 */
static inline float semitone_ratio(float semitones) {
    return powf(2.0f, semitones / 12.0f);
}

/**
 * @brief normalize the values in buf
 */
static inline void normalize(float* buf, uint32_t buf_sz) {
    float max = 0.0;
    for (uint32_t i = 0; i < buf_sz; i++) {
        float a = fabsf(buf[i]);  // use absolute value (handle bipolar signals)
        if (a > max) {
            max = a;
        }
    }
    if ((int) max) {
        float s = 1.0f / max;
        for (uint32_t i = 0; i < buf_sz; i++) {
            buf[i] *= s;
        }
    }
}

#ifdef __cpluplus
}
#endif

#endif
