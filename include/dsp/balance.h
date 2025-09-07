/**
 * @brief - balance - signal rescaling
 *  - the RMS of
 */
#ifndef DSP_BALANCE_H
#define DSP_BALANCE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/bq.h>

typedef struct {
    float sr;
    // private
    float d0_, d1_, coef_, scale_;  // lpf mem and scale factor
} balance;

void balance_init(balance* self, float sr);

void balance_tick_block(balance* self, float* out, float* cmp, uint32_t nsmps);

#ifdef __cplusplus
}
#endif

#endif
