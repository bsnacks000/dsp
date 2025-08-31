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
    float freq, iphase, sr;
    // private
    double incr_, phase_;  // use double internally for better wrap around precision
} phasor;

/**
 * @brief init the phasor. iphs is guaranteed wrapped betwen 0 and 1
 */
void phasor_init(phasor* self, float freq, float iphs, float sr);

/**
 * @brief tick one block of phasor.
 */
void phasor_tick_block(phasor* self, float* out, float* freq, uint32_t sz);

#ifdef __cplusplus
}
#endif

#endif
