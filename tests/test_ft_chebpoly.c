#include "test_ft_chebpoly.h"
#include <dsp/ftable.h>

#define BUF_SZ 64

// TODO: these are more or less smoke tests for now
// needs more testing and csound plugs

MunitResult test_chebpoly(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf[BUF_SZ] = {0.0};
    ftable wt;
    ftable_init(&wt, buf, BUF_SZ);

    // build a ramp
    // TODO: split this into its own test

    for (int i = 0; i < BUF_SZ; i++)
        buf[i] = 1.0;

    float h[4] = {0.0, 0.0, 0.0, 1.0};

    ft_chebpoly_args cheby_args;
    ft_chebpoly_args_init(&cheby_args, h, 4);

    dsp_err err;
    if ((err = ft_chebpoly(&wt, &cheby_args)) != DSP_OK) {
        return MUNIT_ERROR;
    }

    return MUNIT_OK;
}

MunitResult test_chebpoly_deck_generate(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf1[BUF_SZ] = {0.0};
    ftable wt1;
    ftable_init(&wt1, buf1, BUF_SZ);

    float buf2[BUF_SZ] = {0.0};
    ftable wt2;
    ftable_init(&wt2, buf2, BUF_SZ);

    for (int i = 0; i < BUF_SZ; i++) {
        buf1[i] = buf2[i] = 1.0;
    }

    float h1[4] = {0.0, 0.0, 0.0, 1.0};
    float h2[5] = {0.0, 0.0, 0.0, 0.0, 1.0};

    ft_chebpoly_args cheby_args1;
    ft_chebpoly_args_init(&cheby_args1, h1, 4);

    ft_chebpoly_args cheby_args2;
    ft_chebpoly_args_init(&cheby_args2, h2, 5);

    ftable* tabs[2];
    tabs[0] = &wt1;
    tabs[1] = &wt2;

    ft_chebpoly_args* args[2];
    args[0] = &cheby_args1;
    args[1] = &cheby_args2;

    ft_deck deck;
    ft_deck_init(&deck, tabs, 2);

    dsp_err err;
    if ((err = ft_deck_chebpoly_generate(&deck, args, 2)) != DSP_OK) {
        return MUNIT_ERROR;
    }

    return MUNIT_OK;
}
