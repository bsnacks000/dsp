
#include <dsp/line.h>
#include <dsp/utils.h>

static inline void line_update_(line* self) {
    self->finished_ = false;
    self->nsmps_ = (uint32_t) nsmps_dur(self->sr, self->dur_sec);

    self->nsmps_ = self->nsmps_ <= 0 ? 1 : self->nsmps_;
    self->level_ = self->start;
    self->step_ = (self->stop - self->start) / (float) self->nsmps_;
}

static inline float line_tick_(line* self) {

    if (self->finished_)
        return self->level_;

    float out = self->level_;
    self->level_ += self->step_;
    self->nsmps_--;
    if (self->nsmps_ == 0)
        self->finished_ = true;

    return out;
}

void line_init(line* self, float start, float stop, float dur_sec, float sr) {
    self->start = start;
    self->stop = stop;
    self->dur_sec = dur_sec;
    self->sr = sr;

    line_update_(self);
}

void line_tick_block(line* self, float* out, uint32_t nsmps) {

    for (uint32_t i = 0; i < nsmps; i++) {
        out[i] = line_tick_(self);
    }
}

/**
 * Retriggering behavior --
 *  - we only retrigger if the previous gate was below threshold and current is above.
 *
 */

static inline float ar_tick_(line_ar* self) {
    bool prev_gate_below_thresh = self->prev_gate_ < self->gate_thresh;
    bool curr_gate_above_thresh = self->curr_gate_ >= self->gate_thresh;

    switch (self->stage_) {
        case AR_IDLE: {
            if (prev_gate_below_thresh && curr_gate_above_thresh) {
                self->stage_ = AR_ATK;
                return line_tick_(&self->state_);
            }
            return self->state_.level_;
        }
        case AR_ATK: {

            // retrigger occurrs during attack phase: reset the envelope to
            // instantaneous start/atk levels.
            if (prev_gate_below_thresh && curr_gate_above_thresh) {
                line_init(&self->state_, self->start_level, self->atk_level,
                          self->atk_sec, self->sr);
                return line_tick_(&self->state_);
            }

            float out = line_tick_(&self->state_);

            // the last tick updated finished state of line: reset the envelope to
            // rel levels and proceed to rel.
            if (self->state_.finished_) {
                self->stage_ = AR_REL;
                line_init(&self->state_, out, self->rel_level, self->rel_sec, self->sr);
            }

            return out;
        }

        case AR_REL: {

            // retrigger occurs: reset the envelope to current start/atk levels
            if (prev_gate_below_thresh && curr_gate_above_thresh) {
                self->stage_ = AR_ATK;
                line_init(&self->state_, self->start_level, self->atk_level,
                          self->atk_sec, self->sr);
                return line_tick_(&self->state_);
            }

            float out = line_tick_(&self->state_);

            // release is finished: reset to idle and current start/atk levels.
            if (self->state_.finished_) {
                self->stage_ = AR_IDLE;
                line_init(&self->state_, self->start_level, self->atk_level,
                          self->atk_sec, self->sr);
            }
            return out;
        }

        default:
            // TODO dsp_assert
            return 0.0;  // should never reach
    }
}

// analyze the gate signal
// float gate_delta = self->curr_gate_ - self->prev_gate_;

void line_ar_init(line_ar* self,
                  float gate_thresh,
                  float start_level,
                  float atk_sec,
                  float atk_level,
                  float rel_sec,
                  float rel_level,
                  float sr) {

    self->gate_thresh = gate_thresh;
    self->start_level = start_level;
    self->atk_sec = fabs(atk_sec);
    self->atk_level = atk_level;
    self->rel_sec = fabs(rel_sec);
    self->rel_level = rel_level;
    self->sr = sr;

    self->stage_ = AR_IDLE;
    self->curr_gate_ = 0.0;
    self->prev_gate_ = 0.0;

    line_init(&self->state_, self->start_level, self->atk_level, self->atk_sec,
              self->sr);
}

// NOTE: not sure about this yet ..
// possibly better to have an i_rate tick_block
void ar_tick_block(line_ar* self,
                   float* out,
                   float* gate,
                   float* gate_thresh,
                   float* start_level,
                   float* atk_sec,
                   float* atk_level,
                   float* rel_sec,
                   float* rel_level,
                   uint32_t nsmps) {
    for (uint32_t i = 0; i < nsmps; i++) {

        // update all inputs
        self->prev_gate_ = self->curr_gate_;
        self->curr_gate_ = gate[i];
        self->gate_thresh = (gate_thresh == NULL) ? self->gate_thresh : gate_thresh[i];
        self->start_level = (start_level == NULL) ? self->start_level : start_level[i];
        self->atk_sec = (atk_sec == NULL) ? self->atk_sec : atk_sec[i];
        self->atk_level = (atk_level == NULL) ? self->atk_level : atk_level[i];
        self->rel_sec = (rel_sec == NULL) ? self->rel_sec : rel_sec[i];
        self->rel_level = (rel_level == NULL) ? self->rel_level : rel_level[i];

        out[i] = ar_tick_(self);
    }
}

