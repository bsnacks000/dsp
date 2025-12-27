#include <dsp/ftable/sinesum.h>
#include <dsp/oscil.h>
#include <stdint.h>
#include "common.h"

#include "dsp/ftable/deck.h"
#include "dsp/ftable/ftable.h"
#include "dsp/ftable/ramp.h"
#include "dsp/utils.h"
#include "wavio.h"

// TODO: while this works its not great...
// need to experiment with interpolating table generator.

#define TARGET_SAMPLERATE 48000.0f
#define ftable_BUF_SZ 8194

typedef enum {
    APP_OK = 0,
    APP_ERR_INVALID_PATH,
    APP_DSP_ERR,
} app_err;

void cleanup(ftable** wavtbs, ft_sinesum_args** args, int n) {
    for (int i = 0; i < n; i++) {
        free(wavtbs[i]->buf);
        free(wavtbs[i]);
        free(args[i]);
    }
}

app_err entrypoint(const char* outfile) {
    if (!validate_path(outfile, ".wav")) {
        return APP_ERR_INVALID_PATH;
    }

    uint32_t amps_sz = 64;
    float amps_saw[64] = {0};
    float amps_sqr[64] = {0};
    float amps_tri[64] = {0};

    // fill with amps. 0=saw, 1=sqr, 2=tri
    saw_amps(amps_saw, amps_sz);
    sqr_amps(amps_sqr, amps_sz);
    tri_amps(amps_tri, amps_sz);

    ft_sinesum_args* args[3] = {0};

    args[0] = (ft_sinesum_args*) malloc(sizeof(ft_sinesum_args));
    args[1] = (ft_sinesum_args*) malloc(sizeof(ft_sinesum_args));
    args[2] = (ft_sinesum_args*) malloc(sizeof(ft_sinesum_args));

    ft_sinesum_args_init(args[0], amps_saw, amps_sz, 0.0, true, amps_sz);
    ft_sinesum_args_init(args[1], amps_sqr, amps_sz, 0.0, true, amps_sz);
    ft_sinesum_args_init(args[2], amps_tri, amps_sz, 0.0, true, amps_sz);

    // create the ftable deck
    ftable* wavtabs[3] = {0};
    for (int i = 0; i < 3; i++) {
        ftable* wt = (ftable*) malloc(sizeof(ftable));
        float* buf = (float*) malloc(sizeof(float) * ftable_BUF_SZ);
        memset(buf, 0, sizeof(float) * ftable_BUF_SZ);
        ftable_init(wt, buf, ftable_BUF_SZ);
        wavtabs[i] = wt;
    }

    dsp_err err;
    if ((err = sinesum_deck_generate(wavtabs, args, 3, TARGET_SAMPLERATE)) != DSP_OK) {
        cleanup(wavtabs, args, 3);
        return APP_DSP_ERR;
    }

    uint32_t nsmps = (uint32_t) (TARGET_SAMPLERATE * 6.f);
    uint32_t lintab_buf_sz = nsmps + 2;

    float* lintab_buf = (float*) malloc(sizeof(float) * lintab_buf_sz);
    memset(lintab_buf, 0, sizeof(float) * lintab_buf_sz);
    ftable lintab;
    ftable_init(&lintab, lintab_buf, lintab_buf_sz);

    // pos ramp between 0 - 1
    ft_ramp_args ramp_args = {
        .start = 0.0,
        .stop = 1.0,
        .endpoint = true,
    };
    // nsmps long linear ramp
    if ((err = ft_linspace(&lintab, &ramp_args)) != DSP_OK) {
        return APP_DSP_ERR;
    }

    // setup xoscil
    // then add the deck wrapper.
    oscil left;
    oscil right;

    if ((err = oscil_init(&left, wavtabs[0], 220.0, 0.0, TARGET_SAMPLERATE)) !=
        DSP_OK) {
        fprintf(stderr, "oscil_init: left\n");
        return APP_DSP_ERR;
    }

    if ((err = oscil_init(&right, wavtabs[0], 220.0, 0.0, TARGET_SAMPLERATE)) !=
        DSP_OK) {
        fprintf(stderr, "oscil_init: right\n");
        return APP_DSP_ERR;
    }

    xoscil morph;
    ft_deck deck;
    ft_deck_init(&deck, wavtabs, 3);

    xoscil_init(&morph, &deck, &left, &right, 220.0, 0.0, 0.0);

    //  tick the out block
    float* out = (float*) malloc(sizeof(float) * nsmps);
    memset(out, 0, sizeof(float) * nsmps);

    float* freq = (float*) malloc(sizeof(float) * nsmps);
    memset(out, 0, sizeof(float) * nsmps);

    for (uint32_t i = 0; i < nsmps; i++)
        freq[i] = 220.0;

    xoscil3_tick_block(&morph, out, freq, lintab.buf, nsmps);

    // for (uint32_t i = 0; i < nsmps; i++) {
    //     fprintf(stderr, "%.6f\n", lintab.buf[i]);
    // }
    wavio w;
    wavio_open_write(&w, malloc, outfile, TARGET_SAMPLERATE,
                     SF_FORMAT_WAV | SF_FORMAT_FLOAT, 1, nsmps);
    wavio_fill_block(&w, out);
    wavio_write_block(&w);
    wavio_close(&w, free);

    // cleanup deck
    cleanup(deck.frames, args, 3);

    // free the extra stuff
    free(out);
    free(freq);
    free(lintab.buf);

    return APP_OK;
}

int main(int argc, char** argv) {
    (void) argc;
    (void) argv;

    cli_command* c = cli_command_new();

    cli_err err;
    const char* desc = "Morph between a saw and triangle wave.";
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

    return EXIT_SUCCESS;
}
