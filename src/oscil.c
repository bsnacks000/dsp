#include <dsp/oscil.h>

#include <dsp/assert.h>
#include <dsp/interpolate.h>
#include <dsp/matrix.h>
#include <dsp/shape.h>
#include <dsp/utils.h>
#include <dsp/xfade.h>

static inline void oscil_update_(oscil* self) {
    self->incr_ = (double) self->freq * self->wt_len_ / (double) self->sr;
}

static inline float osciln_tick_(oscil* self) {
    uint32_t mask = self->mask_;
    double pos = self->index_;
    int32_t ipos = (int32_t) floor(pos);

    float out = self->wt[(uint32_t) ipos & mask];
    self->index_ += self->incr_;

    // phase wrap
    if (self->index_ >= self->wt_len_)
        self->index_ -= self->wt_len_;

    return out;
}

static inline float oscili_tick_(oscil* self) {
    uint32_t mask = self->mask_;
    double pos = self->index_;
    int32_t ipos = (int32_t) floor(pos);

    float frac = (float) (pos - ipos);

    uint32_t idx = (uint32_t) ipos & mask;

    float a = self->wt[idx];
    float b = self->wt[(idx + 1) & mask];

    float out = interpolate_linear(a, b, frac);
    self->index_ += self->incr_;

    return out;
}

static inline float oscili_pm_tick_(oscil* self, float pmod) {
    uint32_t mask = self->mask_;
    // phs mod calc
    double pos = self->index_ + (double) (pmod * (float) self->wt_len_);
    int32_t ipos = (int32_t) floor(pos);

    float frac = (float) (pos - ipos);

    uint32_t idx = (uint32_t) ipos & mask;

    float a = self->wt[idx];
    float b = self->wt[(idx + 1) & mask];

    float out = interpolate_linear(a, b, frac);
    self->index_ += self->incr_;

    return out;
}

static inline float oscil3_tick_(oscil* self) {
    uint32_t mask = self->mask_;
    double pos = self->index_;
    int32_t ipos = (int32_t) floor(pos);

    float frac = (float) (pos - ipos);

    uint32_t idx = (uint32_t) ipos & mask;

    float a = self->wt[(idx - 1) & mask];
    float b = self->wt[idx];
    float c = self->wt[(idx + 1) & mask];
    float d = self->wt[(idx + 2) & mask];

    float out = interpolate_cubic(a, b, c, d, frac);
    self->index_ += self->incr_;

    return out;
}

static inline float oscil3_pm_tick_(oscil* self, float pmod) {

    uint32_t mask = self->mask_;
    double pos = self->index_ + (double) (pmod * (float) self->wt_len_);
    int32_t ipos = (int32_t) floor(pos);

    float frac = (float) (pos - ipos);

    uint32_t idx = (uint32_t) ipos & mask;

    float a = self->wt[(idx - 1) & mask];
    float b = self->wt[idx];
    float c = self->wt[(idx + 1) & mask];
    float d = self->wt[(idx + 2) & mask];

    float out = interpolate_cubic(a, b, c, d, frac);
    self->index_ += self->incr_;

    return out;
}

void oscil_init(oscil* self,
                float* wt,
                uint32_t wt_sz,
                float freq,
                float phase,
                float sr) {
    dsp_assert(is_pow2(wt_sz - 2), "wt_sz must be pow2 + 2.");

    self->wt = wt;
    self->freq = freq;
    self->phase = clamp(phase, 0.0, 1.0);
    self->sr = assure_gt_zero(sr);

    self->wt_sz = wt_sz;              // pow2 + 2 (guardpoint)
    self->wt_len_ = wt_sz - 2;        // pow2
    self->mask_ = self->wt_len_ - 1;  // pow2 - 1

    self->incr_ = 0.0;
    self->index_ = (double) self->phase * (double) self->wt_len_;

    oscil_update_(self);
}

void osciln_tick_block(oscil* self,
                       float* out,
                       float* freq,
                       uint32_t start,
                       uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        float freq_ = freq[i];
        bool freq_eq = check_float_equal(freq_, self->freq);
        if (!freq_eq) {
            self->freq = freq_;
            oscil_update_(self);
        }
        out[i] = osciln_tick_(self);
    }
    // accmodate phase drift
    self->index_ -= self->wt_len_ * floor(self->index_ / self->wt_len_);
}

