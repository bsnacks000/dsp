/**
 * @brief chebyshev nth-order polynomial waveshaper.
 * Based on Dodge/Jerse (5.2, 5.3) and csound gen13 (Pinkston)
 *  - the signal from wt->buf is clipped between  [-1, 1].
 *  - for typical waveshaping synthesis pass in linspace(-1, 1)
 *  - h can be arbitrarily large; yaxis is normalized between
 */

#ifndef DSP_CHEBYTAB_H
#define DSP_CHEBYTAB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/wavetable/wavetable.h>

/**
 * @brief state for wt_cheby_args
 */
typedef struct {
    const float* h;
    uint32_t h_sz;
} wt_chebpoly_args;

/**
 * @brief init wt_cheby_args
 */
dsp_err wt_chebpoly_args_init(wt_chebpoly_args* self, const float* h, uint32_t h_sz);

/**
 * @brief fill the wt with an nth order chebyshev polynomial series.
 */
dsp_err wt_chebpoly(wavetable* wt, void* args);

/**
 * @brief generate a deck with a set of chebypoly args.
 */
dsp_err chebpoly_deck_generate(wavetable** wt,
                               wt_chebpoly_args** args,
                               uint32_t n_bands);

#ifdef __cplusplus
}
#endif

#endif
