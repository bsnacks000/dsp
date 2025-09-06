#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include <dsp/lag.h>
#include <dsp/utils.h>

static inline void update_(lag* self) {
    self->alpha_ = 1.0 - exp((-1.0 / (self->t_sec * self->sr)));
}

static inline float tick_(lag* self, float xn) {
    self->last_ += self->alpha_ * (xn - self->last_);
    return self->last_;
}

void lag_init(lag* self, float t_sec, float sr) {
    self->sr = sr;
    self->t_sec = t_sec;
    self->last_ = 0.0;
    self->alpha_ = 0.0;
    update_(self);
}

void lag_tick_block(lag* self, float* out, float* in, float* t_sec, uint32_t nsmps) {

    for (uint32_t i = 0; i < nsmps; i++) {
        float t_sec_ = t_sec[i];
        bool t_sec_eq = check_float_equal(self->t_sec, t_sec_);

        if (!t_sec_eq) {
            self->t_sec = t_sec_;
            update_(self);
        }

        out[i] = tick_(self, in[i]);
    }
}
