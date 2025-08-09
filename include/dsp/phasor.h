/**
 * @brief a unipolar phasor. The phase and incr are double precision to avoid drift.
 * TODO:
 */
#ifndef DSP_PHASOR_H
#define DSP_PHASOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    float freq, sr;
    // private
    double incr_, phase_;  // use double internally for better wrap around precision
} phasor;

void phasor_init(phasor* self, float freq, float sr);

void phasor_tick_block(phasor* self, float* out, float* freq, uint32_t sz);

#ifdef __cplusplus
}
#endif

#endif
