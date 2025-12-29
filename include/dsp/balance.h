/**
 * @brief balance - signal rescaling
 *  - use the RMS estimate of cmp signal to balance out
 *  - based on lazzarini - Audio Programming Book impl
 */
#ifndef DSP_BALANCE_H
#define DSP_BALANCE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    float sr;
    // private
    float d0_, d1_, coef_, scale_;  // lpf mem and scale factor
} balance;

/**
 * @brief balance init
 */
void balance_init(balance* self, float sr);

/**
 * @brief tick one block of balance.
 */
void balance_tick_block(balance* self,
                        float* out,
                        float* cmp,
                        uint32_t start,
                        uint32_t nsmps);

#ifdef __cplusplus
}
#endif

#endif
