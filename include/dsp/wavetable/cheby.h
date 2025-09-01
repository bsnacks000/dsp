/**
 * @brief chebyshev nth-order polynomial waveshaper
 *  - the signal from wt->buf is clipped between  [-1, 1].
 *      - high res pow2 is a good choice for resolution (>1024)
 *      - to create a standalone waveshaping typically we send in linspace(-1,1)
 *  - coeffs can be arbitrarily large and typically range [0,1]
 *  - gain impacts strength and polarity of the polynomial
 */

#ifndef DSP_CHEBYTAB_H
#define DSP_CHEBYTAB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/wavetable/wavetable.h>

typedef struct {
    const float* coeffs;
    uint32_t coeffs_sz;
    float gain;
} wt_cheby_args;

/**
 * @brief fill the wt with an nth order chebyshev polynomial.
 */
dsp_err wt_cheby(wavetable* wt, void* args);

#ifdef __cplusplus
}
#endif

#endif
