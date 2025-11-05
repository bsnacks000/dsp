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
#include <dsp/xfade.h>

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

/**
 * @brief represents a low/high pair. The tables used for the actual crossfade.
 */
typedef struct {
    wavetable* low;
    wavetable* high;
} wt_frame_pair;

static inline wt_frame_pair wt_deck_pos_lookup(wt_deck* self, float pos) {
    pos = clamp(pos, 0.0f, 1.0f);  // ensure [0, 1]
    pos = 0.5f - 0.5f * cosf(pos * M_PI);

    float fidx = pos * (self->frames_sz - 1);  // range: [0, N-1]
    uint32_t idx = (int) fidx;

    if (idx >= self->frames_sz - 1) {
        idx = self->frames_sz - 2;
        fidx = (float) idx;
    }

    return (wt_frame_pair) {
        .low = self->frames[idx],
        .high = self->frames[idx + 1],
    };
}

#ifdef __cplusplus
}
#endif

#endif
