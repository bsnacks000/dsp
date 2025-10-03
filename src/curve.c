#include <dsp/curve.h>
#include <dsp/utils.h>
#include <math.h>

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

void curve_tick_block(curve* self, float* out, uint32_t nsmps) {

    for (uint32_t i = 0; i < nsmps; i++) {
        out[i] = tick_(self);
    }
}

static inline float ar_tick_(curve_ar* self);

static inline float adsr_tick_(curve_adsr* self);

void curve_ar_init(curve_ar* self,
                   float gate_thresh,
                   float start_level,
                   float atk_sec,
                   float atk_crv,
                   float atk_level,
                   float rel_sec,
                   float rel_crv,
                   float rel_level,
                   float sr);

void curve_ar_tick_block(curve_ar* self,
                         float* out,
                         float* gate,
                         float* gate_thresh,
                         float* start_level,
                         float* atk_sec,
                         float* atk_crv,
                         float* atk_level,
                         float* rel_sec,
                         float* rel_crv,
                         float* rel_level,
                         uint32_t nsmps);

void curve_adsr_init(curve_adsr* self,
                     float gate_thresh,
                     float start_level,
                     float atk_sec,
                     float atk_crv,
                     float atk_level,
                     float dcy_sec,
                     float dcy_crv,
                     float sustain_level,
                     float rel_sec,
                     float rel_crv,
                     float rel_level,
                     float sr);

void curve_adsr_tick_block(curve_adsr* self,
                           float* out,
                           float* gate,
                           float* gate_thresh,
                           float* start_level,
                           float* atk_sec,
                           float* atk_crv,
                           float* atk_level,
                           float* dcy_sec,
                           float* dcy_crv,
                           float* sustain_level,
                           float* rel_sec,
                           float* rel_crv,
                           float* rel_level,
                           uint32_t nsmps);
