#include <dsp/nonlin.h>
#include <dsp/utils.h>
#include <dsp/wavetable/ramp.h>
#include <stdint.h>

dsp_err wt_linspace(wavetable* wt, void* args) {
    wt_ramp_args* args_ = (wt_ramp_args*) args;

    float start = args_->start;
    float stop = args_->stop;
    bool endpoint = args_->endpoint;
    uint32_t num = wt->len;

    if (num < 2) {
        return DSP_ERR;
    }

    float step;
    if (endpoint) {
        step = (stop - start) / (float) (num - 1);
    } else {
        step = (stop - start) / (float) num;
    }

    for (uint32_t i = 0; i < num; i++) {
        wt->buf[i] = start + i * step;
    }

    return DSP_OK;
}

dsp_err wt_geomspace(wavetable* wt, void* args) {
    wt_ramp_args* args_ = (wt_ramp_args*) args;

    float start = args_->start;
    float stop = args_->stop;
    uint32_t num = wt->len;

    // assure 0.0 does not introduce ub.
    start += 1e-9;
    stop += 1e-9;

    // rebuild log args - take abs to assure we are non-negative.
    float log_start = log10(fabs(start));
    float log_stop = log10(fabs(stop));

    wt_ramp_args log_args = {
        .start = log_start,
        .stop = log_stop,
        .endpoint = args_->endpoint,
    };

    // first build linear space;
    dsp_err err;
    if ((err = wt_linspace(wt, (void*) &log_args)) != DSP_OK) {
        return DSP_ERR;
    }

    // now re project across log space
    for (uint32_t i = 0; i < num; i++) {
        wt->buf[i] = pow(10.0, wt->buf[i]);
    }

    return DSP_OK;
}
