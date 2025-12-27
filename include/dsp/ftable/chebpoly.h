/**
 * @brief chebyshev nth-order polynomial waveshaper.
 * Based on Dodge/Jerse (5.2, 5.3) and csound gen13 (Pinkston)
 *  - the signal from wt->buf is clipped between  [-1, 1].
 *  - for typical waveshaping synthesis pass in linspace(-1, 1)
 *  - h can be arbitrarily large; yaxis is normalized between
 *  - the guard point carries over from the transfer function (no wrap)
 */

#ifndef DSP_CHEBYTAB_H
#define DSP_CHEBYTAB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/ftable/ftable.h>

/**
 * @brief state for ft_cheby_args
 */
typedef struct {
    const float* h;
    uint32_t h_sz;
} ft_chebpoly_args;

/**
 * @brief init ft_cheby_args
 */
dsp_err ft_chebpoly_args_init(ft_chebpoly_args* self, const float* h, uint32_t h_sz);

/**
 * @brief fill the wt with an nth order chebyshev polynomial series.
 */
dsp_err ft_chebpoly(ftable* wt, void* args);

/**
 * @brief generate a deck with a set of chebypoly args.
 */
dsp_err chebpoly_deck_generate(ftable** wt, ft_chebpoly_args** args, uint32_t n_bands);

#ifdef __cplusplus
}
#endif

#endif
