
#include <dsp/maths.h>
#include <dsp/phasor.h>
#include <dsp/utils.h>

static inline void update_(phasor* self) {
    self->incr_ = self->freq / self->sr;
}

static inline float tick_(phasor* self) {
    float out = (float) self->phase_;
    self->phase_ += self->incr_;
    if (self->phase_ >= 1.0)
        self->phase_ -= 1.0;
    return out;
}

void phasor_init(phasor* self, float freq, float iphs, float sr) {
    self->freq = freq;
    self->sr = sr;
    self->phase_ = (double) wrap_float_positive(iphs, 1.0);
    self->incr_ = 0.0;

    update_(self);
}

void phasor_tick_block(phasor* self, float* out, float* freq, uint32_t nsmps) {
    for (uint32_t i = 0; i < nsmps; i++) {
        float freq_ = freq[i];
        bool freq_eq = check_float_equal(freq_, self->freq);
        if (!freq_eq) {
            self->freq = freq_;
            update_(self);
        }
        out[i] = tick_(self);
    }
}

void impulse_tick_block(phasor* self, float* out, float* freq, uint32_t nsmps) {
    for (uint32_t i = 0; i < nsmps; i++) {
        float freq_ = freq[i];
        bool freq_eq = check_float_equal(freq_, self->freq);
        if (!freq_eq) {
            self->freq = freq_;
            update_(self);
        }
        // invert phasor ramp to start impulse on 1st sample
        float x = invert_unipolar(tick_(self));
        // printf("%.3f\n", x);
        out[i] = (x >= 1.0) ? 1.0 : 0.0;
    }
}

void rand_impulse_tick_block(phasor* self, float* out, float* freq, uint32_t nsmps) {
    for (uint32_t i = 0; i < nsmps; i++) {
        float freq_ = freq[i];
        bool freq_eq = check_float_equal(freq_, self->freq);
        if (!freq_eq) {
            self->freq = freq_;
            update_(self);
        }

        self->phase_ += self->incr_;

        if (self->phase_ >= 1.0) {
            // draw a new phase based on a quasi exponential dist.
            double wait = -log(rand_unipolar()) * (self->sr / self->freq);
            double next = 1.0 - (wait * self->incr_);
            self->phase_ = (next < 0.0) ? 0.0 : next;
            out[i] = 1.0;
        } else {
            out[i] = 0.0;
        }
    }
}
