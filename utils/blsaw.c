#include <dsp/ftable/sinesum.h>
#include <dsp/oscil.h>
#include <sndfile.h>
#include "common.h"

#include "dsp/ftable/deck.h"
#include "dsp/ftable/ftable.h"
#include "dsp/ftable/ramp.h"
#include "dsp/utils.h"
#include "wavio.h"

/**
 * target 15kHz .. add +2 for ftable buf sz
 */
#define TARGET_SAMPLERATE 48000.0f
#define ftable_BUF_SZ 8194
#define MAX_FREQ 15000.0f

typedef enum {
    APP_OK = 0,
    APP_ERR_INVALID_PATH,
    APP_DSP_ERR,
} app_err;

void cleanup(ftable** wavtbs, wt_sinesum_args** args, int n) {
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
    float amps[64] = {0};

    // fill with saw amps
    saw_amps(amps, amps_sz);

    // 7 is our magic number for this app
    uint32_t bands[7] = {64, 32, 16, 8, 4, 2, 1};

    wt_sinesum_args* args[7] = {0};

    for (int i = 0; i < 7; i++) {
        wt_sinesum_args* a = (wt_sinesum_args*) malloc(sizeof(wt_sinesum_args));
        wt_sinesum_args_init(a, (const float*) &amps, amps_sz, 0.0, true, bands[i]);
        args[i] = a;
    }

    // create the ftable deck
    ftable* wavtabs[7] = {0};
    for (int i = 0; i < 7; i++) {
        ftable* wt = (ftable*) malloc(sizeof(ftable));
        float* buf = (float*) malloc(sizeof(float) * ftable_BUF_SZ);
        ftable_init(wt, buf, ftable_BUF_SZ);
        wavtabs[i] = wt;
    }

    dsp_err err;
    if ((err = sinesum_deck_generate(wavtabs, args, 7, TARGET_SAMPLERATE)) != DSP_OK) {
        cleanup(wavtabs, args, 7);
        return APP_DSP_ERR;
    }

    // generate bandlimited saw wave with blxoscil
    // use a ftable ramp fill 5 seconds.

    uint32_t nsmps = (uint32_t) TARGET_SAMPLERATE * 5;

    float* lintab_buf = (float*) malloc(sizeof(float) * (nsmps + 2));
    ftable lintab;
    ftable_init(&lintab, lintab_buf, nsmps);

    wt_ramp_args ramp_args = {
        .start = 1.0,
        .stop = MAX_FREQ,
        .endpoint = true,
    };
    // nsmps long linear ramp
    if ((err = wt_linspace(&lintab, &ramp_args)) != DSP_OK) {
        return APP_DSP_ERR;
    }

    // setup blxoscil finally .. first the two backin oscils
    // then add the deck wrapper.
    oscil left;
    oscil right;

    if ((err = oscil_init(&left, wavtabs[0], 1.0, 0.0, TARGET_SAMPLERATE)) != DSP_OK) {
        fprintf(stderr, "oscil_init: left\n");
        return APP_DSP_ERR;
    }

    if ((err = oscil_init(&right, wavtabs[0], 1.0, 0.0, TARGET_SAMPLERATE)) != DSP_OK) {
        fprintf(stderr, "oscil_init: right\n");
        return APP_DSP_ERR;
    }

    // printf("oscil left: %f\n", left.freq);

    blxoscil blsaw;
    wt_deck deck;
    wt_deck_init(&deck, wavtabs, 7);

    blxoscil_init(&blsaw, &deck, &left, &right, 1.0, 0.0);

    // // tick the out block
    float* out = (float*) malloc(sizeof(float) * nsmps);
    blxoscil3_tick_block(&blsaw, out, lintab.buf, nsmps);

    wavio w;
    wavio_open_write(&w, malloc, outfile, TARGET_SAMPLERATE,
                     SF_FORMAT_WAV | SF_FORMAT_FLOAT, 1, nsmps);
    wavio_fill_block(&w, out);
    wavio_write_block(&w);
    wavio_close(&w, free);

    // cleanup deck
    cleanup(deck.frames, args, 7);

    // free the extra stuff
    free(out);
    free(lintab.buf);

    return APP_OK;
}

int main(int argc, char** argv) {
    (void) argc;
    (void) argv;

    cli_command* c = cli_command_new();

    cli_err err;
    const char* desc = "Generate a band limited saw sweep.";
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
