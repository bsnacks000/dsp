/**
 * @file samph.h
 * @brief gated sample/hold
 *
 */

// SPDX-License-Identifier: MIT

#ifndef DSP_SAMPH_H
#define DSP_SAMPH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief samph state.
 * */
typedef struct {
    float gate_thresh;
    float curr_out_, curr_gate_, prev_gate_;
} samph;

// TODO: do we need gate_thresh in ctor if its a signal?

/**
 * @brief samph init.
 * @param gate_thresh sets the initial threshold to trigger the gate.
 */
void samph_init(samph* self, float gate_thresh);

/**
 * @brief samph tick block. Captures the instantaneous value of input when the gate
 * signal rises above threshold
 * @param in input signal
 * @param gate gate signal usually between 0 and 1 though could be anything.
 * @param gate_thresh signal that controls the threshold level to trigger a change.
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
