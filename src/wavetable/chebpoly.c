#include <dsp/conversions.h>
#include <dsp/shape.h>
#include <dsp/wavetable/chebpoly.h>
#include <dsp/wavetable/normalize.h>
#include <stdint.h>
#include <sys/types.h>
#include "dsp/utils.h"

// TODO: check : sz >= 2; sum of coeffs != 0.0
dsp_err wt_chebpoly_args_init(wt_chebpoly_args* self, const float* h, uint32_t h_sz) {
    self->h = h;
    self->h_sz = h_sz;

    return DSP_OK;
}

/**
 * Calculate the chebyshev polynomial of x using harmonic weights h using the
 * standard recurrence relation. Assumes at least h_sz == 1
 */
static inline float chebyshev_polynomial(float x, const float* h, uint32_t h_sz) {
    uint32_t n = h_sz - 1;
    float s = 0.5 * h[0];
    if (n == 0) {
        return s;
    }

    float t_nm1 = 1.0;
    float t_n = x;

    s += h[1] * t_n;

    for (uint32_t k = 2; k < n + 1; k++) {
        float t_k = 2.0 * x * t_n - t_nm1;
        s += h[k] * t_k;
        t_nm1 = t_n;
        t_n = t_k;
    }
    return s;
}

dsp_err wt_chebpoly(wavetable* wt, void* args) {
    wt_chebpoly_args* args_ = (wt_chebpoly_args*) args;

    const float* h = args_->h;
    uint32_t h_sz = args_->h_sz;

    if (h_sz < 3) {
        return DSP_ERR;  // we need at least T0,T1,T2 to values to produce a waveform.
    }

    float* buf = wt->buf;
    uint32_t len = wt->len;

    for (uint32_t i = 0; i < len; i++) {
        buf[i] = chebyshev_polynomial(buf[i], h, h_sz);
    }

    normalize(buf, len);

    return DSP_OK;
}
