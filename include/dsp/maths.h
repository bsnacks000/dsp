/**
 * @file maths.h
 * @brief basic maths on signals.
 */
#ifndef DSP_MATHS_H
#define DSP_MATHS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

#include <dsp/constants.h>
#include <dsp/utils.h>

// TODO: add const correctness

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
        out[i] = fabs(x[i]);
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

#ifdef __cplusplus
}
#endif
#endif
