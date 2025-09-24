#include "distort.h"
#include <dsp/maths.h>

#include <dsp/tabread.h>
#include <dsp/utils.h>
#include <dsp/wavetable/chebpoly.h>
#include <dsp/wavetable/ramp.h>

#define WT_BUF_SZ 1026

static float chebsaw_buf[WT_BUF_SZ] = {0};

static wavetable chebsaw_wt;

int chebsaw_tab_init(CSOUND* csound) {
    (void) csound;
    wavetable_init(&chebsaw_wt, chebsaw_buf, WT_BUF_SZ);

    int err;

    // first create bipolar linear ramp
    wt_ramp_args ramp_args = {
        .start = -1.0,
        .stop = 1.0,
        .endpoint = true,
    };

    if ((err = wt_linspace(&chebsaw_wt, &ramp_args)) != DSP_OK) {
        return NOTOK;
    }

    // ~saw wave coeffs from csound gen13 example
    float h[16] = {
        0.0,  100.0, -50.0, -33.0, 25.0,  20.0, -16.7, -14.2,
        12.5, 11.1,  -10.0, -9.09, 8.333, 7.69, -7.14, -6.67,
    };

    wt_chebpoly_args cheby_args;
    wt_chebpoly_args_init(&cheby_args, h, 16);

    if ((err = wt_chebpoly(&chebsaw_wt, &cheby_args)) != DSP_OK) {
        return NOTOK;
    }

    return OK;
}

int chebsaw_init(CSOUND* csound, chebsaw* obj) {
    (void) csound;
    (void) obj;

    tabread_init(&obj->tr, &chebsaw_wt);

    return OK;
}

int chebsaw_vector(CSOUND* csound, chebsaw* obj) {
    (void) csound;

    float* a_out = (float*) obj->a_out;
    float* a_in = (float*) obj->a_in;

    uint32_t nsmps = GetLocalKsmps(&obj->h);

    // covert to unipolar and scale to len wavetab len
    // (-1,1) -> (0, N)
    scale(a_out, a_in, 0.5, nsmps);
    dc_offset(a_out, a_out, 0.5, nsmps);
    scale(a_out, a_out, (float) (chebsaw_wt.len - 1), nsmps);

    // read off the waveshaped value
    tabread3_tick_block(&obj->tr, a_out, a_out, nsmps);

    return OK;
}
