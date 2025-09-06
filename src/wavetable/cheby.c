#include <dsp/shape.h>
#include <dsp/wavetable/cheby.h>
#include <dsp/wavetable/normalize.h>
#include <math.h>
#include <stdint.h>
#include <sys/types.h>
#include "dsp/conversions.h"

// TODO: check : sz >= 2; sum of coeffs != 0.0
dsp_err wt_cheby_args_init(wt_cheby_args* self,
                           const float* coeffs,
                           uint32_t coeffs_sz,
                           float gain) {
    self->coeffs = coeffs;
    self->coeffs_sz = coeffs_sz;
    self->gain = gain;

    return DSP_OK;
}

/**
 * @brief computes the chebyshev series for a single sample of the waveform.
 *  NOTE: the sample is clipped (-1, 1)
 */
static inline float chebyshev_series(float xn,
                                     const float* coeffs,
                                     uint32_t coeffs_sz) {
    xn = hard_clip(xn, 1.0);
    float theta = acos(xn);

    float result = 0.0;
    for (uint32_t i = 0; i < coeffs_sz; i++) {
        result += coeffs[i] * cos(i * theta);
    }

    return result;
}

dsp_err wt_cheby(wavetable* wt, void* args) {
    wt_cheby_args* args_ = (wt_cheby_args*) args;

    const float* coeffs = args_->coeffs;
    uint32_t coeffs_sz = args_->coeffs_sz;
    float gain = args_->gain;

    float* buf = wt->buf;
    uint32_t len = wt->len;

    // clip the gain (-1, 1) to avoid signal blow up.
    gain = hard_clip(gain, 1.0);

    for (uint32_t i = 0; i < len; i++) {
        buf[i] = chebyshev_series(buf[i] * gain, coeffs, coeffs_sz);
    }

    normalize(buf, len);
    // NOTE: should make this a config option since default polynomials
    // are unipolar ..
    for (uint32_t i = 0; i < len; i++) {
        buf[i] = unipolar_to_bipolar(buf[i]);
    }

    return DSP_OK;
}
