#include <dsp/assert.h>
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
    // ADDED guard to init
    // factor = factor == 0.0 ? 1e-6 : factor;

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

static inline float curve_tick_(curve* self) {
    if (self->finished_)
        return (float) self->level_;

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
    self->dur_sec = assure_gt_zero(dur_sec);
    self->factor = zero_guard(factor);
    self->sr = assure_gt_zero(sr);

    update_(self);
}

void curve_tick_block(curve* self, float* out, uint32_t start, uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = curve_tick_(self);
    }
}

// these are basically clones of linear AR/ADSR more or less
// TODO: maybe combine the internals of line/curve AR/ADSR into one API for sanity

static inline float ar_tick_(curve_ar* self) {
    bool prev_gate_below_thresh = self->prev_gate_ < self->gate_thresh;
    bool curr_gate_above_thresh = self->curr_gate_ >= self->gate_thresh;

    switch (self->stage_) {
        case AR_IDLE: {
            if (prev_gate_below_thresh && curr_gate_above_thresh) {
                self->stage_ = AR_ATK;
                // NOTE: must reinit or we can be off by one sample in some situations.
                curve_init(&self->state_, self->start_level, self->atk_level,
                           self->atk_sec, self->atk_crv, self->sr);
                return curve_tick_(&self->state_);
            }
            return (float) self->state_.level_;
        }
        case AR_ATK: {

            // retrigger occurs during attack phase: reset the envelope to
            // instantaneous start/atk levels.
            if (prev_gate_below_thresh && curr_gate_above_thresh) {
                curve_init(&self->state_, self->start_level, self->atk_level,
                           self->atk_sec, self->atk_crv, self->sr);
                return curve_tick_(&self->state_);
            }

            float out = curve_tick_(&self->state_);

            // the last tick updated finished state of line: reset the envelope to
            // rel levels and proceed to rel.
            if (self->state_.finished_) {
                self->stage_ = AR_REL;
                curve_init(&self->state_, out, self->rel_level, self->rel_sec,
                           self->rel_crv, self->sr);
            }

            return out;
        }

        case AR_REL: {

            // retrigger occurs: reset the envelope to current start/atk levels
            if (prev_gate_below_thresh && curr_gate_above_thresh) {
                self->stage_ = AR_ATK;
                curve_init(&self->state_, self->start_level, self->atk_level,
                           self->atk_sec, self->atk_crv, self->sr);
                return curve_tick_(&self->state_);
            }

            float out = curve_tick_(&self->state_);

            // release is finished: reset to idle and current start/atk levels.
            if (self->state_.finished_) {
                self->stage_ = AR_IDLE;
                curve_init(&self->state_, self->start_level, self->atk_level,
                           self->atk_sec, self->atk_crv, self->sr);
            }
            return out;
        }

        default:
            // TODO dsp_assert
            return 0.0;  // should never reach
    }
}

static inline float adsr_tick_(curve_adsr* self) {
    bool prev_gate_below_thresh = self->prev_gate_ < self->gate_thresh;
    bool curr_gate_above_thresh = self->curr_gate_ >= self->gate_thresh;

    switch (self->stage_) {
        case ADSR_IDLE: {

            // initial atk
            if (prev_gate_below_thresh && curr_gate_above_thresh) {
                self->stage_ = ADSR_ATK;
                curve_init(&self->state_, self->start_level, self->atk_level,
                           self->atk_sec, self->atk_crv, self->sr);
                return curve_tick_(&self->state_);
            }
            return (float) self->state_.level_;
        }
        case ADSR_ATK: {

            // early release
            if (!curr_gate_above_thresh) {
                self->stage_ = ADSR_REL;
                curve_init(&self->state_, (float) self->state_.level_, self->rel_level,
                           self->rel_sec, self->rel_crv, self->sr);
                return curve_tick_(&self->state_);
            }

            float out = curve_tick_(&self->state_);

            // next stage: decay
            if (self->state_.finished_) {
                self->stage_ = ADSR_DCY;
                curve_init(&self->state_, out, self->sustain_level, self->dcy_sec,
                           self->dcy_crv, self->sr);
            }

            return out;
        }
        case ADSR_DCY: {
            if (!curr_gate_above_thresh) {
                self->stage_ = ADSR_REL;
                curve_init(&self->state_, (float) self->state_.level_, self->rel_level,
                           self->rel_sec, self->rel_crv, self->sr);
                return curve_tick_(&self->state_);
            }
            float out = curve_tick_(&self->state_);

            if (self->state_.finished_) {
                self->stage_ = ADSR_SUS;
            }

            return out;
        }
        case ADSR_SUS: {
            if (!curr_gate_above_thresh) {
                self->stage_ = ADSR_REL;
                curve_init(&self->state_, (float) self->state_.level_, self->rel_level,
                           self->rel_sec, self->rel_crv, self->sr);

                return curve_tick_(&self->state_);
            }
            return (float) self->state_.level_;
        }
        case ADSR_REL: {

            // retrigger occurs: reset the envelope to current start/atk levels
            if (prev_gate_below_thresh && curr_gate_above_thresh) {
                self->stage_ = ADSR_ATK;
                curve_init(&self->state_, self->start_level, self->atk_level,
                           self->atk_sec, self->atk_crv, self->sr);
                return curve_tick_(&self->state_);
            }

            float out = curve_tick_(&self->state_);

            // release is finished: reset to idle and current start/atk levels.
            if (self->state_.finished_) {
                self->stage_ = ADSR_IDLE;
                curve_init(&self->state_, self->start_level, self->atk_level,
                           self->atk_sec, self->atk_crv, self->sr);
            }
            return out;
        }
        default:
            return 0.0;
    }
}

