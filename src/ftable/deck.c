#include <dsp/ftable/deck.h>
#include <dsp/ftable/ftable.h>
#include <dsp/ftable/sinesum.h>
#include <dsp/maths.h>
#include <dsp/rmap.h>

/**
 * @brief init a ft_deck. We check that frames is at least 2 so that we can iterate
 * and determine if all frames in the deck are equal.
 */
void ft_deck_init(ft_deck* self, ftable** frames, uint32_t frames_sz) {
    self->frames = frames;
    self->frames_sz = frames_sz;
}

/**
 * @brief check if all frames are of equal length. Usually this is the case when we're
 * using decks, but there could be impls that might want decks with frames of different
 * sizes.
 */
bool ft_deck_frames_equal(ft_deck* self) {

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
dsp_err ft_deck_matrix_fill(ft_deck* self, matrix* out) {
    if (!ft_deck_frames_equal(self)) {
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
