/**
 * @file maths.h
 * @brief basic maths on signals.
 */
#ifndef DSP_MATHS_H
#define DSP_MATHS_H

#include "dsp/utils.h"
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

static inline float rand_unipolar(void) {
    return (float) rand() / (float) RAND_MAX;
}

static inline float rand_bipolar(void) {
    return ((float) rand() / (float) RAND_MAX) * 2.0 - 1.0;
}

/**
 * @brief random bipolar number (-1, 1)
 */
static inline void noise_block(float* out, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        out[i] = rand_bipolar();
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
 * @brief abs(*x)
 */
static inline void abs_block(float* out, float* x, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        out[i] = fabs(x[i]);
    }
}

// various logic gates
//
//

/**
 * @brief *x > *y
 */
static inline void gt_block(float* out, float* x, float* y, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        out[i] = (float) (x[i] > y[i]);
    }
}

/**
 * @brief *x > y
 */
static inline void gt_scalar_block(float* out, float* x, float y, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        out[i] = (float) (x[i] > y);
    }
}

/**
 * @brief *x >= *y
 */
static inline void gte_block(float* out, float* x, float* y, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        out[i] = (float) (x[i] >= y[i]);
    }
}

/**
 * @brief *x >= y
 */
static inline void gte_scalar_block(float* out, float* x, float y, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        out[i] = (float) (x[i] >= y);
    }
}

/**
 * @brief *x < *y
 */
static inline void lt_block(float* out, float* x, float* y, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        out[i] = (float) (x[i] < y[i]);
    }
}

/**
 * @brief *x < y
 */
static inline void lt_scalar_block(float* out, float* x, float y, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        out[i] = (float) (x[i] < y);
    }
}

/**
 * @brief *x <= *y
 */
static inline void lte_block(float* out, float* x, float* y, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        out[i] = (float) (x[i] <= y[i]);
    }
}

/**
 * @brief *x <= y
 */
static inline void lte_scalar_block(float* out, float* x, float y, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        out[i] = (float) (x[i] <= y);
    }
}

/**
 * @brief *x == *y
 */
static inline void ee_block(float* out, float* x, float* y, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        out[i] = (float) check_float_equal(x[i], y[i]);
    }
}

/**
 * @brief *x == y
 */
static inline void ee_scalar_block(float* out, float* x, float y, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        out[i] = (float) check_float_equal(x[i], y);
    }
}

/**
 * @brief *x != *y
 */
static inline void ne_block(float* out, float* x, float* y, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        out[i] = (float) !check_float_equal(x[i], y[i]);
    }
}

/**
 * @brief *x != *y
 */
static inline void ne_scalar_block(float* out, float* x, float y, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        out[i] = (float) !check_float_equal(x[i], y);
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

#ifdef __cplusplus
}
#endif
#endif