void line_adsr_init(line_adsr* self,
                    float gate_thresh,
                    float start_level,
                    float atk_sec,
                    float atk_level,
                    float decay_sec,
                    float sustain_level,
                    float rel_sec,
                    float rel_level,
                    float sr) {
    self->gate_thresh = gate_thresh;
    self->start_level = start_level;
    self->atk_sec = fabs(atk_sec);
    self->atk_level = atk_level;
    self->decay_sec = fabs(decay_sec);
    self->sustain_level = sustain_level;
    self->rel_sec = fabs(rel_sec);
    self->rel_level = rel_level;
    self->sr = sr;

    self->stage_ = ADSR_IDLE;
    self->curr_gate_ = 0.0;
    self->prev_gate_ = 0.0;

    line_init(&self->state_, self->start_level, self->atk_level, self->atk_sec,
              self->sr);
}

/**
 * Gate sustaining / early release logics0
 *  - while gate is above theshold run start/atk, decay, sus
 *    - if the current gate falls below move to release
 *  - if we are in the release stage and recv a new gate jump immediately to start/atk
 */

float adsr_tick_(line_adsr* self) {
    bool prev_gate_below_thresh = self->prev_gate_ < self->gate_thresh;
    bool curr_gate_above_thresh = self->curr_gate_ >= self->gate_thresh;

    switch (self->stage_) {
        case ADSR_IDLE: {

            // initial atk
            if (prev_gate_below_thresh && curr_gate_above_thresh) {
                self->stage_ = ADSR_ATK;
                return line_tick_(&self->state_);
            }
            return self->state_.level_;
        }
        case ADSR_ATK: {

            // early release
            if (!curr_gate_above_thresh) {
                self->stage_ = ADSR_REL;
                line_init(&self->state_, self->state_.level_, self->rel_level,
                          self->rel_sec, self->sr);
                return line_tick_(&self->state_);
            }

            float out = line_tick_(&self->state_);

            // next stage: decay
            if (self->state_.finished_) {
                self->stage_ = ADSR_DCY;
                line_init(&self->state_, out, self->sustain_level, self->decay_sec,
                          self->sr);
            }

            return out;
        }
        case ADSR_DCY: {
            if (!curr_gate_above_thresh) {
                self->stage_ = ADSR_REL;
                line_init(&self->state_, self->state_.level_, self->rel_level,
                          self->rel_sec, self->sr);
                return line_tick_(&self->state_);
            }
            float out = line_tick_(&self->state_);

            if (self->state_.finished_) {
                self->stage_ = ADSR_SUS;
            }

            return out;
        }
        case ADSR_SUS: {
            if (!curr_gate_above_thresh) {
                self->stage_ = ADSR_REL;
                line_init(&self->state_, self->state_.level_, self->rel_level,
                          self->rel_sec, self->sr);

                return line_tick_(&self->state_);
            }
            return self->state_.level_;
        }
        case ADSR_REL: {

            // retrigger occurs: reset the envelope to current start/atk levels
            if (prev_gate_below_thresh && curr_gate_above_thresh) {
                self->stage_ = ADSR_ATK;
                line_init(&self->state_, self->start_level, self->atk_level,
                          self->atk_sec, self->sr);
                return line_tick_(&self->state_);
            }

            float out = line_tick_(&self->state_);

            // release is finished: reset to idle and current start/atk levels.
            if (self->state_.finished_) {
                self->stage_ = ADSR_IDLE;
                line_init(&self->state_, self->start_level, self->atk_level,
                          self->atk_sec, self->sr);
            }
            return out;
        }
        default:
            return 0.0;
    }
}

void line_adsr_tick_block(line_adsr* self,
                          float* out,
                          float* gate,
                          float* gate_thresh,
                          float* start_level,
                          float* atk_sec,
                          float* atk_level,
                          float* decay_sec,
                          float* sustain_level,
                          float* rel_sec,
                          float* rel_level,
                          uint32_t nsmps) {
    for (uint32_t i = 0; i < nsmps; i++) {

        // update all inputs
        self->prev_gate_ = self->curr_gate_;
        self->curr_gate_ = gate[i];
        self->gate_thresh = (gate_thresh == NULL) ? self->gate_thresh : gate_thresh[i];
        self->start_level = (start_level == NULL) ? self->start_level : start_level[i];
        self->atk_sec = (atk_sec == NULL) ? self->atk_sec : atk_sec[i];
        self->atk_level = (atk_level == NULL) ? self->atk_level : atk_level[i];
        self->decay_sec = (decay_sec == NULL) ? self->decay_sec : decay_sec[i];
        self->sustain_level =
            (sustain_level == NULL) ? self->sustain_level : sustain_level[i];
        self->rel_sec = (rel_sec == NULL) ? self->gate_thresh : rel_sec[i];
        self->rel_level = (rel_level == NULL) ? self->gate_thresh : rel_level[i];

        out[i] = adsr_tick_(self);
    }
}
