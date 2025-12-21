#include <dsp/ftable/fill.h>
#include <dsp/ftable/normalize.h>

static inline dsp_err fill_base_(ftable* wt, void* args) {
    wt_fill_args* args_ = (wt_fill_args*) args;
    if (args_->nsmps != wt->len) {
        return DSP_ERR;
    }
    copy_nsmps(wt->buf, args_->in, args_->nsmps);
    return DSP_OK;
}

dsp_err wt_fill(ftable* wt, void* args) {
    return fill_base_(wt, args);
}

dsp_err wt_fill_normalize(ftable* wt, void* args) {
    dsp_err err;
    if ((err = fill_base_(wt, args) != DSP_OK)) {
        return err;
    }
    normalize(wt->buf, wt->buf_sz);
    return DSP_OK;
}
