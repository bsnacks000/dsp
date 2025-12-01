#include <dsp/balance.h>
#include <dsp/constants.h>
#include <dsp/shape.h>
#include <math.h>

// NOTE: we are hardcoding 10.0 as the frequency for this module.
inline static void update_(balance* self) {
    // bilinear transform
    double costh = 2.0 - cos(2.0 * (double) DSP_PI * 10.0 / (double) self->sr);
    self->coef_ = (float) (sqrt(costh * costh - 1.0) - costh);
}

// leaky integrator .. tick both cmp and out and create the scale per tick
// this uses lazzarini's maths based on: y[n]=(1−alpha)*x[n] + alpha * y[n−1]
// NOTE: this is an RMS estimation (Mean Absolute Value)
// To convert to true RMS:
//   - square out and cmp  (out * out) (cmp * cmp)
//   - take sqrt of d0 and d1 before zero div check and clamp
inline static void tick_(balance* self, float out, float cmp) {
    float coef = self->coef_;
    self->d0_ = (float) (fabs(out) * (1.0 + coef)) - self->d0_ * coef;
    self->d1_ = (float) (fabs(cmp) * (1.0 + coef)) - self->d1_ * coef;
    // guard div0
    float s = self->d0_ != 0.0f ? self->d1_ / self->d0_ : self->d1_;
    // clamp to avoid extremes
    self->scale_ = clamp(s, 0.1, 10.0);
}

void balance_init(balance* self, float sr) {
    self->sr = fabs(sr);
    self->coef_ = 0.0;
    self->d0_ = 0.0;
    self->d1_ = 0.0;
    self->scale_ = 0.0;

    update_(self);
}

void balance_tick_block(balance* self, float* out, float* cmp, uint32_t nsmps) {
    for (uint32_t i = 0; i < nsmps; i++) {
        tick_(self, out[i], cmp[i]);
        out[i] *= self->scale_;
    }
}
