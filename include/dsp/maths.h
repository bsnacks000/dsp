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

/**
 * @brief add two blocks
 */
static inline void add_block(float* out, float* x, float* y, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        out[i] = x[i] + y[i];
    }
}
/**
 * @brief mult two blocks
 */
static inline void mult_block(float* out, float* x, float* y, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        out[i] = x[i] * y[i];
    }
}

/**
 * @brief subtract two blocks
 */
static inline void subtract_block(float* out, float* x, float* y, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        out[i] = x[i] - y[i];
    }
}

/**
 * @brief divide two blocks
 */
static inline void div_block(float* out, float* x, float* y, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        out[i] = x[i] / y[i];
    }
}

/**
 * @brief random bipolar number (-1, 1)
 */
static inline void noise_block(float* out, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        out[i] = ((float) rand() / (float) RAND_MAX) * 2.0 - 1.0;
    }
}

/**
 * @brief scale a block by a factor (*)
 */
static inline void scale(float* out, float* x, float factor, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        out[i] = x[i] * factor;
    }
}

/**
 * @brief add dc offset for a block (+)
 */
static inline void dc_offset(float* out, float* x, float factor, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        out[i] = x[i] + factor;
    }
}

/**
 * @brief a 2d matrix. data should be accessed in column major order.
 */
typedef struct {
    float* data;
    size_t n_rows, n_cols;
} matrix;

/**
 * @brief init a matrix
 */
static inline void matrix_init(matrix* self,
                               float* data,
                               size_t n_rows,
                               size_t n_cols) {
    self->data = data;
    self->n_rows = n_rows;
    self->n_cols = n_cols;
}

/**
 * @brief col major order accessor.
 */
static inline float matrix_at(matrix* self, size_t i, size_t j) {
    return self->data[i * self->n_cols + j];
}

/**
 * @brief col major order setter
 */
static inline void matrix_set(matrix* self, float val, size_t i, size_t j) {
    self->data[i * self->n_cols + j] = val;
}

/**
 * @brief return a pointer to a matrix row
 */
static inline float* matrix_get_row(matrix* self, size_t i) {
    return self->data + (i * self->n_cols);
}

/**
 * @brief fill a row of the matrix from a src buffer
 */
static inline void matrix_set_row(matrix* m,
                                  size_t row,
                                  const float* buf,
                                  size_t buf_sz) {
    for (size_t j = 0; j < buf_sz; j++) {
        matrix_set(m, buf[j], row, j);
    }
}

/**
 * @brief transpose a into b (a.T)
 *  conditions:
 *  - in->n_rows == out->n_cols
 *  - in->n_cols == out->n_rows;
 */
static inline void matrix_transpose(matrix* b, matrix* a) {
    for (size_t i = 0; i < a->n_rows; i++) {
        for (size_t j = 0; j < a->n_cols; j++) {
            float val = matrix_at(a, i, j);
            matrix_set(b, val, j, i);
        }
    }
}

// TODO: more matrix ops as needed?

// bhaskara fast sin/cos ops - expect phase x (between 0 and 1)

/**
 * @brief fast quarter sin
 */
static inline float fast_qsinf(float x) {
    float pi_m_x = DSP_PI - x;
    return (16.0 * x * pi_m_x) / (5.0 * DSP_PI_SQUARED - 4.0 * x * pi_m_x);
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
    float theta = x * 2.0;
    int q = (int) theta;
    float qpos = theta - q;

    qpos = (q & 1) ? 1.0 - qpos : qpos;
    theta = qpos * HALF_PI;

    return fast_qsinf(theta);
}

/**
 * @brief fast half cos
 */
static inline float fast_hcosf(float x) {
    return fast_hsinf(x + 0.5);
}

/**
 * @brief fast sin
 */
static inline float fast_sinf(float x) {
    float theta = x * 4.0;
    int q = (int) theta;
    float qpos = theta - q;

    qpos = (q & 1) ? 1.0 - qpos : qpos;
    theta = qpos * HALF_PI;

    float sign = (q & 2) ? -1.0 : 1.0;
    return sign * fast_qsinf(theta);
}

/**
 * @brief fast cosf;
 */
static inline float fast_cosf(float x) {
    return fast_sinf(x + 0.25);
}

#ifdef __cplusplus
}
#endif
#endif
