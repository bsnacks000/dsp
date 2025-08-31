
#include <dsp/assert.h>
#include <dsp/maths.h>
#include <dsp/utils.h>
#include <dsp/wavetable/normalize.h>
#include <dsp/wavetable/sinesum.h>
#include <dsp/wavetable/wavetable.h>

/**
 * @brief The lanczos smoothing interpolation formula. Mainly used to round off
 * waveforms with high harmonics during waveform generation.
 *
 * @param i  index (current harmonic)
 * @param n  total harmonics  (can not be zero)
 */
static inline float lanczos_smoothing(uint32_t i, uint32_t n) {
    dsp_assert(n > 0, "n must be greater then zero");
    float x = (float) (i + 1) * DSP_PI / (float) n;
    return sin(x) / x;
}

dsp_err wt_sinesum_args_init(wt_sinesum_args* self,
                             const float* amps,
                             uint32_t amps_sz,
                             float phase,
                             bool smooth,
                             uint32_t nharms) {

    if (amps_sz < 1) {
        return DSP_ERR;
    }
    self->amps = amps;
    self->amps_sz = amps_sz;
    self->phase = clamp(phase, -1, 1);
    self->smooth = smooth;
    self->nharms = (uint32_t) clamp(nharms, 1, amps_sz);  // XXX: might need u32 clamp

    return DSP_OK;
}

dsp_err wt_sinesum(wavetable* wt, void* args) {
    wt_sinesum_args* args_ = (wt_sinesum_args*) args;

    float* buf = wt->buf;
    const float* amps = args_->amps;

    uint32_t len = wt->len, nharms = args_->nharms;

    bool smooth = args_->smooth;
    float phase = args_->phase;

    phase *= TWO_PI;
    float sigma = 1.0;
    for (uint32_t i = 0; i < nharms; i++) {
        if (smooth) {
            sigma = lanczos_smoothing(i, nharms);
        }
        for (uint32_t j = 0; j < len; j++) {
            float freq = (i + 1) * (j * TWO_PI / len);
            buf[j] += amps[i] * sin(freq + phase) * sigma;
        }
    }

    normalize(buf, len);
    return DSP_OK;
}

dsp_err sinesum_deck_generate(wavetable** wt,
                              wt_sinesum_args** args,
                              uint32_t n_bands,
                              float sr) {
    for (uint32_t i = 0; i < n_bands; i++) {
        dsp_err err;
        wavetable* w = wt[i];
        wt_sinesum_args* arg = args[i];

        if ((err = wt_sinesum(w, (void*) arg)) != DSP_OK) {
            return err;
        }
        // manually wrap guard points here ..
        wavetable_add_guard_point(w);
        // NOTE: 0.707 seems to be the sweet spot to avoid aliasing
        w->f0 = max_fundamental(arg->nharms, sr, 0.707);
        w->sr = sr;
        w->nharms = arg->nharms;
    }
    return DSP_OK;
}
