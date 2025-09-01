/**
 * @brief sinesum - choose from on the class waveforms and render a table.
 */

#include <stdint.h>
#include <stdio.h>

#include <dsp/utils.h>
#include <dsp/wavetable/cheby.h>
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

    // // open sf write .. set block size equal to wt buf sz
    // // this means we do not copy guard point
    // // sr = nharms for writing oscillator wavetables.
    // wavio w;
    // wavio_open_write(&w, malloc, outfile, nharms, 1, WT_BUF_SZ);
    // wavio_fill_block(&w, wt.buf);
    // wavio_write_block(&w);
    // wavio_close(&w, free);

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
