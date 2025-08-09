
#include <dsp/phasor.h>
#include <dsp/utils.h>

static inline void update_(phasor* self) {
    self->incr_ = self->freq / self->sr;
}

static inline float tick_(phasor* self) {
    self->phase_ += self->incr_;
    if (self->phase_ >= 1.0)
        self->phase_ -= 1.0;
    return (float) self->phase_;
}

void phasor_init(phasor* self, float freq, float sr) {
    self->freq = freq;
    self->sr = sr;
    self->phase_ = 0.0;
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
