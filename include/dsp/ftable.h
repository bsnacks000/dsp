/**
 * @brief ftable utilities
 */
#ifndef DSP_FTABLE_H
#define DSP_FTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/chebpoly.h>
#include <dsp/fasttrig.h>
#include <dsp/maths.h>
#include <dsp/shape.h>
#include <dsp/sinesum.h>
#include <dsp/utils.h>

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief A general purpose ftable. The buf_sz **should always include +2 slots** (8
 * bytes) for 2 guard points to support linear / cubic interpolation. buf_sz -2 is
 * stored in len and should be used in any linear / cubic to handle guardpoint.
 *
 * Guardpoint can be explicitly set with ftable_add_guardpoint
 *
 * If len is pow2 then a bit mask is provided for algos that want to use
 * fast wrap around.
 *
 * f0 and sr are initialized to 0 and may be filled later by other utilities to help
 * provide neccessary metadata for band limiting or playback.
 */
typedef struct {
    float* buf;
    uint32_t buf_sz, len, mask;
    bool is_pow2;
    float f0, sr, nharms;  // set by utilities
} ftable;

/**
 * @brief initialize a ftable. buf size should be desired size + 2. Eg. 1024 -> 1026.
 * The len and mask are calculated from buf_sz. The `is_pow2` flag is also set.
 */
static inline void ftable_init(ftable* self, float* buf, uint32_t buf_sz) {
    self->buf = buf;
    self->buf_sz = buf_sz;

    self->len = buf_sz - 2;
    self->mask = self->len - 1;
    self->is_pow2 = is_pow2(self->len);

    // safety first..
    self->nharms = 0.0;
    self->f0 = 0.0;
    self->sr = 0.0;
}

/**
 * @brief Zero an ftable buffer.
 */
static inline void ftable_zero_buf(ftable* self) {
    zero_buf(self->buf, self->buf_sz);
}

/**
 * @brief an interface that fills a ftable buffer.
 */
typedef dsp_err (*ft_f)(ftable* wt, void* args);

/**
 * @brief write nsmps at offset directly into the ftable buffer
 * from an external buffer. Does not respect guard point (last 2 indices of buf)
 */
static inline void ftable_write(ftable* self,
                                float* in,
                                uint32_t nsmps,
                                uint32_t offset) {
    for (uint32_t i = offset; i < nsmps; i++) {
        self->buf[i] = in[i];
    }
}
/**
 * @brief add the guard point wrap around manually. This is automatically called if
 * using the ftable_func API.
 */
static inline void ftable_add_guard_point(ftable* self) {
    self->buf[self->len] = self->buf[0];
    self->buf[self->len + 1] = self->buf[1];
}
/**
 * @brief runs the ft_f over the entire ftable, potentially filling or updating the
 * ftable buffer depending on the function. Propagates dsp_err from ft_f. Will
 * automatically add the guard points as a convenience.
 */
static inline dsp_err ftable_func(ftable* self, ft_f f, void* args) {
    ftable_zero_buf(self);

    dsp_err err;
    if ((err = f(self, args)) != DSP_OK) {
        return err;
    }
    return DSP_OK;
}
/**
 * @brief Safely copy other into self. Runtime check for equal buffer size.
 */
static inline dsp_err ftable_shallow_copy(ftable* self, ftable* other) {
    if (self->buf_sz != other->buf_sz)
        return DSP_ERR;
    ftable_init(self, other->buf, other->buf_sz);
    return DSP_OK;
}

/**
 * @brief Deep copy other into self. Runtime check for equal buffer size.
 */
static inline dsp_err ftable_deep_copy(ftable* self, ftable* other) {
    if (self->buf_sz != other->buf_sz)
        return DSP_ERR;
    copy_nsmps(self->buf, other->buf, 0, self->buf_sz);
    return DSP_OK;
}

// TODO: add ftable implementations

/**
 * @brief args for ft_sinesum.
 */
typedef struct {
    float* amps;       // a list of harmonic amplitudes.
    uint32_t amps_sz;  // sz of amplitude buffer > 1
    float phase;       // btwn -1 and 1; normalized internally to TWO_PI
    bool smooth;       // w/ smoothing
    uint32_t nharms;   // should be 1 -> amps_sz; if 0 skipped and amps_sz is used.
} ft_sinesum_args;

static inline dsp_err ft_sinesum_args_init(ft_sinesum_args* self,
                                           float* amps,
                                           uint32_t amps_sz,
                                           float phase,
                                           bool smooth,
                                           uint32_t nharms) {

    if (amps_sz < 1) {
        return DSP_ERR;
    }
    self->amps = amps;
    self->amps_sz = amps_sz;
    self->phase = clamp(phase, -1.0f, 1.0f);
    self->smooth = smooth;
    self->nharms =
        (uint32_t) clamp(nharms, 1.0f, amps_sz);  // XXX: might need u32 clamp

    return DSP_OK;
}

