/**
 * @file maths.h
 * @brief basic maths on signals.
 */
#ifndef DSP_MATHS_H
#define DSP_MATHS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include <dsp/utils.h>

// constants for float and double

#define DSP_PI 3.14159265358979323846
#define DSP_PI_F 3.14159274101257324219f

#define DSP_TWO_PI 6.28318530717958647692
#define DSP_TWO_PI_F 6.28318548202514648438f

#define DSP_HALF_PI_F 1.57079637050628662109f
#define DSP_QTR_PI_F 0.78539818525314331055f

#define DSP_SQRT_TWO 1.41421356237309504880
#define DSP_E 2.71828182845904523536

#define DSP_PI_SQUARED_F 9.86960506439208984375f

// avoids tangent asymptote pi/2
#define TANGENT_THRESHOLD_F 1.4922565104551517f

// TODO: const correct variants? vectorized/unrolled variants?

/**
 * @brief add two blocks
 */
static inline void add_block(float* out,
                             float* x,
                             float* y,
                             uint32_t start,
                             uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = x[i] + y[i];
    }
}
/**
 * @brief mult two blocks
 */
static inline void mult_block(float* out,
                              float* x,
                              float* y,
                              uint32_t start,
                              uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = x[i] * y[i];
    }
}

/**
 * @brief subtract two blocks
 */
static inline void subtract_block(float* out,
                                  float* x,
                                  float* y,
                                  uint32_t start,
                                  uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = x[i] - y[i];
    }
}

/**
 * @brief divide two blocks
 */
static inline void div_block(float* out,
                             float* x,
                             float* y,
                             uint32_t start,
                             uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = x[i] / y[i];
    }
}

// TODO: replace stdlib rand() with more efficient rand number generator

/**
 * @brief return 1 if xn > 0, -1 if xn < 0 and 0 otherwise
 */
static inline float sign_of(float xn) {
    return (float) ((float) (xn > 0) - (float) (xn < 0));
}

static inline float rand_unipolar(void) {
    return (float) rand() / (float) RAND_MAX;
}

static inline float rand_bipolar(void) {
    return ((float) rand() / (float) RAND_MAX) * 2.0f - 1.0f;
}

// // inverversions for unipolar / bipolar signals

/**
 * @brief phase invert a single sample
 */
static inline float phase_invert(float x) {
    return -x;
}

/**
 * @brief phase invert a block
 */
static inline void phase_invert_block(float* out,
                                      float* x,
                                      uint32_t start,
                                      uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = phase_invert(x[i]);
    }
}

/**
 * @brief invert over linear range using (a+b) -x
 */
static inline float range_invert(float x, float a, float b) {
    return (a + b) - x;
}

/**
 * @brief invert a unipolar signal (0,1) -> (1,0)
 */
#define invert_unipolar(x) range_invert(x, 0.0f, 1.0f)

/**
 * @brief invert a bipolar signal (-1,1) -> (1,-1)
 */
#define invert_bipolar(x) range_invert(x, -1.0f, 1.0f)

/**
 * @brief invert a unipolar block
 */
static inline void invert_unipolar_block(float* out,
                                         float* x,
                                         uint32_t start,
                                         uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = invert_unipolar(x[i]);
    }
}

/**
 * @brief invert a bipolar block
 */
static inline void invert_bipolar_block(float* out,
                                        float* x,
                                        uint32_t start,
                                        uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = invert_bipolar(x[i]);
    }
}

/**
 * @brief random bipolar number (-1, 1)
 */
static inline void noise_block(float* out, uint32_t start, uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = rand_bipolar();
    }
}

/**
 * @brief scale a block by a factor (*)
 */
static inline void scale_block(float* out,
                               float* x,
                               float factor,
                               uint32_t start,
                               uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = x[i] * factor;
    }
}

/**
 * @brief add dc offset for a block (+)
 */
static inline void dc_block(float* out,
                            float* x,
                            float factor,
                            uint32_t start,
                            uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = x[i] + factor;
    }
}

/**
 * @brief abs(*x)
 */
static inline void abs_block(float* out, float* x, uint32_t start, uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = fabsf(x[i]);
    }
}

// various logic gates
//
//

/**
 * @brief *x > *y
 */
static inline void gt_block(float* out,
                            float* x,
                            float* y,
                            uint32_t start,
                            uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = (float) (x[i] > y[i]);
    }
}

/**
 * @brief *x > y
 */
static inline void gt_scalar_block(float* out,
                                   float* x,
                                   float y,
                                   uint32_t start,
                                   uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = (float) (x[i] > y);
    }
}

