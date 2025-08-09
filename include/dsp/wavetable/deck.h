/**
 * @brief wavetable decks for various utilities
 *  - sinesum (band limited / novel waveforms)
 *  - TODO: interpolation / spectral morph (based on serum2)
 *  - TODO: wrapers for other wavetable module functionality (cheby, window)
 *
 */

#ifndef DSP_DECK_H
#define DSP_DECK_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/maths.h>
#include <dsp/rmap.h>
#include <dsp/wavetable/sinesum.h>
#include <dsp/wavetable/wavetable.h>

/**
 * @brief wt_deck is fat pointer type that simply holds an array of wavetable pointers.
 * Its lookup strategy is based on position. We borrow terminology from serum and other
 * wt synths and call these "frames".
 */
typedef struct {
    wavetable** frames;
    uint32_t frames_sz;  // nrows
} wt_deck;

/**
 * @brief init a wt_deck. We check that frames is at least 2 so that we can iterate
 * and determine if all frames in the deck are equal.
 */
void wt_deck_init(wt_deck* self, wavetable** frames, uint32_t frames_sz);

/**
 * @brief check if all frames are of equal length. Usually this is the case when we're
 * using decks, but there could be impls that might want decks with frames of different
 * sizes.
 */
bool wt_deck_frames_equal(wt_deck* self);

/**
 * @brief fill a matrix from the underlying frames ptr. All frames must be equally
 * sized and the matrix should be initialized with the correct buffer
 */
dsp_err wt_deck_matrix_fill(wt_deck* self, matrix* out);

#ifdef __cplusplus
}
#endif

#endif
