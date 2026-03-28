/**
 * @brief a block based delay line API. This is the traditional low level delay line
 * implementation found in most music-N languages. Taps should be located in between
 *
 */
#ifndef DSP_DELAY_H
#define DSP_DELAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    float* buf;
    uint32_t buf_sz, write_idx, mask_, wrap_sz_;
} delay_line;

/**
 * @brief A utility function to calculate the max delay_line given the sample
 * rate and the total desired time in ms. This function will calculate the next
 * highest pow2 sized table length for allocating the buffer.
 */
uint32_t delay_line_calculate_buf_sz(float sr, float ms);

/**
 * @brief initialize the delay line
 */
void delay_line_init(delay_line* self, float* buf, uint32_t buf_sz);

/**
 * @brief advance the read pointer of the delay_line by nsmps
 */
void delay_line_advance(delay_line* self, uint32_t nsmps);

/**
 * @brief write nsmps into the delay_line. This should be called sometime after
 * delay_line_advance in the graph.
 */
void delay_line_write(delay_line* self, float* in, uint32_t start, uint32_t nsmps);

/**
 * @brief read using linear interpolation at fractional offset index. This should be
 * in between delay_line_advance and delay_line_write in the graph.
 */
void delay_line_tapi(delay_line* self,
                     float* out,
                     float* offset,
                     uint32_t start,
                     uint32_t nsmps);

/**
 * @brief read using cubic interpolation at fractional offset index. This should be
 * called in between delay_line_advance and delay_line_write in the graph.
 */
void delay_line_tap3(delay_line* self,
                     float* out,
                     float* offset,
                     uint32_t start,
                     uint32_t nsmps);

/**
 * @brief a one sample delay. z^-1
 */
typedef struct {
    float z1_;
} z1_delay;

/**
 * @brief z1_delay_init
 */
static inline void z1_delay_init(z1_delay* self) {

    self->z1_ = 0.0f;
}

/**
 * @brief z1_delay_tick_block
 */
static inline void z1_delay_tick_block(z1_delay* self,
                                       float* out,
                                       float* in,
                                       uint32_t start,
                                       uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = self->z1_;
        self->z1_ = in[i];
    }
}

#ifdef __cplusplus
}
#endif

#endif
