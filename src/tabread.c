#include <dsp/interpolate.h>
#include <dsp/tabread.h>

// TODO: implement multiple methods .. zero clip (zclip) and wrap (wrp)
//      -- (zclip we have)
//      -- do this after we have a stable `table` struct/API to sue
//  Ex. wrap..
//       while (val < 0) val += buf_sz;
//       while (val >= buf_sz) val -= buf_sz;

void tabread_init(tabread* self, wavetable* wt) {
    self->wt = wt;
}

void tabreadn_tick_block(tabread* self, float* out, float* idx, uint32_t nsmps) {
    float* buf = self->wt->buf;
    uint32_t len = self->wt->len;

    for (uint32_t i = 0; i < nsmps; i++) {
        float val = idx[i];
        if (val < 0.0 || val >= (float) len) {
            out[i] = 0.0;
            continue;
        }
        uint32_t ipos = (uint32_t) idx[i];
        out[i] = buf[ipos];
    }
}

void tabreadi_tick_block(tabread* self, float* out, float* idx, uint32_t nsmps) {
    float* buf = self->wt->buf;
    uint32_t len = self->wt->len;

    for (uint32_t i = 0; i < nsmps; i++) {
        float val = idx[i];
        if (val < 0.0 || val >= (float) len) {
            out[i] = 0.0;
            continue;
        }
        uint32_t ipos = (uint32_t) val;
        float frac = val - ipos;

        float a = buf[ipos];
        float b = buf[ipos + 1];

        out[i] = interpolate_linear(a, b, frac);
    }
}

void tabread3_tick_block(tabread* self, float* out, float* idx, uint32_t nsmps) {
    float* buf = self->wt->buf;
    uint32_t len = self->wt->len;

    for (uint32_t i = 0; i < nsmps; i++) {
        float val = idx[i];
        if (val < 0.0 || val >= (float) len) {
            out[i] = 0.0;
            continue;
        }
        uint32_t ipos = (uint32_t) val;
        float frac = val - ipos;

        float a = ipos > 0 ? buf[ipos - 1] : buf[len - 1];
        float b = buf[ipos];
        float c = buf[ipos + 1];
        float d = buf[ipos + 2];

        out[i] = interpolate_cubic(a, b, c, d, frac);
    }
}