static inline dsp_err ft_sinesum(ftable* self, void* args) {
    ft_sinesum_args* args_ = (ft_sinesum_args*) args;

    float* buf = self->buf;
    float* amps = args_->amps;

    // use wt->len as the end
    uint32_t len = self->len, nharms = args_->nharms;

    bool smooth = args_->smooth;
    float phase = args_->phase;

    sinesum(buf, len, amps, nharms, phase, smooth);

    // wrap for oscillator use
    ftable_add_guard_point(self);
    return DSP_OK;
}

static inline dsp_err ft_sinesum1(ftable* self, void* args) {

    ft_sinesum_args* args_ = (ft_sinesum_args*) args;

    float* buf = self->buf;
    float* amps = args_->amps;

    // here we use wt->buf_sz as the end of the interpolation
    uint32_t len = self->buf_sz, nharms = args_->nharms;

    bool smooth = args_->smooth;
    float phase = args_->phase;

    sinesum(buf, len, amps, nharms, phase, smooth);

    // no wrap. This is for a single scan.
    normalize(buf, len);
    return DSP_OK;
}

// chebpoly
/**
 * @brief state for ft_cheby_args
 */
typedef struct {
    float* h;
    uint32_t h_sz;
} ft_chebpoly_args;

// TODO: check : sz >= 2; sum of coeffs != 0.0
static inline dsp_err ft_chebpoly_args_init(ft_chebpoly_args* self,
                                            float* h,
                                            uint32_t h_sz) {
    self->h = h;
    self->h_sz = h_sz;
    return DSP_OK;
}

static inline dsp_err ft_chebpoly(ftable* self, void* args) {
    ft_chebpoly_args* args_ = (ft_chebpoly_args*) args;

    float* h = args_->h;
    uint32_t h_sz = args_->h_sz;

    if (h_sz < 3) {
        return DSP_ERR;  // we need at least T0,T1,T2 to values to produce a waveform.
    }

    float* buf = self->buf;
    uint32_t buf_sz =
        self->buf_sz;  // for chebyshev we always write to the end of the buffer.

    chebyshev_fill(buf, buf_sz, h, h_sz);

    return DSP_OK;
}
/**
 * @brief
 * ftable deck -- an array of ftable
 */

/**
 * @brief ft_deck is fat pointer type that simply holds an array of ftable pointers.
 * Its lookup strategy is based on position. We borrow terminology from serum and other
 * wt synths and call these "frames".
 */
typedef struct {
    ftable** frames;
    uint32_t frames_sz;  // nrows
} ft_deck;

/**
 * @brief init a ft_deck. We check that frames is at least 2 so that we can iterate
 * and determine if all frames in the deck are equal.
 */
static inline void ft_deck_init(ft_deck* self, ftable** frames, uint32_t frames_sz) {
    self->frames = frames;
    self->frames_sz = frames_sz;
}
/**
 * @brief check if all frames are of equal length. Usually this is the case when we're
 * using decks, but there could be impls that might want decks with frames of different
 * sizes.
 */
static inline bool ft_deck_frames_equal(ft_deck* self) {

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

static inline dsp_err ft_deck_matrix_fill(ft_deck* self, matrix* out) {
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

/**
 * @brief represents a low/high pair. The tables used for the actual crossfade.
 */
typedef struct {
    ftable* low;
    ftable* high;
} ft_frame_pair;

/**
 * @brief return a frame_pair from a deck given pos.
 */
static inline ft_frame_pair ft_deck_pos_lookup(ft_deck* self, float pos) {
    pos = clamp(pos, 0.0f, 1.0f);         // ensure [0, 1]
    pos = 0.5f - 0.5f * fast_hcosf(pos);  // smooth transition w/ half cos

    float fidx = pos * (self->frames_sz - 1);  // range: [0, N-1]
    uint32_t idx = (int) fidx;

    if (idx >= self->frames_sz - 1) {
        idx = self->frames_sz - 2;
        fidx = (float) idx;
    }

    return (ft_frame_pair) {
        .low = self->frames[idx],
        .high = self->frames[idx + 1],
    };
}

static inline dsp_err ft_deck_sinesum_generate(ft_deck* self,
                                               ft_sinesum_args** args,
                                               uint32_t n_bands,
                                               float sr) {
    for (uint32_t i = 0; i < n_bands; i++) {
        dsp_err err;
        ftable* w = self->frames[i];
        ft_sinesum_args* arg = args[i];

        if ((err = ft_sinesum(w, (void*) arg)) != DSP_OK) {
            return err;
        }

        // TODO: move to band info ...
        // NOTE: 0.707 seems to be the sweet spot to avoid aliasing
        w->f0 = max_fundamental(arg->nharms, sr, 0.707);
        w->sr = sr;
        w->nharms = arg->nharms;
    }
    return DSP_OK;
}

static inline dsp_err ft_deck_chebpoly_generate(ft_deck* self,
                                                ft_chebpoly_args** args,
                                                uint32_t n_bands) {

    for (uint32_t i = 0; i < n_bands; i++) {
        dsp_err err;
        ftable* w = self->frames[i];
        ft_chebpoly_args* arg = args[i];
        if ((err = ft_chebpoly(w, arg)) != DSP_OK) {
            return err;
        }
    }

    return DSP_OK;
}

#ifdef __cplusplus
}
#endif

#endif