void curve_ar_init(curve_ar* self,
                   float gate_thresh,
                   float start_level,
                   float atk_sec,
                   float atk_crv,
                   float atk_level,
                   float rel_sec,
                   float rel_crv,
                   float rel_level,
                   float sr) {

    self->gate_thresh = gate_thresh;
    self->start_level = start_level;
    self->atk_sec = assure_gt_zero(atk_sec);
    self->atk_crv = zero_guard(atk_crv);
    self->atk_level = atk_level;
    self->rel_sec = assure_gt_zero(rel_sec);
    self->rel_crv = zero_guard(rel_crv);
    self->rel_level = rel_level;
    self->sr = sr;

    self->stage_ = AR_IDLE;
    self->curr_gate_ = 0.0;
    self->prev_gate_ = 0.0;

    curve_init(&self->state_, self->start_level, self->atk_level, self->atk_sec,
               self->atk_crv, self->sr);
}

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
                         uint32_t start,
                         uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {

        // update all inputs
        self->prev_gate_ = self->curr_gate_;
        self->curr_gate_ = gate[i];
        self->gate_thresh = gate_thresh[i];
        self->start_level = start_level[i];
        self->atk_sec = assure_gt_zero(atk_sec[i]);
        self->atk_crv = zero_guard(atk_crv[i]);
        self->atk_level = atk_level[i];
        self->rel_sec = assure_gt_zero(rel_sec[i]);
        self->rel_crv = zero_guard(rel_crv[i]);
        self->rel_level = rel_level[i];

        out[i] = ar_tick_(self);
    }
}

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
                     float sr) {

    self->gate_thresh = gate_thresh;
    self->start_level = start_level;
    self->atk_sec = assure_gt_zero(atk_sec);
    self->atk_crv = zero_guard(atk_crv);
    self->atk_level = atk_level;
    self->dcy_sec = assure_gt_zero(dcy_sec);
    self->dcy_crv = zero_guard(dcy_crv);
    self->sustain_level = sustain_level;
    self->rel_sec = assure_gt_zero(rel_sec);
    self->rel_crv = zero_guard(rel_crv);
    self->rel_level = rel_level;
    self->sr = sr;

    self->stage_ = ADSR_IDLE;
    self->curr_gate_ = 0.0;
    self->prev_gate_ = 0.0;

    curve_init(&self->state_, self->start_level, self->atk_level, self->atk_sec,
               self->atk_crv, self->sr);
}

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
                           uint32_t start,
                           uint32_t nsmps) {

    for (uint32_t i = start; i < nsmps; i++) {
        // update all inputs
        self->prev_gate_ = self->curr_gate_;
        self->curr_gate_ = gate[i];
        self->gate_thresh = gate_thresh[i];
        self->start_level = start_level[i];
        self->atk_sec = assure_gt_zero(atk_sec[i]);
        self->atk_level = atk_level[i];
        self->atk_crv = zero_guard(atk_crv[i]);
        self->dcy_sec = assure_gt_zero(dcy_sec[i]);
        self->dcy_crv = zero_guard(dcy_crv[i]);
        self->sustain_level = sustain_level[i];
        self->rel_sec = assure_gt_zero(rel_sec[i]);
        self->rel_crv = zero_guard(rel_crv[i]);
        self->rel_level = rel_level[i];

        out[i] = adsr_tick_(self);
    }
}