/**
 * @brief *x >= *y
 */
static inline void gte_block(float* out,
                             float* x,
                             float* y,
                             uint32_t start,
                             uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = (float) (x[i] >= y[i]);
    }
}

/**
 * @brief *x >= y
 */
static inline void gte_scalar_block(float* out,
                                    float* x,
                                    float y,
                                    uint32_t start,
                                    uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = (float) (x[i] >= y);
    }
}

/**
 * @brief *x < *y
 */
static inline void lt_block(float* out,
                            float* x,
                            float* y,
                            uint32_t start,
                            uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = (float) (x[i] < y[i]);
    }
}

/**
 * @brief *x < y
 */
static inline void lt_scalar_block(float* out,
                                   float* x,
                                   float y,
                                   uint32_t start,
                                   uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = (float) (x[i] < y);
    }
}

/**
 * @brief *x <= *y
 */
static inline void lte_block(float* out,
                             float* x,
                             float* y,
                             uint32_t start,
                             uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = (float) (x[i] <= y[i]);
    }
}

/**
 * @brief *x <= y
 */
static inline void lte_scalar_block(float* out,
                                    float* x,
                                    float y,
                                    uint32_t start,
                                    uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = (float) (x[i] <= y);
    }
}

/**
 * @brief *x == *y
 */
static inline void ee_block(float* out,
                            float* x,
                            float* y,
                            uint32_t start,
                            uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = (float) check_float_equal(x[i], y[i]);
    }
}

/**
 * @brief *x == y
 */
static inline void ee_scalar_block(float* out,
                                   float* x,
                                   float y,
                                   uint32_t start,
                                   uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = (float) check_float_equal(x[i], y);
    }
}

/**
 * @brief *x != *y
 */
static inline void ne_block(float* out,
                            float* x,
                            float* y,
                            uint32_t start,
                            uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = (float) !check_float_equal(x[i], y[i]);
    }
}

/**
 * @brief *x != *y
 */
static inline void ne_scalar_block(float* out,
                                   float* x,
                                   float y,
                                   uint32_t start,
                                   uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = (float) !check_float_equal(x[i], y);
    }
}

/**
 * Fast trig functions sine/cos shaping using bhaskara's formulas.
 *  - expect phase (x) to be normalized between 0 and 1
 */

/**
 * @brief fast quarter sin
 */
static inline float fast_qsinf(float x) {
    float pi_m_x = DSP_PI_F - x;
    return (16.0f * x * pi_m_x) / (5.0f * DSP_PI_SQUARED_F - 4.0f * x * pi_m_x);
}

/**
 * @brief fast quarter cos
 */
static inline float fast_qcosf(float x) {
    return fast_qsinf(DSP_HALF_PI_F - x);
}

/**
 * @brief fast half sin
 */
static inline float fast_hsinf(float x) {
    float theta = x * 2.0f;
    int q = (int) theta;
    float qpos = theta - (float) q;

    qpos = (q & 1) ? 1.0f - qpos : qpos;
    theta = qpos * DSP_HALF_PI_F;

    return fast_qsinf(theta);
}

/**
 * @brief fast half cos
 */
static inline float fast_hcosf(float x) {
    return fast_hsinf(x + 0.5f);
}

/**
 * @brief fast sin
 */
static inline float fast_sinf(float x) {
    float theta = x * 4.0f;
    int q = (int) theta;
    float qpos = theta - (float) q;

    qpos = (q & 1) ? 1.0f - qpos : qpos;
    theta = qpos * DSP_HALF_PI_F;

    float sign = (q & 2) ? -1.0f : 1.0f;
    return sign * fast_qsinf(theta);
}

/**
 * @brief fast cosf;
 */
static inline float fast_cosf(float x) {
    return fast_sinf(x + 0.25f);
}

/**
 * @brief fast tanh.
 *  - x should be clamped [-1, 1]
 *  - See https://mathr.co.uk/blog/2017-09-06_approximating_hyperbolic_tangent.html
 */
static inline float fast_tanh(float x) {
    float xx = x * x;
    return x * (27.0f + xx) / (27.0f + 9.0f * xx);
}

/**
 * @brief fast tanh using doubles.
 */
static inline double fast_tanh_d(double x) {
    double xx = x * x;
    return x * (27.0 + xx) / (27.0 + 9.0 * xx);
}

/**
 * @brief a softsign function.
 *  - x should be clamped [-1, 1]
 */
static inline float softsign(float x) {
    return x / (1.0f + fabsf(x));
}

