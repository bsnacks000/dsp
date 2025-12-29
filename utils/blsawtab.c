#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dsp/ftable/ftable.h>
#include <dsp/ftable/sinesum.h>
#include "cli.h"
#include "common.h"
#include "wavio.h"

#define TARGET_SAMPLERATE 48000.0f
#define ftable_BUF_SZ 1024

typedef enum {
    APP_OK = 0,
    APP_ERR_INVALID_DIR,
    APP_DSP_ERR,
} app_err;

bool validate_outdir(const char* outdir) {
    struct stat st;
    // check its a dir
    if (stat(outdir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            return false;
        }
    }
    // dir is not writable
    if (access(outdir, W_OK) != 0) {
        return false;
    }

    return true;
}

void cleanup(ftable** wavtbs, ft_sinesum_args** args, int n) {
    for (int i = 0; i < n; i++) {
        free(wavtbs[i]->buf);
        free(wavtbs[i]);
        free(args[i]);
    }
}

app_err entrypoint(const char* outdir) {
    if (!validate_outdir(outdir)) {
        return APP_ERR_INVALID_DIR;
    }
    // create saw amps .. max harms = 64
    // we work our way down in pow2 to set up the band limited deck
    // for this exercise we create 7 bands. This is the simplest and most practical
    // setup for a bandlimited deck.
    uint32_t amps_sz = 64;
    float amps[64] = {0};

    // fill with saw amps
    saw_amps(amps, amps_sz);

    // 7 is our magic number for this app
    uint32_t bands[7] = {64, 32, 16, 8, 4, 2, 1};

    ft_sinesum_args* args[7] = {0};

    for (int i = 0; i < 7; i++) {
        ft_sinesum_args* a = (ft_sinesum_args*) malloc(sizeof(ft_sinesum_args));
        ft_sinesum_args_init(a, (const float*) &amps, amps_sz, 0.0, true, bands[i]);
        args[i] = a;
    }

    // create the ftable deck
    ftable* wavtbs[7] = {0};
    for (int i = 0; i < 7; i++) {
        ftable* wt = (ftable*) malloc(sizeof(ftable));
        float* buf = (float*) malloc(sizeof(float) * ftable_BUF_SZ);
        ftable_init(wt, buf, ftable_BUF_SZ);
        wavtbs[i] = wt;
    }

    dsp_err err;
    if ((err = sinesum_deck_generate(wavtbs, args, 7, TARGET_SAMPLERATE)) != DSP_OK) {
        cleanup(wavtbs, args, 7);
        return APP_DSP_ERR;
    }

    for (int i = 0; i < 7; i++) {
        printf("band: %d, f0: %.5f sr: %.1f nharms: %.1f\n", i, wavtbs[i]->f0,
               wavtbs[i]->sr, wavtbs[i]->nharms);

        // for (int j = 0; j < ftable_BUF_SZ; j++) {
        //     printf("%f\n", wavtbs[i]->buf[j]);
        // }
        // build outpath
        char path[256];
        snprintf(path, sizeof(path), "%s%sblsaw%d.wav", outdir,
                 (outdir[strlen(outdir) - 1] == '/') ? "" : "/", i);

        wavio w;
        wavio_open_write(&w, malloc, path, wavtbs[i]->nharms,
                         SF_FORMAT_WAV | SF_FORMAT_FLOAT, 1, ftable_BUF_SZ);
        wavio_fill_block(&w, wavtbs[i]->buf);
        wavio_write_block(&w);
        wavio_close(&w, free);
    }

    // cleanup and return ok
    cleanup(wavtbs, args, 7);
    return APP_OK;
}

int main(int argc, char** argv) {

    cli_command* c = cli_command_new();

    cli_err err;
    const char* desc =
        "A test driver that generates a band limited saw deck from summed sinusoids "
        "for analysis.\n"
        "Bands are written directly to outdir as sawb1.wav sawb2.wav sawb3.wav etc.\n";
    const char* usage = "[Options]... outdir";

    if ((err = cli_init(c, desc, usage, argc, (char**) argv)) != CLI_OK) {
        exit_app_cli_err(c, err);
    }

    char outdir[64] = "";
    if ((err = cli_add_str_argument(c, outdir, 32))) {
        exit_app_cli_err(c, err);
    }

    if ((err = cli_parse(c)) != CLI_OK) {
        exit_app_cli_err(c, err);
    }

    app_err aerr;
    if ((aerr = entrypoint(outdir)) != APP_OK) {
        fprintf(stderr, "error: %d\n", aerr);
    }

    return EXIT_SUCCESS;
}
