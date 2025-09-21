#include "distort.h"
#include <dsp/maths.h>
#include <dsp/rmap.h>
#include <dsp/utils.h>
#include <dsp/wavetable/cheby.h>
#include <dsp/wavetable/ramp.h>
#include "dsp/balance.h"
#include "dsp/conversions.h"
#include "dsp/tabread.h"
#include "dsp/xfade.h"

#define WT_BUF_SZ 1026

static float cheby3_wt_buf[WT_BUF_SZ] = {0};

static wavetable cheby3_wt;

int cheby3_tab_init(CSOUND* csound) {
    (void) csound;
    wavetable_init(&cheby3_wt, cheby3_wt_buf, WT_BUF_SZ);

    int err;

    // first create bipolar linear ramp
    wt_ramp_args ramp_args = {
        .start = -1.0,
        .stop = 1.0,
        .endpoint = true,
    };

    if ((err = wt_linspace(&cheby3_wt, &ramp_args)) != DSP_OK) {
        return NOTOK;
    }

    // cheby3 = 3 coeffs set gain to 1 for now;
    float coeffs[3] = {1.0, 1.0, 0.3};
    float gain = 0.5;

    wt_cheby_args cheby_args;
    wt_cheby_args_init(&cheby_args, coeffs, 3, gain);

    if ((err = wt_cheby(&cheby3_wt, &cheby_args)) != DSP_OK) {
        return NOTOK;
    }

    return OK;
}

int cheby3_init(CSOUND* csound, cheby3* obj) {
    (void) csound;
    (void) obj;

    MYFLT sr = GetLocalSr(&obj->h);

    tabread_init(&obj->tr, &cheby3_wt);
    balance_init(&obj->bal, sr);
    xfade_init(&obj->xf, 0.0);

    return OK;
}

static inline void clamp_block(float* out,
                               float* in,
                               float min,
                               float max,
                               uint32_t nsmps) {
    for (uint32_t i = 0; i < nsmps; i++) {
        out[i] = clamp(in[i], min, max);
    }
}

static inline void linlin_block(float* out,
                                float* in,
                                float src_lo,
                                float src_hi,
                                float dst_lo,
                                float dst_hi,
                                uint32_t nsmps) {
    for (uint32_t i = 0; i < nsmps; i++) {
        out[i] = linlin(in[0], src_lo, src_hi, dst_lo, dst_hi);
    }
}

int cheby3_vector(CSOUND* csound, cheby3* obj) {
    (void) csound;

    float* a_out = (float*) obj->a_out;
    float* a_in = (float*) obj->a_in;
    float* a_drive = (float*) obj->a_drive;
    float* a_amt = (float*) obj->a_amt;

    uint32_t nsmps = GetLocalKsmps(&obj->h);

    // clamp the drive signal (linear range 0-3)
    // clamp_block(a_drive, a_drive, 0.0, 2.0, nsmps);

    // apply drive to input and write to a_out
    // mult_block(a_out, a_in, a_drive, nsmps);

    // covert to unipolar and scale to len wavetab len
    // (-1,1) -> (0, N)
    // scale(a_out, a_out, 0.5, nsmps);
    // dc_offset(a_out, a_out, 0.5, nsmps);
    // scale(a_out, a_out, cheby3_wt.len, nsmps);

    linlin_block(a_out, a_in, -1.0, 1.0, 0.0, (float) cheby3_wt.len - 1, nsmps);

    // csound->Message(csound, "%f\n", *a_out);

    // read off the waveshaped value
    tabreadi_tick_block(&obj->tr, a_out, a_out, nsmps);

    // // balance with a_in
    // balance_tick_block(&obj->bal, a_out, a_in, nsmps);

    // // apply xfade and mix wet/dry (left dry right wet)
    // xfade_tick_block(&obj->xf, a_out, a_in, a_out, a_amt, nsmps);
    // add_block(a_out, a_in, a_out, nsmps);

    return OK;
}
