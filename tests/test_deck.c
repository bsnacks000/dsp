
#include "test_deck.h"

#include <dsp/ftable/deck.h>
#include <dsp/ftable/ftable.h>
#include <dsp/maths.h>

#include <stdio.h>

#define BUF_SZ 64

MunitResult test_deck_matrix_fill(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    ftable ft1, ft2;
    float buf1[BUF_SZ] = {0.0};
    float buf2[BUF_SZ] = {0.0};

    ftable_init(&ft1, buf1, BUF_SZ);
    ftable_init(&ft2, buf2, BUF_SZ);

    ftable* frames[2];
    frames[0] = &ft1;
    frames[1] = &ft2;

    wt_deck deck;

    wt_deck_init(&deck, frames, 2);

    size_t n_rows = 2;  // frames_sz;
    size_t n_cols = BUF_SZ;
    float mdat[2 * BUF_SZ] = {0.0};

    matrix m;
    matrix_init(&m, mdat, n_rows, n_cols);
    dsp_err err = wt_deck_matrix_fill(&deck, &m);

    printf("%d\n, %d\n", ft1.buf_sz, ft2.buf_sz);

    munit_assert_int(err, ==, 0);

    return MUNIT_OK;
}

MunitResult test_deck_frames_equal(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    ftable ft1, ft2;
    float buf1[BUF_SZ] = {0.0};
    float buf2[BUF_SZ] = {0.0};

    ftable_init(&ft1, buf1, BUF_SZ);
    ftable_init(&ft2, buf2, BUF_SZ);

    ftable* frames[2];
    frames[0] = &ft1;
    frames[1] = &ft2;

    wt_deck deck;

    wt_deck_init(&deck, frames, 2);

    bool ok = wt_deck_frames_equal(&deck);
    munit_assert_true(ok);

    ft2.buf_sz = 42;
    bool notok = wt_deck_frames_equal(&deck);
    munit_assert_false(notok);

    return MUNIT_OK;
}
