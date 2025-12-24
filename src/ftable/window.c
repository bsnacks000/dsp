#include <dsp/constants.h>
#include <dsp/ftable/window.h>

// ref: https://github.com/csound/csound/blob/develop/Engine/fgens.c#L1043

dsp_err ft_window(ftable* wt, void* args) {
    ft_window_args* args_ = (ft_window_args*) args;
    ft_window_type type = args_->type;

    if (!wt->is_pow2) {
        return DSP_ERR;  // code: 1
    }

    float* buf = wt->buf;
    uint32_t len = wt->len;

    // several windows just require us to set coeffs so we can store here.
    double cf[4];

    // simplfying from csnd for now... these can be controlled from caller but we
    // just set defaults for now. The windows can always be scaled outside of this
    // generator
    double xarg = 1.0;
    double varian = 1.0;

    uint32_t i;  // index and accumulators need declare here
    double arg = 0.0;
    double x;
    switch (type) {
        case (WINDOW_HAMMING):
            cf[0] = 0.54;
            cf[1] = 0.46;
            cf[2] = cf[3] = 0.0;
            break;

        case (WINDOW_HANNING):
            cf[0] = cf[1] = 0.5;
            cf[2] = cf[3] = 0.0;
            break;

        case (WINDOW_BLACKMAN):
            cf[0] = 0.42;
            cf[1] = 0.5;
            cf[2] = 0.08;
            cf[3] = 0.0;
            break;

        case (WINDOW_BLACKMAN_HARRIS):
            cf[0] = 0.35878;
            cf[1] = 0.48829;
            cf[2] = 0.14128;
            cf[3] = 0.01168;
            break;

        case (WINDOW_GAUSS):
            arg = 12.0 / len;
            // left
            for (i = 0, x = -6.0; i < len >> 1; i++, x += arg) {
                buf[i] = xarg * (pow(E, -(x * x) / (2.0 * varian * varian)));
            }
            // right
            for (x = 0.0; i < len; i++, x += arg) {
                buf[i] = xarg * (pow(E, -(x * x) / (2.0 * varian * varian)));
            }
            return DSP_OK;

        case (WINDOW_SINC):
            arg = TWO_PI * varian / len;
            // up
            for (i = 0, x = -DSP_PI * varian; i < len >> 1; i++, x += arg) {
                buf[i] = xarg * sin(x) / x;
            }
            buf[i++] = xarg;  // top point
            // down
            for (x = arg; i < len; i++, x += arg) {
                buf[i] = xarg * sin(x) / x;
            }
            return DSP_OK;

        default:
            return DSP_ERR + 1;  // code 2
    }

    arg = TWO_PI / len;

    for (i = 0, x = 0.0; i < len; i++, x += arg) {
        buf[i] = (float) (xarg * (cf[0] - cf[1] * cos(x) + cf[2] * cos(2.0 * x) -
                                  cf[3] * cos(3.0 * x)));
    }

    return DSP_OK;
}
