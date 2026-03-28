#include <dsp/samph.h>
#include <stdbool.h>

static inline float tick_(samph* self, float xn) {

    bool prev_gate_below_thresh = self->prev_gate_ < self->gate_thresh;
    bool curr_gate_above_thresh = self->curr_gate_ >= self->gate_thresh;

    if (prev_gate_below_thresh && curr_gate_above_thresh)
        self->curr_out_ = xn;

    return self->curr_out_;
}

void samph_init(samph* self, float gate_thresh) {
    self->gate_thresh = gate_thresh;
    self->curr_gate_ = self->prev_gate_ = self->curr_out_ = 0.0f;
}

void samph_tick_block(samph* self,
                      float* out,
                      float* in,
                      float* gate,
                      float* gate_thresh,
                      uint32_t start,
                      uint32_t nsmps) {

    for (uint32_t i = start; i < nsmps; i++) {
        self->prev_gate_ = self->curr_gate_;
        self->curr_gate_ = gate[i];
        self->gate_thresh = gate_thresh[i];

        out[i] = tick_(self, in[i]);
    }
}