void oscili_tick_block(oscil* self,
                       float* out,
                       float* freq,
                       uint32_t start,
                       uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        float freq_ = freq[i];
        bool freq_eq = check_float_equal(freq_, self->freq);
        if (!freq_eq) {
            self->freq = freq_;
            oscil_update_(self);
        }
        out[i] = oscili_tick_(self);
    }
    self->index_ -= self->wt_len_ * floor(self->index_ / self->wt_len_);
}

void oscili_pm_tick_block(oscil* self,
                          float* out,
                          float* freq,
                          float* mod,
                          uint32_t start,
                          uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {

        float freq_ = freq[i];
        bool freq_eq = check_float_equal(freq_, self->freq);
        if (!freq_eq) {
            self->freq = freq_;
            oscil_update_(self);
        }
        out[i] = oscili_pm_tick_(self, mod[i]);
    }
    self->index_ -= self->wt_len_ * floor(self->index_ / self->wt_len_);
}

void oscil3_tick_block(oscil* self,
                       float* out,
                       float* freq,
                       uint32_t start,
                       uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        float freq_ = freq[i];
        bool freq_eq = check_float_equal(freq_, self->freq);

        if (!freq_eq) {
            self->freq = freq_;
            oscil_update_(self);
        }
        out[i] = oscil3_tick_(self);
    }
    self->index_ -= self->wt_len_ * floor(self->index_ / self->wt_len_);
}

void oscil3_pm_tick_block(oscil* self,
                          float* out,
                          float* freq,
                          float* mod,
                          uint32_t start,
                          uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {

        float freq_ = freq[i];
        bool freq_eq = check_float_equal(freq_, self->freq);
        if (!freq_eq) {
            self->freq = freq_;
            oscil_update_(self);
        }
        out[i] = oscil3_pm_tick_(self, mod[i]);
    }
    self->index_ -= self->wt_len_ * floor(self->index_ / self->wt_len_);
}

// xoscil
//

static inline void xoscil_update_(xoscil* self) {
    // update sub oscils
    self->l->freq = self->freq;
    self->l->phase = self->phase;
    oscil_update_(self->l);

    self->r->freq = self->freq;
    self->r->phase = self->phase;
    oscil_update_(self->r);

    frame_pair bpair = matrix_row_pair_positional_lookup(self->deck, self->pos);

    xfade_pair amps = xfade_from_pos(self->pos, (uint32_t) self->deck->n_rows);

    self->l_amp_ = amps.left;
    self->r_amp_ = amps.right;

    float norm = 1.0f / (self->l_amp_ + self->r_amp_);
    self->l_amp_ *= norm;
    self->r_amp_ *= norm;

    self->l->wt = bpair.low;
    self->r->wt = bpair.high;
}

void xoscil_init(xoscil* self,
                 matrix* deck,
                 oscil* l,
                 oscil* r,
                 float freq,
                 float pos,
                 float phase,
                 float sr) {

    dsp_assert(deck->n_cols >= 2, "xoscil_init: deck must have min 2 cols.");
    oscil_init(l, matrix_get_row(deck, 0), deck->n_cols, freq, phase, sr);
    oscil_init(r, matrix_get_row(deck, 1), deck->n_cols, freq, phase, sr);

    phase = clamp(phase, 0.0f, 1.0f);
    pos = clamp(pos, 0.0f, 1.0f);

    self->deck = deck;
    self->l = l;
    self->r = r;
    self->freq = freq;
    self->phase = phase;
    self->pos = pos;
    self->sr = assure_gt_zero(sr);

    xoscil_update_(self);
}

void xoscili_tick_block(xoscil* self,
                        float* out,
                        float* freq,
                        float* pos,
                        uint32_t start,
                        uint32_t nsmps) {

    for (uint32_t i = start; i < nsmps; i++) {
        float freq_ = freq[i];
        float pos_ = pos[i];

        bool freq_eq = check_float_equal(freq_, self->freq);
        bool pos_eq = check_float_equal(pos_, self->pos);

        if (!freq_eq || !pos_eq) {
            self->freq = freq_;
            self->pos = pos_;

            xoscil_update_(self);
        }
        // tick both oscils and mix output.
        float lout = oscili_tick_(self->l);
        float rout = oscili_tick_(self->r);
        out[i] = lout * self->l_amp_ + rout * self->r_amp_;
    }
}

