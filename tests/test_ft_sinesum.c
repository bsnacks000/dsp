

#include "test_ft_sinesum.h"

#include <dsp/ftable/sinesum.h>

#define AMPS_SZ 64
#define BUF_SZ 130

MunitResult test_sinesum(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float amps[AMPS_SZ] = {0.0};

    sqr_amps(amps, AMPS_SZ);

    float buf[BUF_SZ] = {0.0};

    ftable wt;
    ftable_init(&wt, buf, BUF_SZ);

    ft_sinesum_args args = {
        .amps = amps,
        .amps_sz = AMPS_SZ,
        .phase = 0.0,
        .smooth = true,
        .nharms = AMPS_SZ,
    };

    dsp_err err = ftable_func(&wt, ft_sinesum, (void*) &args);

    munit_assert_int(err, ==, 0);

    return MUNIT_OK;
}

MunitResult test_sinesum1(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float amps[AMPS_SZ] = {0.0};

    sqr_amps(amps, AMPS_SZ);

    float buf[BUF_SZ] = {0.0};

    ftable wt;
    ftable_init(&wt, buf, BUF_SZ);

    ft_sinesum_args args = {
        .amps = amps,
        .amps_sz = AMPS_SZ,
        .phase = 0.0,
        .smooth = true,
        .nharms = AMPS_SZ,
    };

    dsp_err err = ftable_func(&wt, ft_sinesum1, (void*) &args);
    munit_assert_int(err, ==, 0);

    return MUNIT_OK;
}

static inline void cleanup(ftable** wavtbs, ft_sinesum_args** args, int n) {
    for (int i = 0; i < n; i++) {
        free(wavtbs[i]->buf);
        free(wavtbs[i]);
        free(args[i]);
    }
}

MunitResult test_sinesum_deck_generate(const MunitParameter* params, void* data) {
    (void) params;
    (void) data;

    float amps_saw[AMPS_SZ] = {0};
    float amps_sqr[AMPS_SZ] = {0};
    float amps_tri[AMPS_SZ] = {0};

    // fill with amps. 0=saw, 1=sqr, 2=tri
    saw_amps(amps_saw, AMPS_SZ);
    sqr_amps(amps_sqr, AMPS_SZ);
    tri_amps(amps_tri, AMPS_SZ);

    ft_sinesum_args* args[3] = {0};

    ft_sinesum_args saw_args;
    ft_sinesum_args sqr_args;
    ft_sinesum_args tri_args;

    args[0] = &saw_args;
    args[1] = &sqr_args;
    args[2] = &tri_args;

    ft_sinesum_args_init(args[0], amps_saw, AMPS_SZ, 0.0, true, AMPS_SZ);
    ft_sinesum_args_init(args[1], amps_sqr, AMPS_SZ, 0.0, true, AMPS_SZ);
    ft_sinesum_args_init(args[2], amps_tri, AMPS_SZ, 0.0, true, AMPS_SZ);

    // create the ftable deck
    ftable* wavtabs[3] = {0};
    for (int i = 0; i < 3; i++) {
        ftable* wt = (ftable*) malloc(sizeof(ftable));
        float* buf = (float*) malloc(sizeof(float) * BUF_SZ);
        memset(buf, 0, sizeof(float) * BUF_SZ);
        ftable_init(wt, buf, BUF_SZ);
        wavtabs[i] = wt;
    }

    float sr = 48000;

    dsp_err err;
    if ((err = sinesum_deck_generate(wavtabs, args, 3, sr)) != DSP_OK) {
        cleanup(wavtabs, args, 3);
        return MUNIT_ERROR;
    }

    return MUNIT_OK;
}
