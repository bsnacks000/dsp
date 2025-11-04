#include <math.h>
#include <stdint.h>

#include <dsp/interpolate.h>
#include <dsp/oscil.h>
#include <dsp/utils.h>
#include <dsp/wavetable/deck.h>

// NOTE: some extra guard like this would avoid potential drift for long running
// (days/weeks) if (self->index_ >= self->wt->len || self->index_ < 0.0)
//     self->index_ -= floor(self->index_ / self->wt->len) * self->wt->len;

// this is a cheap phase guard
static inline void phase_guard(float* phase) {
    if (DSP_UNLIKELY(*phase >= 1.0 || *phase < 0.0)) {
        *phase -= floor(*phase);
    }
}

static inline void oscil_update_(oscil* self) {
    phase_guard(&self->phase);
    self->incr_ = self->freq * self->wt->len / self->sr;
    self->offset_ = (uint32_t) (self->phase * self->wt->len) % self->wt->mask;
}

static inline float osciln_tick_(oscil* self) {
    uint32_t mask = self->wt->mask;
    double pos = self->index_ + (double) self->offset_;
    int32_t ipos = (int32_t) floor(pos);

    float out = self->wt->buf[ipos & mask];
    self->index_ += self->incr_;

    return out;
}

static inline float oscili_tick_(oscil* self) {
    uint32_t mask = self->wt->mask;
    double pos = self->index_ + (double) self->offset_;
    int32_t ipos = (int32_t) floor(pos);

    float frac = (float) (pos - ipos);

    float a = self->wt->buf[ipos & mask];
    float b = self->wt->buf[(ipos + 1) & mask];

    float out = interpolate_linear(a, b, frac);
    self->index_ += self->incr_;
    return out;
}

static inline float oscil3_tick_(oscil* self) {
    uint32_t mask = self->wt->mask;
    double pos = self->index_ + (double) self->offset_;
    int32_t ipos = (int32_t) floor(pos);

    float frac = (float) (pos - ipos);

    float a = self->wt->buf[(ipos - 1) & mask];
    float b = self->wt->buf[ipos & mask];
    float c = self->wt->buf[(ipos + 1) & mask];
    float d = self->wt->buf[(ipos + 2) & mask];

    float out = interpolate_cubic(a, b, c, d, frac);
    self->index_ += self->incr_;
    return out;
}

/// NOTE: These were moved from deck public API.
// The reasoning is that other modules may implement different private lookup strategies
// for decks if needed. These specific strats are more bound to xoscil.

// /**
//  * @brief given a position and deck_sz calculate a cross fade value over a given
//  * frame/band pair.
//  */
// static inline xfade_pair xfade_from_pos(float pos, uint32_t deck_sz) {
//     pos = clamp(pos, 0.0, 1.0);

//     float scaled = pos * (deck_sz - 1);
//     float fader = scaled - floorf(scaled);
//     // fprintf(stderr, "%.8f\n", fader);
//     return xfade_sin(fader);
// }

/**
 * @brief query the l/r amplitudes for use with xfade given freq for bandlimited
 * crossfades.
 */
static inline xfade_pair xfade_from_freq(float freq, float lo, float hi) {
    if (freq < lo) {
        return (xfade_pair) {.left = 1.0, .right = 0.0};
    }

    if (freq > hi) {
        return (xfade_pair) {.left = 0.0, .right = 1.0};
    }

    // TODO: precompute mapping (avoid linlin)
    float fader = linlin(freq, lo, hi, 0.0, 1.0);
    return xfade_sin(fader);
}

/**
 * @brief represents a low/high pair. The tables used for the actual crossfade.
 */
// typedef struct {
//     wavetable* low;
//     wavetable* high;
// } wt_frame_pair;

/**
 * @brief wt_deck lookup using the position to fill a band_pair.
 */
// static inline wt_frame_pair wt_deck_pos_lookup(wt_deck* self, float pos) {
//     pos = clamp(pos, 0.0f, 1.0f);  // ensure [0, 1]
//     pos = 0.5f - 0.5f * cosf(pos * M_PI);

//     float fidx = pos * (self->frames_sz - 1);  // range: [0, N-1]
//     uint32_t idx = (int) fidx;