void xoscil3_tick_block(xoscil* self,
                        float* out,
                        float* freq,
                        float* pos,
                        uint32_t start,
                        uint32_t nsmps) {

    for (uint32_t i = start; i < nsmps; i++) {
        float freq_ = freq[i];
        float pos_ = pos[i];

        bool freq_eq = check_float_equal(freq_, self->freq);
        bool pos_eq = check_float_equal(pos_, self->pos);

        if (!freq_eq || !pos_eq) {
            self->freq = freq_;
            self->pos = pos_;
            xoscil_update_(self);
        }

        // tick both oscils and mix output.
        float lout = oscil3_tick_(self->l);
        float rout = oscil3_tick_(self->r);

        out[i] = lout * self->l_amp_ + rout * self->r_amp_;
    }
}

// generic over xoscil based on lookup callback.
static inline void blxoscil_update_(blxoscil* self) {
    // update the sub oscils
    // printf("self->l->freq: %f\n", self->l->freq);
    self->l->freq = self->freq;
    self->l->phase = self->phase;
    oscil_update_(self->l);

    self->r->freq = self->freq;
    self->r->phase = self->phase;
    oscil_update_(self->r);

    // draw the correct bandpair from the deck using freq..
    band_pair bpair = matrix_row_pair_freq_lookup(self->deck, self->freq, self->f0);
    float lo_freq = bpair.f0_low;
    float hi_freq = bpair.f0_high;

    // use the freqs from the band pair to calculate the xfade value
    xfade_pair amps = xfade_from_freq(self->freq, lo_freq, hi_freq);

    // assign to state and normalize
    self->l_amp_ = amps.left;
    self->r_amp_ = amps.right;

    float norm = 1.0f / (self->l_amp_ + self->r_amp_);
    self->l_amp_ *= norm;
    self->r_amp_ *= norm;

    // fprintf(stdout, "%.3f\n", self->l_amp_);
    // fprintf(stderr, "%.3f\n", self->r_amp_);

    self->l->wt = bpair.low;
    self->r->wt = bpair.high;
}

void blxoscil_init(blxoscil* self,
                   matrix* deck,
                   oscil* l,
                   oscil* r,
                   float* f0,
                   float freq,
                   float phase,
                   float sr) {

    dsp_assert(deck->n_cols >= 2, "xoscil_init: deck must have min 2 cols.");
    oscil_init(l, matrix_get_row(deck, 0), deck->n_cols, freq, phase, sr);
    oscil_init(r, matrix_get_row(deck, 1), deck->n_cols, freq, phase, sr);

    self->f0 = f0;

    self->deck = deck;
    self->sr = l->sr;

    phase = clamp(phase, 0.0, 1.0);

    self->l = l;
    self->r = r;
    self->freq = freq;
    self->phase = phase;
    self->sr = assure_gt_zero(sr);

    blxoscil_update_(self);
}

void blxoscili_tick_block(blxoscil* self,
                          float* out,
                          float* freq,
                          uint32_t start,
                          uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {

        float freq_ = freq[i];
        bool freq_eq = check_float_equal(freq_, self->freq);
        if (!freq_eq) {
            self->freq = freq_;
            blxoscil_update_(self);
        }

        // tick both oscils and mix output.
        float lout = oscili_tick_(self->l);
        float rout = oscili_tick_(self->r);
        out[i] = lout * self->l_amp_ + rout * self->r_amp_;
    }
}

void blxoscil3_tick_block(blxoscil* self,
                          float* out,
                          float* freq,
                          uint32_t start,
                          uint32_t nsmps) {

    for (uint32_t i = start; i < nsmps; i++) {

        float freq_ = freq[i];
        bool freq_eq = check_float_equal(freq_, self->freq);

        if (!freq_eq) {
            self->freq = freq_;
            blxoscil_update_(self);
        }

        // tick both oscils and mix output.
        float lout = oscil3_tick_(self->l);
        float rout = oscil3_tick_(self->r);
        out[i] = lout * self->l_amp_ + rout * self->r_amp_;
    }
}
