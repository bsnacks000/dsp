
#include <dsp/line.h>
#include <dsp/utils.h>

static inline void update_(line* self) {
    self->finished_ = false;
    self->nsmps_ = (uint32_t) nsmps_dur(self->sr, self->dur_sec);
    // assure 1 sample
    self->nsmps_ = self->nsmps_ ? self->nsmps_ > 0 : 1;
    self->level_ = self->start;
    self->step_ = fabsf(self->stop - self->start) / (float) self->nsmps_;
}

static inline float tick_(line* self) {
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

    update_(self);
}

void line_tick_block(line* self, float* out, uint32_t nsmps) {

    for (uint32_t i = 0; i < nsmps; i++) {
        out[i] = tick_(self);
    }
}