/**
 * @brief fast atan.
 *  - x should be clamped [-1,1]
 *  - Builder's method
 */
static inline float fast_atan(float x) {
    return DSP_QTR_PI_F * softsign(x);
}

// Non-linear waveshaping functions
// - clippers, clampers and saturators

/**
 * @brief clamp xn between min and max
 */
static inline float clamp(float xn, float min, float max) {
    if (xn > max)
        return max;
    else if (xn < min)
        return min;
    return xn;
}

static inline void clamp_block(float* out,
                               float* x,
                               float min,
                               float max,
                               uint32_t start,
                               uint32_t nsmps) {

    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = clamp(x[i], min, max);
    }
}

/**
 * @brief hard clip w/ drive amt.
 */
static inline float hard_clip(float xn, float amt) {
    amt += 1e-9f;
    return clamp(amt * xn, -1.0f, 1.0f) / clamp(amt, -1.0f, 1.0f);
}

static inline void hard_clip_block(float* out,
                                   float* x,
                                   float* amt,
                                   uint32_t start,
                                   uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = hard_clip(x[i], amt[i]);
    }
}

/**
 * @brief exp soft clip with pre-gain - from Pirkle via Reiss(2014)
 */
static inline float exp_clip(float xn, float pregain) {
    return sign_of(xn) * (1.0f - expf(-fabsf(pregain * xn)));
}

static inline void exp_clip_block(float* out,
                                  float* x,
                                  float* pregain,
                                  uint32_t start,
                                  uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = exp_clip(x[i], pregain[i]);
    }
}

/*
 * Zavalishin monotonic saturators - amt related to drive in the circuit.
 * - expensive bois but sound nice.
 * - amt controls the drive of the circuit
 */

/**
 * @brief hypertangent monotonic saturator
 */
static inline float tanh_clip(float xn, float amt) {
    amt += 1e-9f;
    return tanhf(amt * xn) / tanhf(amt);
}

static inline void tanh_clip_block(float* out,
                                   float* x,
                                   float* amt,
                                   uint32_t start,
                                   uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = tanh_clip(x[i], amt[i]);
    }
}

/**
 * @brief fast tanh saturator.
 */
static inline float fast_tanh_clip(float xn, float amt) {
    amt += 1e-9f;
    return fast_tanh(amt * xn) / fast_tanh(amt);
}

static inline void fast_tanh_clip_block(float* out,
                                        float* x,
                                        float* amt,
                                        uint32_t start,
                                        uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = fast_tanh_clip(x[i], amt[i]);
    }
}

/**
 * @brief fast tanh saturator using doubles
 */
static inline double fast_tanh_clip_d(double xn, double amt) {
    amt += 1e-9;
    return fast_tanh_d(amt * xn) / fast_tanh_d(amt);
}

/**
 * @brief arctangent monotonic saturator
 */
static inline float atan_clip(float xn, float amt) {
    amt += 1e-9f;
    return atanf(xn * amt) / atanf(amt);
}

static inline void atan_clip_block(float* out,
                                   float* x,
                                   float* amt,
                                   uint32_t start,
                                   uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = atan_clip(x[i], amt[i]);
    }
}

/**
 * @brief arctangent monotonic saturator
 */
static inline float fast_atan_clip(float xn, float amt) {
    amt += 1e-9f;
    return fast_atan(xn * amt) / fast_atan(amt);
}

static inline void fast_atan_clip_block(float* out,
                                        float* x,
                                        float* amt,
                                        uint32_t start,
                                        uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = fast_atan_clip(x[i], amt[i]);
    }
}

/// Various range mapping functions
///  - based on sc3 mappers.

/**
 * @brief map a value x from a linear range to a linear range.
 */
static inline float linlin(float x,
                           float src_lo,
                           float src_hi,
                           float dst_lo,
                           float dst_hi) {
    float denom = ((src_hi - src_lo) * (dst_hi - dst_lo)) + 1e-9f;
    return dst_lo + (x - src_lo) / denom;
}

/**
 * @brief map a value x from a linear range to an exponential range.
 */
static inline float linexp(float x,
                           float src_lo,
                           float src_hi,
                           float dst_lo,
                           float dst_hi) {

    dst_lo += 1e-9f;
    float denom = src_hi - src_lo + 1e-9f;
    float norm = (x - src_lo) / denom;

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

    src_lo += 1e-9f;
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
    src_lo += 1e-9f;
    float norm = logf(x / src_lo) / logf(src_hi / src_lo);
    return dst_lo * powf((dst_hi / dst_lo), norm);
}

#ifdef __cplusplus
}
#endif
#endif
