#include <dsp/conversions.h>
#include <dsp/delay.h>
#include <dsp/interpolate.h>
#include <dsp/utils.h>

// TODO: unify with ftable ... better pow2 semantics

uint32_t delay_line_calculate_buf_sz(float sr, float ms) {
    float samp_ms = samps_per_ms(sr);
    // adding 1.0 accounts for fractional size
    float l = samp_ms * ms + 1.0;
    return (uint32_t) ceiling_pow2(l) + 2;  // add 2 for guard point
}

void delay_line_init(delay_line* self, float* buf, uint32_t buf_sz) {
    self->buf = buf;
    self->buf_sz = buf_sz;              // assume pow2 + 2 for now (ex. 1026)
    self->wrap_sz_ = self->buf_sz - 2;  // (1024)
    self->mask_ = self->wrap_sz_ - 1;   // (1023)
    self->write_idx = 0;
}

void delay_line_advance(delay_line* self, uint32_t nsmps) {
    self->write_idx = (self->write_idx + nsmps) & self->mask_;
}

void delay_line_write(delay_line* self, float* in, uint32_t nsmps) {

    uint32_t mask = self->mask_;
    uint32_t idx = self->write_idx;
    uint32_t wrap_sz = self->wrap_sz_;
    float* buf = self->buf;

    for (uint32_t i = 0; i < nsmps; i++) {
        uint32_t i_ = (idx + i) & mask;
        buf[i_] = in[i];
        if (i_ == 0) {
            buf[wrap_sz] = buf[0];
            buf[wrap_sz + 1] = buf[1];
        }  // update guard point on wrap
    }
}

void delay_line_tapi(delay_line* self, float* out, float* offset, uint32_t nsmps) {
    uint32_t mask = self->mask_;

    for (uint32_t i = 0; i < nsmps; i++) {
        float initial_pos = (float) self->write_idx - offset[i];
        // wrap if negative
        while (initial_pos < 0)
            initial_pos += (float) self->wrap_sz_;

        float pos = initial_pos + (float) i;
        int ipos = (int) pos;
        float frac = pos - ipos;

        float a = self->buf[ipos & mask];
        float b = self->buf[(ipos + 1) & mask];

        out[i] = interpolate_linear(a, b, frac);
    }
}

void delay_line_tap3(delay_line* self, float* out, float* offset, uint32_t nsmps) {
    uint32_t mask = self->mask_;

    for (uint32_t i = 0; i < nsmps; i++) {
        float initial_pos = (float) self->write_idx - offset[i];
        // wrap if negative
        while (initial_pos < 0)
            initial_pos += (float) self->wrap_sz_;

        float pos = initial_pos + (float) i;
        int ipos = (int) pos;
        float frac = pos - ipos;

        float a = self->buf[(ipos - 1) & mask];
        float b = self->buf[ipos & mask];
        float c = self->buf[(ipos + 1) & mask];
        float d = self->buf[(ipos + 2) & mask];

        out[i] = interpolate_cubic(a, b, c, d, frac);
    }
}
