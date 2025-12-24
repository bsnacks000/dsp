/**
 * @brief wintab - window to .wav file
 */
#include <dsp/ftable/ftable.h>
#include <dsp/ftable/window.h>
#include <sndfile.h>

#include "cli.h"
#include "common.h"
#include "wavio.h"

#define ft_BUF_SZ 1024  // size of the window buffer

typedef enum {
    APP_OK = 0,
    APP_ERR_INVALID_PATH,
    APP_ERR_INVALID_WIND,
    APP_DSP_ERR
} app_err;

bool validate_window(ft_window_type w) {
    if (w >= 0 && w <= 5) {
        return true;
    }
    return false;
}

app_err entrypoint(const char* outfile, ft_window_type wind) {

    if (!validate_path(outfile, ".wav")) {
        return APP_ERR_INVALID_PATH;
    }

    if (!validate_window(wind)) {
        return APP_ERR_INVALID_WIND;
    }

    ft_window_args args = {.type = wind};

    float ft_buf[ft_BUF_SZ + 2] = {0};
    ftable wt;
    ftable_init(&wt, ft_buf, ft_BUF_SZ + 2);

    dsp_err err;
    if ((err = ft_window(&wt, &args)) != DSP_OK) {
        return APP_DSP_ERR;
    }

    for (uint32_t i = 0; i < wt.len; i++) {
        if (wt.buf[i] < 0.0) {
            printf("index %d < 0 ... %f\n", i, wt.buf[i]);
        }
    }

    wavio w;
    wavio_open_write(&w, malloc, outfile, ft_BUF_SZ, 1, SF_FORMAT_WAV | SF_FORMAT_FLOAT,
                     ft_BUF_SZ);
    wavio_fill_block(&w, wt.buf);
    wavio_write_block(&w);
    wavio_close(&w, free);

    return APP_OK;
}

int main(int argc, char** argv) {

    cli_command* c = cli_command_new();
    cli_err err;
    const char* desc = "Generate a window.";
    const char* usage = "[Options]... outfile.wav";

    if ((err = cli_init(c, desc, usage, argc, (char**) argv)) != CLI_OK) {
        exit_app_cli_err(c, err);
    }

    char outfile[64] = "";
    if ((err = cli_add_str_argument(c, outfile, 32))) {
        exit_app_cli_err(c, err);
    }

    ft_window_type wind = WINDOW_HAMMING;
    const char* wind_usage =
        "Window: 0=hamming,1=hanning,2=blackman,3=blackman-harris,4=gauss,5=sinc";
    if ((err = cli_add_int_option(c, "w", wind_usage, (int*) &wind, false)) != CLI_OK) {
        exit_app_cli_err(c, err);
    }

    if ((err = cli_parse(c)) != CLI_OK) {
        exit_app_cli_err(c, err);
    }

    app_err aerr;
    if ((aerr = entrypoint(outfile, wind)) != APP_OK) {
        fprintf(stderr, "error: %d\n", aerr);
    }

    exit_app(c);
}
