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

#ifdef __cplusplus
}
#endif
#endif
