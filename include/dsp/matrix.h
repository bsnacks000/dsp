#ifndef DSP_MATRIX_H
#define DSP_MATRIX_H

#include <wchar.h>
#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/assert.h>
#include <dsp/shape.h>

#include <stdio.h>

/**
 * @brief a 2d matrix. data should be accessed in column major order.
 */
typedef struct {
    float* data;
    uint32_t n_rows, n_cols;
} matrix;

/**
 * @brief init a matrix
 */
static inline void matrix_init(matrix* self,
                               float* data,
                               uint32_t n_rows,
                               uint32_t n_cols) {
    self->data = data;
    self->n_rows = n_rows;
    self->n_cols = n_cols;
}

/**
 * @brief col major order accessor.
 */
static inline float matrix_at(matrix* self, uint32_t i, uint32_t j) {
    return self->data[i * self->n_cols + j];
}

/**
 * @brief col major order setter
 */
static inline void matrix_set(matrix* self, float val, uint32_t i, uint32_t j) {
    self->data[i * self->n_cols + j] = val;
}

/**
 * @brief return a pointer to a matrix row
 */
static inline float* matrix_get_row(matrix* self, uint32_t i) {
    return self->data + (i * self->n_cols);
}

/**
 * @brief fill a row of the matrix from a src buffer
 */
static inline void matrix_set_row(matrix* m,
                                  uint32_t row,
                                  const float* buf,
                                  uint32_t buf_sz) {
    for (uint32_t j = 0; j < buf_sz; j++) {
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
    for (uint32_t i = 0; i < a->n_rows; i++) {
        for (uint32_t j = 0; j < a->n_cols; j++) {
            float val = matrix_at(a, i, j);
            matrix_set(b, val, j, i);
        }
    }
}

/**
 * @brief print matrix to stdout. Each row is printed on its own line in csv form.
 */
static inline void matrix_print(matrix* self) {
    for (uint32_t row = 0; row < self->n_rows; row++) {
        for (uint32_t col = 0; col < self->n_cols; col++) {
            float val = matrix_at(self, row, col);
            if (col < self->n_cols - 1) {
                printf("%.10f, ", (double) val);
            } else {
                printf("%.10f\n", (double) val);
            }
        }
    }
}

typedef struct {
    float* low;
    float* high;
} frame_pair;

/**
 * @brief simple positional lookup. Returns row = i and row = i+1 given pos.
 *
 */
static inline frame_pair matrix_row_pair_positional_lookup(matrix* self, float pos) {

    // if matrix only has 1 row return it as both low and high
    // TODO: maybe use DSP_UNLIKEY here ..
    //   could move these to a check that gets called once at init time ?
    if (self->n_rows == 1) {
        return (frame_pair) {
            .low = matrix_get_row(self, 0),
            .high = matrix_get_row(self, 0),
        };
    }
    if (self->n_rows == 2) {
        return (frame_pair) {
            .low = matrix_get_row(self, 0),
            .high = matrix_get_row(self, 1),
        };
    }

    pos = clamp(pos, 0.0f, 1.0f);  // clamp [0-1]
    // pos = 0.5f - 0.5f * fast_hcosf(pos);  // smooth transition

    float fidx = pos * (float) (self->n_rows - 1);
    uint32_t idx = (uint32_t) fidx;

    // if we hit 1.0 set the last 2 band pair
    if (idx >= self->n_rows - 1)
        idx = (uint32_t) self->n_rows - 2;

    // printf("idx: %d idx+1: %d\n", idx, idx + 1);
    return (frame_pair) {
        .low = matrix_get_row(self, idx),
        .high = matrix_get_row(self, idx + 1),
    };
}

typedef struct {
    float* low;
    float* high;
    float f0_low;
    float f0_high;
} band_pair;

/**
 * @brief query the deck and return the correct band_pair given the freq. To provide
 * smooth frequency transition regions its on the caller to supply a deck in the
 * application layer that contains:
 *  - ftables sorted by max fundamental (wt->f0)
 *  - complete freq coverage up to nyquist across frames.
 *
 * NOTE: If there are gaps or unsorted regions this lookup will behave inconsistently.
 */
static inline band_pair matrix_row_pair_freq_lookup(matrix* self,
                                                    float freq,
                                                    float* bands) {
    // if matrix only has 1 row return it as both low and high
    // TODO: maybe use DSP_UNLIKEY here ..
    //   could move these to a check that gets called once at init time ?
    if (self->n_rows == 1) {
        return (band_pair) {
            .low = matrix_get_row(self, 0),
            .high = matrix_get_row(self, 0),
            .f0_low = 1.0f,
            .f0_high = 15000.0f,
        };
    }
    if (self->n_rows == 2) {
        return (band_pair) {
            .low = matrix_get_row(self, 0),
            .high = matrix_get_row(self, 1),
            .f0_low = bands[0],
            .f0_high = bands[1],
        };
    }
    // lower then first band
    if (freq < bands[0]) {
        return (band_pair) {
            .low = matrix_get_row(self, 0),
            .high = matrix_get_row(self, 1),
            .f0_low = bands[0],
            .f0_high = bands[1],
        };
    }

    // higher then last band
    if (freq > bands[self->n_rows - 1]) {
        return (band_pair) {
            .low = matrix_get_row(self, self->n_rows - 2),
            .high = matrix_get_row(self, self->n_rows - 1),
            .f0_low = bands[self->n_rows - 2],
            .f0_high = bands[self->n_rows - 1],
        };
    }

    // we *should* always fall within some frequency region..
    // its on caller to make sure the deck
    for (uint32_t i = 1; i < self->n_rows; i++) {
        if (bands[i - 1] <= freq && freq < bands[i]) {
            return (band_pair) {
                .low = matrix_get_row(self, i - 1),
                .high = matrix_get_row(self, i),
                .f0_low = bands[i - 1],
                .f0_high = bands[i],
            };
        }
    }

    // if we get here there are gaps in the freq bands so we just return
    // the first pair
    // TODO: warning (debug)
    return (band_pair) {
        .low = matrix_get_row(self, 0),
        .high = matrix_get_row(self, 1),
        .f0_low = bands[0],
        .f0_high = bands[1],
    };
}
#ifdef __cplusplus
}
#endif

#endif
