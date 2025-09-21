/**
 * @brief chebytab -- generate an Nth order cheby polinomial and print to .wav
 *
 * NOTE: currently a POC / WIP
 *  - this version calculates fixed coeff set.
 */

#include <stdint.h>
#include <stdio.h>

#include <dsp/utils.h>
#include <dsp/wavetable/cheby.h>
#include <dsp/wavetable/ramp.h>
#include <dsp/wavetable/wavetable.h>

#include "cli.h"
#include "common.h"
#include "wavio.h"

#define WT_BUF_SZ 1024  // size of resulting waveform buffer

typedef enum {
    APP_OK = 0,
    APP_ERR_INVALID_PATH,
    APP_DSP_ERR,
} app_err;

app_err entrypoint(const char* outfile) {
    if (!validate_path(outfile, ".wav")) {
        return APP_ERR_INVALID_PATH;
    }

    dsp_err err;

    // we use a straight line to compute the series though it could
    // be any function really..

    // remeber to add the plus 2 guard point
    float wt_buf[WT_BUF_SZ + 2] = {0};
    wavetable wt;
    wavetable_init(&wt, wt_buf, WT_BUF_SZ + 2);

    // calc a line between -1.0 -> 1.0
    wt_ramp_args ramp_args = {
        .start = -1.0,
        .stop = 1.0,
        .endpoint = true,
    };

    if ((err = wt_linspace(&wt, &ramp_args)) != DSP_OK) {
        return APP_DSP_ERR;
    }

    float coeffs[4] = {1.0, 1.0, 1.0, 0.0};
    float gain = 1.0;

    wt_cheby_args cheby_args;
    wt_cheby_args_init(&cheby_args, coeffs, 4, gain);

    if ((err = wt_cheby(&wt, &cheby_args)) != DSP_OK) {
        return APP_DSP_ERR + 1;
    }

    // // open sf write .. set block size equal to wt buf sz
    // // this means we do not copy guard point
    // // sr = nharms for writing oscillator wavetables.
    wavio w;
    wavio_open_write(&w, malloc, outfile, WT_BUF_SZ, 1, WT_BUF_SZ);
    wavio_fill_block(&w, wt.buf);
    wavio_write_block(&w);
    wavio_close(&w, free);

    return APP_OK;
}

int main(int argc, char** argv) {

    cli_command* c = cli_command_new();

    cli_err err;
    const char* desc = "Generate a table of summed chebyshev polynomials.";
    const char* usage = "[Options]... outfile.wav";

    if ((err = cli_init(c, desc, usage, argc, (char**) argv)) != CLI_OK) {
        exit_app_cli_err(c, err);
    }

    char outfile[64] = "";
    if ((err = cli_add_str_argument(c, outfile, 32))) {
        exit_app_cli_err(c, err);
    }

    if ((err = cli_parse(c)) != CLI_OK) {
        exit_app_cli_err(c, err);
    }

    // printf("wf: %d\n", wf);
    // printf("outfile: %s\n", outfile);
    app_err aerr;
    if ((aerr = entrypoint(outfile)) != APP_OK) {
        fprintf(stderr, "error: %d\n", aerr);
    }

    exit_app(c);
}
