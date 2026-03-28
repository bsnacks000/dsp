/**
 * @file ftrig.h
 * @brief fast sine/cos shaping using bhaskara's formulas.
 *  - expect phase x (between 0 and 1)
 */
#ifndef DSP_FASTTRIG_H
#define DSP_FASTTRIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/constants.h>

/**
 * @brief fast quarter sin
 */
static inline float fast_qsinf(float x) {
    float pi_m_x = DSP_PI - x;
    return (16.0f * x * pi_m_x) / (5.0f * DSP_PI_SQUARED - 4.0f * x * pi_m_x);
}

/**
 * @brief fast quarter cos
 */
static inline float fast_qcosf(float x) {
    return fast_qsinf(HALF_PI - x);
}

/**
 * @brief fast half sin
 */
static inline float fast_hsinf(float x) {
    float theta = x * 2.0f;
    int q = (int) theta;
    float qpos = theta - q;

    qpos = (q & 1) ? 1.0f - qpos : qpos;
    theta = qpos * HALF_PI;

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
    float qpos = theta - q;

    qpos = (q & 1) ? 1.0f - qpos : qpos;
    theta = qpos * HALF_PI;

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
 * @brief a softsign function.
 *  - x should be clamped [-1, 1]
 */
static inline float softsign(float x) {
    return x / (1.0f + fabs(x));
}

/**
 * @brief fast atan.
 *  - x should be clamped [-1,1]
 *  - Builder's method
 */
static inline float fast_atan(float x) {
    return QRTR_PI * softsign(x);
}

#ifdef __cplusplus
}
#endif

#endif
