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
void wt_deck_init(wt_deck* self, wavetable** frames, uint32_t frames_sz) {
    self->frames = frames;
    self->frames_sz = frames_sz;
}

/**
 * @brief check if all frames are of equal length. Usually this is the case when we're
 * using decks, but there could be impls that might want decks with frames of different
 * sizes.
 */
bool wt_deck_frames_equal(wt_deck* self) {

    uint32_t frame_width = self->frames[0]->buf_sz;
    for (uint32_t i = 1; i < self->frames_sz; i++) {
        if (self->frames[i]->buf_sz != frame_width) {
            return false;
        }
    }
    return true;
}

/**
 * @brief fill a matrix from the underlying frames ptr. All frames must be equally
 * sized and the matrix should be initialized with the correct buffer
 */
dsp_err wt_deck_matrix_fill(wt_deck* self, matrix* out) {
    if (!wt_deck_frames_equal(self)) {
        return DSP_ERR;
    }
    size_t n_rows = self->frames_sz;
    size_t n_cols = self->frames[0]->buf_sz;

    if (n_rows != out->n_rows || n_cols != out->n_cols) {
        return DSP_ERR + 1;  // misaligned
    }

    for (size_t i = 0; i < self->frames_sz; i++) {
        matrix_set_row(out, i, self->frames[i]->buf, n_cols);
    }

    return DSP_OK;
}
