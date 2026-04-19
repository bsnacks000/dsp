#include <dsp/interpolate.h>
#include <dsp/tabread.h>
#include <dsp/xfade.h>

// TODO: implement multiple methods .. zero clip (zclip) and wrap (wrp)
//      -- (zclip we have)
//      -- do this after we have a stable `table` struct/API to sue
//  Ex. wrap..
//       while (val < 0) val += buf_sz;
//       while (val >= buf_sz) val -= buf_sz;

void tabread_init(tabread* self, float* wt, uint32_t wt_sz) {
    self->wt = wt;
    self->wt_sz = wt_sz;
    self->wt_len_ = wt_sz - 2;  // assumes guardpoint set for interpolation
}

void tabreadn_tick_block(tabread* self,
                         float* out,
                         float* idx,
                         uint32_t start,
                         uint32_t nsmps) {
    float* buf = self->wt;
    uint32_t len = self->wt_len_;

    for (uint32_t i = start; i < nsmps; i++) {
        float val = idx[i];
        if (val < 0.0 || val >= (float) len) {
            out[i] = 0.0f;
            continue;
        }
        uint32_t ipos = (uint32_t) idx[i];
        out[i] = buf[ipos];
    }
}

static inline float tabreadi_tick_(tabread* self, float val) {

    float* buf = self->wt;
    uint32_t len = self->wt_len_;

    if (val < 0.0 || val >= (float) len) {
        return 0.0f;
    }
    uint32_t ipos = (uint32_t) val;
    float frac = val - ipos;

    float a = buf[ipos];
    float b = buf[ipos + 1];

    return interpolate_linear(a, b, frac);
}

static inline float tabread3_tick_(tabread* self, float val) {

    float* buf = self->wt;
    uint32_t len = self->wt_len_;

    if (val < 0.0 || val >= (float) len) {
        return 0.0f;
    }
    uint32_t ipos = (uint32_t) val;
    float frac = val - ipos;

    // NOTE: the select stmt is needed since we do not handle pow2 mask
    float a = ipos > 0 ? buf[ipos - 1] : buf[len - 1];
    float b = buf[ipos];
    float c = buf[ipos + 1];
    float d = buf[ipos + 2];

    return interpolate_cubic(a, b, c, d, frac);
}

void tabreadi_tick_block(tabread* self,
                         float* out,
                         float* idx,
                         uint32_t start,
                         uint32_t nsmps) {

    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = tabreadi_tick_(self, idx[i]);
    }
}

void tabread3_tick_block(tabread* self,
                         float* out,
                         float* idx,
                         uint32_t start,
                         uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = tabread3_tick_(self, idx[i]);
    }
}

// xtabread

static inline void xtabread_update_(xtabread* self) {

    // draw the correct bandpair from the deck using pos..
    frame_pair pair = matrix_row_pair_positional_lookup(self->deck, self->pos);

    // set up crossfade
    xfade_pair amps = xfade_from_pos(self->pos, self->deck->n_rows);

    // assign to state and normalize
    self->l_amp_ = amps.left;
    self->r_amp_ = amps.right;

    float norm = 1.0f / (self->l_amp_ + self->r_amp_);
    self->l_amp_ *= norm;
    self->r_amp_ *= norm;

    // set band pair
    self->l->wt = pair.low;
    self->r->wt = pair.high;
}

void xtabread_init(xtabread* self, matrix* deck, tabread* l, tabread* r, float pos) {
    self->deck = deck;
    self->l = l;
    self->r = r;
    self->pos = pos;

    // set in update_
    self->l->wt = NULL;
    self->r->wt = NULL;

    xtabread_update_(self);
}

void xtabreadi_tick_block(xtabread* self,
                          float* out,
                          float* idx,
                          float* pos,
                          uint32_t start,
                          uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        float pos_ = pos[i];
        bool pos_eq = check_float_equal(pos_, self->pos);
        if (!pos_eq) {
            self->pos = pos_;
            xtabread_update_(self);
        }
        float lout = tabreadi_tick_(self->l, idx[i]);
        float rout = tabreadi_tick_(self->r, idx[i]);
        out[i] = lout * self->l_amp_ + rout * self->r_amp_;
    }
}

void xtabread3_tick_block(xtabread* self,
                          float* out,
                          float* idx,
                          float* pos,
                          uint32_t start,
                          uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        float pos_ = pos[i];
        bool pos_eq = check_float_equal(pos_, self->pos);

        if (!pos_eq) {
            self->pos = pos_;
            xtabread_update_(self);
        }

        float lout = tabread3_tick_(self->l, idx[i]);
        float rout = tabread3_tick_(self->r, idx[i]);
        out[i] = lout * self->l_amp_ + rout * self->r_amp_;
    }
}
