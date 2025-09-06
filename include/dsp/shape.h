/**
 * @brief nonlinear waveshaping functions
 */
#ifndef DSP_SHAPE_H
#define DSP_SHAPE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/maths.h>
#include <math.h>

/**
 * @brief return 1 if xn > 0, -1 if xn < 0 and 0 otherwise
 */
static inline float sign_of(float xn) {
    return (float) ((float) (xn > 0) - (float) (xn < 0));
}

/**
 * @brief hard clip to threshold
 */
static inline float hard_clip(float xn, float threshold) {
    if (xn > threshold)
        return threshold;
    else if (xn < -threshold)
        return -threshold;
    return xn;
}

/**
 * @brief soft clip with pre-gain - from Pirkle via Reiss(2014)
 */
static inline float soft_clip(float xn, float amt) {
    return sign_of(xn) * (1.0 - expf(-fabs(amt * xn)));
}

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

/*
 * Zavalishin monotonic saturators - amt related to drive in the circuit.
 * - expensive bois but sound nice.
 * - amt controls the drive of the circuit
 */

/**
 * @brief hypertangent monotonic saturator
 */
static inline float hyptan_saturator(float xn, float amt) {
    amt += 1e-9;
    return tanh(amt * xn) / tanh(amt);
}

/**
 * @brief arctangent monotonic saturator
 */
static inline float arctan_saturator(float xn, float amt) {
    amt += 1e-9;
    return atan(xn * amt) / atan(amt);
}

/**
 * Cheap saturators (expander / compander curves)
 * - amt drives pre-gain (NOTE: may change this to drive)
 */

// TODO: sin_arctan, parabolic, hyperbolic, hyperbolic_sin, inverse_hyperbolic

#ifdef __cplusplus
}
#endif
#endif
