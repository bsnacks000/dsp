#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include <dsp/assert.h>
#include <dsp/lag.h>
#include <dsp/utils.h>

static inline void update_(lag* self) {
    self->t_sec += 1e-9f;  // guard div/0
    self->alpha_ = 1.0 - exp((-1.0 / ((double) self->t_sec * (double) self->sr)));
}

static inline float tick_(lag* self, float xn) {
    double x = (double) xn;
    self->last_ += self->alpha_ * (x - self->last_);
    return (float) self->last_;
}

void lag_init(lag* self, float t_sec, float sr) {

    self->sr = assure_gt_zero(sr);
    self->t_sec = assure_gt_zero(t_sec);
    self->last_ = 0.0;
    self->alpha_ = 0.0;
    update_(self);
}

void lag_tick_block(lag* self,
                    float* out,
                    float* in,
                    float* t_sec,
                    uint32_t start,
                    uint32_t nsmps) {

    for (uint32_t i = start; i < nsmps; i++) {
        float t_sec_ = assure_gt_zero(t_sec[i]);
        bool t_sec_eq = check_float_equal(self->t_sec, t_sec_);

        if (!t_sec_eq) {
            self->t_sec = t_sec_;
            update_(self);
        }

        out[i] = tick_(self, in[i]);
    }
}
