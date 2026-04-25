
#include <dsp/pblep.h>
#include <math.h>
#include "dsp/constants.h"
#include "dsp/conversions.h"
#include "dsp/shape.h"
#include "dsp/utils.h"

// NOTE: a bit of duplication from phasor but we are going to prefer this over
// a direct dependency with <dsp/phasor.h> for now to avoid complexity and
// potential problems in applications.

static inline void update_blepsaw_(blepsaw* self) {
    self->incr_ = (double) self->freq / (double) self->sr;
}

static inline float blepsaw_tick_(blepsaw* self) {
    self->phase_ += self->incr_;
    if (self->phase_ >= 1.0)
        self->phase_ -= 1.0;

    double out = unipolar_to_bipolar_d(self->phase_);  // 2.0 * self->phase_ - 1.0;
    out -= polyblep(self->phase_, self->incr_);
    return (float) out;  // (float) (1.0 - out);
}

static inline void update_blepsqr_(blepsqr* self) {
    self->incr_ = (double) self->freq / (double) self->sr;
    self->duty = clamp(self->duty, 0.0001f, 0.9999f);
}

static inline float blepsqr_tick_(blepsqr* self) {
    self->phase_ += self->incr_;
    if (self->phase_ >= 1.0)
        self->phase_ -= 1.0;

    double duty = (double) self->duty;
    double out = self->phase_ < duty ? 1.0 : -1.0;

    // blep: falling edge at phase 0.0;
    out += polyblep(self->phase_, self->incr_);

    // blep: rising edge at phase = duty
    double edge_phase = fmod((self->phase_ - duty + 1.0), 1.0);
    out -= polyblep(edge_phase, self->incr_);

    return (float) out;
}

void blepsaw_init(blepsaw* self, float freq, float iphs, float sr) {
    self->freq = freq;
    self->sr = sr;
    self->phase_ = (double) wrap_float_positive(iphs, 1.0);
    self->incr_ = 0.0;

    update_blepsaw_(self);
}

void blepsaw_tick_block(blepsaw* self,
                        float* out,
                        float* freq,
                        uint32_t start,
                        uint32_t nsmps) {

    for (uint32_t i = start; i < nsmps; i++) {
        float freq_ = freq[i];
        bool freq_eq = check_float_equal(freq_, self->freq);
        if (!freq_eq) {
            self->freq = freq_;
            update_blepsaw_(self);
        }
        out[i] = blepsaw_tick_(self);
    }
}

void blepsqr_init(blepsqr* self, float freq, float duty, float iphs, float sr) {
    self->freq = freq;
    self->sr = sr;
    self->duty = duty;
    self->phase_ = (double) wrap_float_positive(iphs, 1.0);

    update_blepsqr_(self);
}

void blepsqr_tick_block(blepsqr* self,
                        float* out,
                        float* freq,
                        float* duty,
                        uint32_t start,
                        uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        float freq_ = freq[i];
        float duty_ = duty[i];
        bool freq_eq = check_float_equal(freq_, self->freq);
        bool duty_eq = check_float_equal(duty_, self->duty);

        if (!freq_eq || !duty_eq) {
            self->duty = duty_;
            self->freq = freq_;
            update_blepsqr_(self);
        }

        out[i] = blepsqr_tick_(self);
    }
}
