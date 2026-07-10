/**
 * @file phasor.h
 * @brief a unipolar phasor ramp generator.
 *
 * The phase and incr are double precision to avoid timing drift.
 *
 * The impulse tick block produces 1 on wrap else 0 instead of a ramp.
 *
 * The rand impulse block jumps the phasor to produce stochastic impulses. This is
 * based loosely on Dust2 from sc3.
 *
 */

// SPDX-License-Identifier: MIT

#ifndef DSP_PHASOR_H
#define DSP_PHASOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    float freq, iphase, sr;
    // private
    double incr_, phase_;
} phasor;

/**
 * @brief initialize the phasor with an iphs between 0 and 1.
 *
 * */
void phasor_init(phasor* self, float freq, float iphs, float sr);

/**
 * @brief tick one block of phasor.
 *
 */
void phasor_tick_block(phasor* self,
                       float* out,
                       float* freq,
                       uint32_t start,
                       uint32_t nsmps);

/**
 * @brief a unipolar impulse generator. Outputs 1 when the phasor signal wraps.
 *
 */
void impulse_tick_block(phasor* self,
                        float* out,
                        float* freq,
                        uint32_t start,
                        uint32_t nsmps);

/**
 * @brief a random impulse generator. Clusters impulses around
 * an exp distribution using phase jumping. Produces clustered, stochastic
 * impulses with density roughly proportional to the base frequency.
 */
void rand_impulse_tick_block(phasor* self,
                             float* out,
                             float* freq,
                             uint32_t start,
                             uint32_t nsmps);

#ifdef __cplusplus
}
#endif

#endif
