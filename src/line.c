
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

static inline float arline_tick_(arline* self) {
    bool prev_gate_below_thresh = self->prev_gate_ < self->gate_thresh;
    bool curr_gate_above_thresh = self->curr_gate_ >= self->gate_thresh;

    switch (self->stage_) {
        case ARLINE_OFF: {
            if (prev_gate_below_thresh && curr_gate_above_thresh) {
                self->stage_ = ARLINE_ATK;
                return line_tick_(&self->state_);
            }
            return self->state_.level_;
        }
        case ARLINE_ATK: {
            float out = line_tick_(&self->state_);

            if (prev_gate_below_thresh && curr_gate_above_thresh) {
                line_init(&self->state_, self->start_level, self->atk_level,
                          self->atk_sec, self->sr);
                return out;
            }

            if (self->state_.finished_) {
                self->stage_ = ARLINE_REL;
                line_init(&self->state_, out, self->rel_level, self->rel_sec, self->sr);
            }

            return out;
        }

        case ARLINE_REL: {
            float out = line_tick_(&self->state_);

            if (prev_gate_below_thresh && curr_gate_above_thresh) {
                self->stage_ = ARLINE_ATK;
                line_init(&self->state_, self->start_level, self->atk_level,
                          self->atk_sec, self->sr);
                return out;
            }

            if (self->state_.finished_) {
                self->stage_ = ARLINE_OFF;
                line_init(&self->state_, self->start_level, self->atk_level,
                          self->atk_sec, self->sr);
            }
            return out;
        }

        default:
            break;
    }
}

// analyze the gate signal
// float gate_delta = self->curr_gate_ - self->prev_gate_;

void arline_init(arline* self,
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

    self->stage_ = ARLINE_OFF;
    self->curr_gate_ = 0.0;
    self->prev_gate_ = 0.0;

    line_init(&self->state_, self->start_level, self->atk_level, self->atk_sec,
              self->sr);
}

void arline_tick_block(arline* self,
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
        self->start_level = (start_level == NULL) ? self->gate_thresh : start_level[i];
        self->atk_sec = (atk_sec == NULL) ? self->gate_thresh : atk_sec[i];
        self->atk_level = (atk_level == NULL) ? self->gate_thresh : atk_level[i];
        self->rel_sec = (rel_sec == NULL) ? self->gate_thresh : rel_sec[i];
        self->rel_level = (rel_level == NULL) ? self->gate_thresh : rel_level[i];

        out[i] = arline_tick_(self);
    }
}
