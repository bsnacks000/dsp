/**
 * @brief sinesum - choose from on the class waveforms and render a table.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dsp/ftable/ftable.h>
#include <dsp/ftable/sinesum.h>
#include <dsp/utils.h>

#include "cli.h"
#include "common.h"
#include "wavio.h"

#define HARMS_MAX_SZ 256  // max allowed harmonics to generate
#define WT_BUF_SZ 1024    // size of resulting waveform buffer

typedef enum {
    APP_OK = 0,
    APP_ERR_INVALID_PATH,
    APP_ERR_INVALID_WF,
    APP_ERR_INVALID_NHARMS,
    APP_DSP_ERR
} app_err;

typedef enum {
    SINE = 0,
    SAW,
    SQR,
    TRI,
} waveform;

static bool validate_waveform(waveform w) {
    if (w >= 0 && w <= 3)
        return true;
    return false;
}

static bool validate_nharms(int nharms) {
    if (nharms > 0 && nharms <= HARMS_MAX_SZ && is_pow2(nharms))
        return true;
    return false;
}

static app_err entrypoint(const char* outfile, waveform wf, int nharms) {
    if (!validate_path(outfile, ".wav")) {
        return APP_ERR_INVALID_PATH;
    }
    if (!validate_waveform(wf)) {
        return APP_ERR_INVALID_WF;
    }
    if (!validate_nharms(nharms)) {
        return APP_ERR_INVALID_NHARMS;
    }

    // set amps ..
    // assure amp_sz is 1 for SINE
    size_t amps_sz;
    if (wf == SINE) {
        amps_sz = 1;
    } else {
        amps_sz = nharms;
    }

    float amps[amps_sz];
    memset(amps, 0, sizeof(float) * amps_sz);

    switch (wf) {
        case SQR:
            sqr_amps(amps, amps_sz);
            break;
        case SAW:
            saw_amps(amps, amps_sz);
            break;
        case TRI:
            tri_amps(amps, amps_sz);
            break;
        default:  // SINE
            amps[0] = 1.0;
            break;
    }

    // for (uint32_t i = 0; i < amps_sz; i++) {
    //     printf("%f\n", amps[i]);
    // }

    // // alloc ftable
    size_t buf_sz = WT_BUF_SZ + 2;  // don't forget guard point
    float buf[buf_sz];
    memset(buf, 0, sizeof(float) * buf_sz);

    ftable wt;
    ftable_init(&wt, buf, buf_sz);

    wt_sinesum_args args = {
        .amps = amps,
        .amps_sz = amps_sz,
        .phase = 0.0,
        .smooth = true,
        .nharms = amps_sz,
    };

    // // run .. sinesum infalliable but we check for good measure in case it changes.
    dsp_err err;
    if ((err = ftable_func(&wt, wt_sinesum, (void*) &args)) != DSP_OK) {
        return APP_DSP_ERR;
    }

    // not sure if i want this or not
    for (int i = 0; i < WT_BUF_SZ; i++) {
        printf("%f\n", wt.buf[i]);
    }

    // // open sf write .. set block size equal to wt buf sz
    // // this means we do not copy guard point
    // // sr = nharms for writing oscillator ftables.
    wavio w;
    wavio_open_write(&w, malloc, outfile, nharms, SF_FORMAT_WAV | SF_FORMAT_FLOAT, 1,
                     WT_BUF_SZ);
    wavio_fill_block(&w, wt.buf);
    wavio_write_block(&w);
    wavio_close(&w, free);

    return APP_OK;
}

int main(int argc, char** argv) {

    cli_command* c = cli_command_new();

    cli_err err;
    const char* desc = "Generate a table of summed sinusoids.";
    const char* usage = "[Options]... outfile.wav";

    if ((err = cli_init(c, desc, usage, argc, (char**) argv)) != CLI_OK) {
        exit_app_cli_err(c, err);
    }

    char outfile[64] = "";
    if ((err = cli_add_str_argument(c, outfile, 32))) {
        exit_app_cli_err(c, err);
    }

    waveform wf = SINE;
    const char* wf_usage = "Waveform: 0=sine, 1=saw, 2=sqr, 3=tri. default sine.\n";
    if ((err = cli_add_int_option(c, "w", wf_usage, (int*) &wf, false))) {
        exit_app_cli_err(c, err);
    }

    int nharms = 1;
    if ((err =
             cli_add_int_option(c, "n", "n harmonics. default 1.\n", &nharms, false))) {
        exit_app_cli_err(c, err);
    }

    if ((err = cli_parse(c)) != CLI_OK) {
        exit_app_cli_err(c, err);
    }

    // printf("wf: %d\n", wf);
    // printf("outfile: %s\n", outfile);
    app_err aerr;
    if ((aerr = entrypoint(outfile, wf, nharms)) != APP_OK) {
        fprintf(stderr, "error: %d\n", aerr);
    }

    exit_app(c);
}