//     if (idx >= self->frames_sz - 1) {
//         idx = self->frames_sz - 2;
//         fidx = (float) idx;
//     }

//     return (wt_frame_pair) {
//         .low = self->frames[idx],
//         .high = self->frames[idx + 1],
//     };
// }

/**
 * @brief query the deck and return the correct band_pair given the freq. To provide
 * smooth frequency transition regions its on the caller to supply a deck in the
 * application layer that contains:
 *  - wavetables sorted by max fundamental (wt->f0)
 *  - complete freq coverage up to nyquist across frames.
 *
 * NOTE: If there are gaps or unsorted regions this lookup will behave inconsistently.
 */
static inline wt_frame_pair wt_deck_freq_lookup(wt_deck* self, float freq) {
    // TODO maybe clamp freq.

    // lower then first band
    if (freq < self->frames[0]->f0) {
        return (wt_frame_pair) {
            .low = self->frames[0],
            .high = self->frames[1],
        };
    }

    // higher then last band
    if (freq > self->frames[self->frames_sz - 1]->f0) {
        return (wt_frame_pair) {
            .low = self->frames[self->frames_sz - 2],
            .high = self->frames[self->frames_sz - 1],
        };
    }

    // we *should* always fall within some frequency region..
    // its on caller to make sure the deck
    for (uint32_t i = 1; i < self->frames_sz; i++) {
        if (self->frames[i - 1]->f0 <= freq && freq < self->frames[i]->f0) {
            return (wt_frame_pair) {
                .low = self->frames[i - 1],
                .high = self->frames[i],
            };
        }
    }

    // if we get here there are gaps in the freq bands so we just return
    // the first pair
    // TODO: warning (debug)
    return (wt_frame_pair) {
        .low = self->frames[0],
        .high = self->frames[1],
    };
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
    wt_frame_pair bpair = wt_deck_freq_lookup(self->deck, self->freq);
    float lo_freq = bpair.low->f0;
    float hi_freq = bpair.high->f0;

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

    if (self->l->wt != bpair.low) {
        self->l->wt = bpair.low;
    }
    if (self->r->wt != bpair.high) {
        self->r->wt = bpair.high;
    }
}

// generic over xoscil based on lookup callback.
static inline void xoscil_update_(xoscil* self) {
    // fprintf(stderr, "%8.f\n", self->pos);
    //  update the sub oscils

    self->l->freq = self->freq;
    self->l->phase = self->phase;

    oscil_update_(self->l);

    self->r->freq = self->freq;
    self->r->phase = self->phase;
    oscil_update_(self->r);

    // draw the correct bandpair from the deck using pos..
    wt_frame_pair bpair = wt_deck_pos_lookup(self->deck, self->pos);

    // use the freqs from the band pair to calculate the xfade value
    xfade_pair amps = xfade_from_pos(self->pos, self->deck->frames_sz);

    // assign to state and normalize
    self->l_amp_ = amps.left;
    self->r_amp_ = amps.right;

    float norm = 1.0f / (self->l_amp_ + self->r_amp_);
    self->l_amp_ *= norm;
    self->r_amp_ *= norm;

    // fprintf(stdout, "%.3f\n", self->l_amp_);

    if (self->l->wt != bpair.low) {
        self->l->wt = bpair.low;
    }
    if (self->r->wt != bpair.high) {
        self->r->wt = bpair.high;
    }
}

// public oscil API
//

dsp_err oscil_init(oscil* self, wavetable* wt, float freq, float phase, float sr) {
    if (!wt->is_pow2) {
        return DSP_ERR;
    }
    self->wt = wt;
    self->freq = freq;
    self->phase = phase;
    self->sr = sr;

    self->incr_ = 0.0;
    self->offset_ = 0;
    self->index_ = 0.0;

    oscil_update_(self);
    return DSP_OK;
}

void osciln_tick_block(oscil* self, float* out, float* freq, uint32_t nsmps) {
    for (uint32_t i = 0; i < nsmps; i++) {
        float freq_ = freq[i];
        bool freq_eq = check_float_equal(freq_, self->freq);
        if (!freq_eq) {
            self->freq = freq_;
            oscil_update_(self);
        }
        out[i] = osciln_tick_(self);
    }
}

