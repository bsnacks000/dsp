#include <dsp/curve.h>
#include <dsp/utils.h>
#include <math.h>

// static inline void update_(curve* self) {
//     self->finished_ = false;
//     self->nsmps_ = (uint32_t) nsmps_dur(self->sr, self->dur_sec);
//     // assure 1 sample
//     self->nsmps_ = self->nsmps_ <= 0 ? 1 : self->nsmps_;

//     // assure math doesn't break
//     self->factor = self->factor == 0.0 ? 1e-6 : self->factor;

//     // build line maths
//     self->level_ = self->start;
//     self->a1_ = (self->stop - self->start) / (1.0 - expf(self->factor));
//     self->a2_ = self->start + self->a1_;
//     self->b1_ = self->a1_;

//     // calc growth factor
//     self->grow_ = expf(self->factor / self->nsmps_);
// }

static inline void update_(curve* self) {
    self->finished_ = false;

    double nsmps_frac = (double) self->sr * fabs((double) self->dur_sec);
    self->nsmps_ = (uint32_t) nsmps_frac;
    // assure 1 sample
    self->nsmps_ = self->nsmps_ <= 0 ? 1 : self->nsmps_;

    // upcast our params
    double factor = (double) self->factor;
    double start = (double) self->start;
    double stop = (double) self->stop;

    // assure math doesn't break
    factor = factor == 0.0 ? 1e-6 : factor;

    if (fabs(factor) < 0.01) {
        factor = (factor < 0.0 ? -0.01 : 0.01);
    }

    // build line maths
    self->level_ = start;
    self->a1_ = (stop - start) / (1.0 - exp(factor));
    self->a2_ = start + self->a1_;
    self->b1_ = self->a1_;

    // calc growth factor
    self->grow_ = exp(factor / nsmps_frac);
}

static inline float tick_(curve* self) {
    if (self->finished_)
        return self->level_;

    // downcast here
    float out = (float) self->level_;

    self->b1_ *= self->grow_;
    self->level_ = self->a2_ - self->b1_;

    self->nsmps_--;
    if (self->nsmps_ == 0)
        self->finished_ = true;

    return out;
}

/**
 * @brief curve init.
 */
void curve_init(curve* self,
                float start,
                float stop,
                float dur_sec,
                float factor,
                float sr) {

    self->start = start;
    self->stop = stop;
    self->dur_sec = dur_sec;
    self->factor = factor;
    self->sr = sr;

    update_(self);
}

/**
 * @brief curve tick block.
 */
void curve_tick_block(curve* self, float* out, uint32_t nsmps) {

    for (uint32_t i = 0; i < nsmps; i++) {
        out[i] = tick_(self);
    }
}
