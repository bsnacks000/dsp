/**
 * @brief gated sample/hold and variants
 */

#ifndef DSP_SAMPH_H
#define DSP_SAMPH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    float gate_thresh;
    float curr_out_, curr_gate_, prev_gate_;
} samph;

/**
 * @brief samph init
 */
void samph_init(samph* self, float gate_thresh);

/**
 * @brief samph tick block. Triggered by gate signal.
 */
void samph_tick_block(samph* self,
                      float* out,
                      float* in,
                      float* gate,
                      float* gate_thresh,
                      uint32_t start,
                      uint32_t nsmps);

#ifdef __cplusplus
}
#endif

#endif
