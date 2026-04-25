/**
 * @brief nonlinear waveshaping functions
 */
#ifndef DSP_SHAPE_H
#define DSP_SHAPE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdint.h>

#include <dsp/fasttrig.h>
#include <dsp/maths.h>

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

#ifdef __cplusplus
}
#endif
#endif