void oscili_tick_block(oscil* self, float* out, float* freq, uint32_t nsmps) {
    for (uint32_t i = 0; i < nsmps; i++) {
        float freq_ = freq[i];
        bool freq_eq = check_float_equal(freq_, self->freq);
        if (!freq_eq) {
            self->freq = freq_;
            oscil_update_(self);
        }
        out[i] = oscili_tick_(self);
    }
}

void oscili_pm_tick_block(oscil* self,
                          float* out,
                          float* freq,
                          float* phs,
                          uint32_t nsmps) {
    for (uint32_t i = 0; i < nsmps; i++) {
        float freq_ = freq[i];
        bool freq_eq = check_float_equal(freq_, self->freq);

        float phs_ = phs[i];
        bool phs_eq = check_float_equal(phs_, self->phase);

        if (!freq_eq || !phs_eq) {
            self->freq = freq_;
            self->phase = phs_;
            oscil_update_(self);
        }
        out[i] = oscili_tick_(self);
    }
}

void oscil3_tick_block(oscil* self, float* out, float* freq, uint32_t nsmps) {
    for (uint32_t i = 0; i < nsmps; i++) {
        float freq_ = freq[i];
        bool freq_eq = check_float_equal(freq_, self->freq);

        if (!freq_eq) {
            self->freq = freq_;
            oscil_update_(self);
        }
        out[i] = oscil3_tick_(self);
    }
}

void oscil3_pm_tick_block(oscil* self,
                          float* out,
                          float* freq,
                          float* phs,
                          uint32_t nsmps) {
    for (uint32_t i = 0; i < nsmps; i++) {
        float freq_ = freq[i];
        bool freq_eq = check_float_equal(freq_, self->freq);

        float phs_ = phs[i];
        bool phs_eq = check_float_equal(phs_, self->phase);

        if (!freq_eq || !phs_eq) {
            self->freq = freq_;
            self->phase = phs_;
            oscil_update_(self);
        }
        out[i] = oscil3_tick_(self);
    }
}

dsp_err blxoscil_init(blxoscil* self,
                      wt_deck* deck,
                      oscil* l,
                      oscil* r,
                      float freq,
                      float phase) {
    self->deck = deck;

    // prevent caller from passing in two srs ... the sub oscils must coordinate.
    if (l->sr != r->sr) {
        return DSP_ERR;
    }
    self->sr = l->sr;

    // TODO: double check this is 0-1 ..
    phase = clamp(phase, 0.0, 1.0);

    self->l = l;
    self->r = r;
    self->freq = freq;
    self->phase = phase;

    blxoscil_update_(self);
    return DSP_OK;
}

void blxoscili_tick_block(blxoscil* self, float* out, float* freq, uint32_t nsmps) {
    for (uint32_t i = 0; i < nsmps; i++) {

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

void blxoscil3_tick_block(blxoscil* self, float* out, float* freq, uint32_t nsmps) {

    for (uint32_t i = 0; i < nsmps; i++) {

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

dsp_err xoscil_init(xoscil* self,
                    wt_deck* deck,
                    oscil* l,
                    oscil* r,
                    float freq,
                    float pos,
                    float phase) {

    // prevent caller from passing in two srs ... the sub oscils must coordinate.
    if (l->sr != r->sr) {
        return DSP_ERR;
    }
    self->sr = l->sr;

    // TODO: double check these are 0-1 ..
    phase = clamp(phase, 0.0, 1.0);
    pos = clamp(pos, 0.0, 1.0);

    self->deck = deck;
    self->l = l;
    self->r = r;
    self->freq = freq;
    self->phase = phase;
    self->pos = pos;

    xoscil_update_(self);
    return DSP_OK;
}

void xoscili_tick_block(xoscil* self,
                        float* out,
                        float* freq,
                        float* pos,
                        uint32_t nsmps) {

    for (uint32_t i = 0; i < nsmps; i++) {
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
                        uint32_t nsmps) {

    for (uint32_t i = 0; i < nsmps; i++) {
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
