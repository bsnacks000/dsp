#include "distort.h"
#include <dsp/maths.h>

#include <dsp/tabread.h>
#include <dsp/utils.h>
#include <dsp/wavetable/cheby.h>
#include <dsp/wavetable/ramp.h>

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

    // hard code some coeffs
    float coeffs[4] = {1.0, 0.25, 0.75, 0.3};
    float gain = 1.0;

    wt_cheby_args cheby_args;
    wt_cheby_args_init(&cheby_args, coeffs, 4, gain);

    if ((err = wt_cheby(&cheby3_wt, &cheby_args)) != DSP_OK) {
        return NOTOK;
    }

    return OK;
}

int cheby3_init(CSOUND* csound, cheby3* obj) {
    (void) csound;
    (void) obj;

    tabread_init(&obj->tr, &cheby3_wt);

    return OK;
}

int cheby3_vector(CSOUND* csound, cheby3* obj) {
    (void) csound;

    float* a_out = (float*) obj->a_out;
    float* a_in = (float*) obj->a_in;

    uint32_t nsmps = GetLocalKsmps(&obj->h);

    // covert to unipolar and scale to len wavetab len
    // (-1,1) -> (0, N)
    scale(a_out, a_in, 0.5, nsmps);
    dc_offset(a_out, a_out, 0.5, nsmps);
    scale(a_out, a_out, (float) (cheby3_wt.len - 1), nsmps);

    // read off the waveshaped value
    tabread3_tick_block(&obj->tr, a_out, a_out, nsmps);

    return OK;
}
