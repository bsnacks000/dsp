/**
 * @file rc.h
 * @brief RC one pole filters.
 *
 * Based on Zavalishin: Art of VA filter design.
 *
 * Implemented as a multi-mode filter.
 *
 *
 */

#ifndef DSP_RC_H
#define DSP_RC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief rc_onle_pole state
 */
typedef struct {
    // public
    float sr, freq, lp, hp, ap;
    // private
    float t_, t_over_2_, two_over_t_, g_, z0_;
    // TODO: pad
} rc_one_pole;

/**
 * @brief initialize an rc_one_pole.
 */
void rc_one_pole_init(rc_one_pole* self, float freq, float sr);

/**
 * @brief tick one block. Will write to 1 or all 3 output signal vectors depending
 *  on rc_mode.
 */
void rc_one_pole_tick_block(rc_one_pole* self,
                            float* in,
                            float* lp_out,
                            float* hp_out,
                            float* ap_out,
                            float* freq,
                            uint32_t sz);

#ifdef __cplusplus
}
#endif

#endif
