
#include <stdint.h>

#include <dsp/maths.h>
#include <dsp/oscil.h>
#include <dsp/utils.h>
#include <dsp/wavetable/deck.h>
#include <dsp/wavetable/ramp.h>
#include <dsp/wavetable/sinesum.h>
#include <dsp/wavetable/wavetable.h>

#include "common.h"
#include "dsp/interpolate.h"
#include "wavio.h"

#define TARGET_SAMPLERATE 48000.0f
#define WAVETABLE_BUF_SZ 8194
#define INTERP_FRAME_SZ 256
#define AMPS_SZ 64

typedef enum {
    APP_OK = 0,
    APP_ERR_INVALID_PATH,
    APP_DSP_ERR,
} app_err;

void cleanup(wavetable** wavtbs, int n) {
    for (int i = 0; i < n; i++) {
        free(wavtbs[i]->buf);
        free(wavtbs[i]);
    }
}

app_err entrypoint(const char* outfile) {
    if (!validate_path(outfile, ".wav")) {
        return APP_ERR_INVALID_PATH;
    }

    uint32_t amps_sz = AMPS_SZ;
    float amps_saw[AMPS_SZ] = {0};
    float amps_sqr[AMPS_SZ] = {0};
    float amps_tri[AMPS_SZ] = {0};

    // fill with amps. 0=saw, 1=sqr, 2=tri
    saw_amps(amps_saw, amps_sz);
    sqr_amps(amps_sqr, amps_sz);
    tri_amps(amps_tri, amps_sz);

    wt_sinesum_args* args[3] = {0};

    args[0] = (wt_sinesum_args*) malloc(sizeof(wt_sinesum_args));
    args[1] = (wt_sinesum_args*) malloc(sizeof(wt_sinesum_args));
    args[2] = (wt_sinesum_args*) malloc(sizeof(wt_sinesum_args));

    wt_sinesum_args_init(args[0], amps_tri, amps_sz, 0.0, true, amps_sz);
    wt_sinesum_args_init(args[1], amps_saw, amps_sz, 0.0, true, amps_sz);
    wt_sinesum_args_init(args[2], amps_sqr, amps_sz, 0.0, true, amps_sz);

    // create the initial wavetable deck
    wavetable* initial_wavtabs[3] = {0};
    for (int i = 0; i < 3; i++) {
        wavetable* wt = (wavetable*) malloc(sizeof(wavetable));
        float* buf = (float*) malloc(sizeof(float) * WAVETABLE_BUF_SZ);
        memset(buf, 0, sizeof(float) * WAVETABLE_BUF_SZ);
        wavetable_init(wt, buf, WAVETABLE_BUF_SZ);
        initial_wavtabs[i] = wt;
    }

    // generate the initial waveforms
    dsp_err err;
    if ((err = sinesum_deck_generate(initial_wavtabs, args, 3, TARGET_SAMPLERATE)) !=
        DSP_OK) {
        cleanup(initial_wavtabs, 3);
        for (int i = 0; i < 3; i++) {
            free(initial_wavtabs[i]->buf);
            free(args[i]);
        }

        return APP_DSP_ERR;
    }

    // we initialize this deck so we can use the matrix fill method
    wt_deck initial_deck;
    wt_deck_init(&initial_deck, initial_wavtabs, 3);

    // we need alot of memory to facilitate the wavetable interp dance.

    // m - original matrix from wavtabs shape=(3, WAVETABLE_BUF_SZ)
    matrix m;
    float* m_data = (float*) malloc(sizeof(float) * 3 * WAVETABLE_BUF_SZ);
    memset(m_data, 0, sizeof(float) * 3 * WAVETABLE_BUF_SZ);
    matrix_init(&m, m_data, 3, WAVETABLE_BUF_SZ);

    // fill the matrix from the existing deck
    wt_deck_matrix_fill(&initial_deck, &m);

    // t - transpose matrix  shape=(WAVETABLE_BUF_SZ, 3)
    matrix t;
    float* t_data = (float*) malloc(sizeof(float) * 3 * WAVETABLE_BUF_SZ);
    memset(t_data, 0, sizeof(float) * 3 * WAVETABLE_BUF_SZ);
    matrix_init(&t, t_data, WAVETABLE_BUF_SZ, 3);

    // initial transpose
    matrix_transpose(&t, &m);

    // tx - extended transpose matrix shape=(WAVETABLE_BUF_SZ, INTERP_FRAME_SZ)
    matrix tx;
    float* tx_data =
        (float*) malloc(sizeof(float) * WAVETABLE_BUF_SZ * INTERP_FRAME_SZ);
    memset(tx_data, 0, sizeof(float) * WAVETABLE_BUF_SZ * INTERP_FRAME_SZ);
    matrix_init(&tx, tx_data, WAVETABLE_BUF_SZ, INTERP_FRAME_SZ);

    // iter over each row of the transpose and project cols from in to out
    for (size_t row = 0; row < tx.n_rows; row++) {
        float* in_row_ptr = matrix_get_row(&t, row);
        float* out_row_ptr = matrix_get_row(&tx, row);
        npinterp(out_row_ptr, tx.n_cols, in_row_ptr, t.n_cols);
    }

    // u - final matrix shape=(INTERP_FRAME_SZ, WAVETABLE_BUF_SZ)
    matrix u;
    float* u_data = (float*) malloc(sizeof(float) * INTERP_FRAME_SZ * WAVETABLE_BUF_SZ);
    memset(u_data, 0, sizeof(float) * WAVETABLE_BUF_SZ * INTERP_FRAME_SZ);
    matrix_init(&u, u_data, INTERP_FRAME_SZ, WAVETABLE_BUF_SZ);

    // final transpose back to the original
    matrix_transpose(&u, &tx);

    // new wavtabs - these are the bois that go to xoscil
    // we write the data into the new buffer so we can free all the extra matrix memory.
    wavetable* wavtabs[INTERP_FRAME_SZ];
    for (int i = 0; i < INTERP_FRAME_SZ; i++) {
        wavetable* wt = (wavetable*) malloc(sizeof(wavetable));
        float* buf = (float*) malloc(sizeof(float) * WAVETABLE_BUF_SZ);
        memset(buf, 0, sizeof(float) * WAVETABLE_BUF_SZ);

        wavetable_init(wt, buf, WAVETABLE_BUF_SZ);
        float* row_ptr = matrix_get_row(&u, i);
        wavetable_write(wt, row_ptr, WAVETABLE_BUF_SZ, 0);
        wavtabs[i] = wt;
    }

    // free initial memory
    free(u_data);
    free(tx_data);
    free(t_data);
    free(m_data);
    cleanup(initial_deck.frames, 3);

    // make a stupid line to test the position scroller
    uint32_t nsmps = (uint32_t) (TARGET_SAMPLERATE * 10.f);
    uint32_t lintab_buf_sz = nsmps + 2;

    float* lintab_buf = (float*) malloc(sizeof(float) * lintab_buf_sz);
    memset(lintab_buf, 0, sizeof(float) * lintab_buf_sz);
    wavetable lintab;
    wavetable_init(&lintab, lintab_buf, lintab_buf_sz);

    // pos ramp between 0 - 1
    wt_ramp_args ramp_args = {
        .start = 0.0,
        .stop = 1.0,
        .endpoint = true,
    };
    // nsmps long linear ramp
    if ((err = wt_linspace(&lintab, &ramp_args)) != DSP_OK) {
        return APP_DSP_ERR;
    }

    // setup xoscil
    // then add the deck wrapper.
    oscil left;
    oscil right;

    // xoscil deck
    wt_deck deck;
    wt_deck_init(&deck, wavtabs, INTERP_FRAME_SZ);

    float freq_ = 120.f;

    if ((err = oscil_init(&left, wavtabs[0], freq_, 0.0, TARGET_SAMPLERATE)) !=
        DSP_OK) {
        fprintf(stderr, "oscil_init: left\n");
        return APP_DSP_ERR;
    }

    if ((err = oscil_init(&right, wavtabs[0], freq_, 0.0, TARGET_SAMPLERATE)) !=
        DSP_OK) {
        fprintf(stderr, "oscil_init: right\n");
        return APP_DSP_ERR;
    }

    xoscil morph;
    xoscil_init(&morph, &deck, &left, &right, freq_, 0.0, 0.0);

    // //  tick the out block
    float* out = (float*) malloc(sizeof(float) * nsmps);
    memset(out, 0, sizeof(float) * nsmps);

    float* freq = (float*) malloc(sizeof(float) * nsmps);
    memset(freq, 0, sizeof(float) * nsmps);

    for (uint32_t i = 0; i < nsmps; i++)
        freq[i] = freq_;

    xoscil3_tick_block(&morph, out, freq, lintab.buf, nsmps);

    // oscil3_tick_block(&left, out, freq, nsmps);

    wavio w;
    wavio_open_write(&w, malloc, outfile, SF_FORMAT_WAV | SF_FORMAT_FLOAT,
                     TARGET_SAMPLERATE, 1, nsmps);
    wavio_fill_block(&w, out);
    wavio_write_block(&w);
    wavio_close(&w, free);

    // // // cleanup

    cleanup(deck.frames, INTERP_FRAME_SZ);

    // // free the extra stuff
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
    const char* desc = "Morph between a saw and triangle wave using wt interpolation.";
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
