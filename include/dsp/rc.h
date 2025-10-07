/**
 * @file rc.h
 * @brief RC one pole and ladder (LP) filters.
 *
 * Based on Zavalishin and Pirkle.
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
    float freq, sr;
    double lp_, hp_, ap_, t_, t_over_2_, two_over_t_, g_, z0_;
    // TODO: pad
} rc_one_pole;

/**
 * @brief initialize an rc_one_pole.
 */
void rc_one_pole_init(rc_one_pole* self, float freq, float sr);

/**
 * @brief tick one block. Will write to 1 or all 3 output vectors if presented.
 */
void rc_one_pole_tick_block(rc_one_pole* self,
                            float* in,
                            float* out_lp,
                            float* out_hp,
                            float* out_ap,
                            float* freq,
                            uint32_t sz);

/**
 * @brief 4 pole rc ladder lowpass (Zavalishin / Pirkle)
 */
typedef struct {
    // public
    float freq, q, sr;

    // private
    double t_, t_over_2_, two_over_t_, k_, g_, g2_, g3_, g4_;
    rc_one_pole bank_[4];
} rc_ladder;

/**
 * @brief initialize rc_ladder.
 */
void rc_ladder_init(rc_ladder* self, float freq, float q, float sr);

/**
 * @brief tick one block of the rc_ladder.
 */
void rc_ladder_tick_block(rc_ladder* self,
                          float* out,
                          float* in,
                          float* freq,
                          float* q,
                          uint32_t nsmps);

#ifdef __cplusplus
}
#endif

#endif
