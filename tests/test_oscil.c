

#include "test_oscil.h"
#include <dsp/oscil.h>
#include "helpers.h"

#include <assert.h>

#define NSMPS 64
#define BUF_SZ 256 + 2

typedef void (*fill_fn)(float*, uint32_t);

static inline void set_ftable_fixture(ftable* ft,
                                      float* buf,
                                      uint32_t buf_sz,
                                      fill_fn fn) {
    // fill a cosine and set the guardpoints
    fn(buf, buf_sz - 2);
    buf[buf_sz - 2] = buf[0];
    buf[buf_sz - 1] = buf[1];
    ftable_init(ft, buf, buf_sz);
}

static inline void set_deck_fixture(wt_deck* deck,
                                    ftable** frames,
                                    uint32_t frames_sz,
                                    ftable* ft1,
                                    ftable* ft2,
                                    float* buf1,
                                    float* buf2,
                                    uint32_t buf_sz,
                                    fill_fn fn1,
                                    fill_fn fn2,
                                    float sr) {

    assert(frames_sz == 2);

    set_ftable_fixture(ft1, buf1, buf_sz, fn1);
    set_ftable_fixture(ft2, buf2, buf_sz, fn2);

    frames[0] = ft1;
    ft1->f0 = 100.0;
    ft1->sr = sr;
    ft1->nharms = 1.0;

    frames[1] = ft2;
    ft2->f0 = 200.0;
    ft2->sr = sr;
    ft2->nharms = 1.0;

    wt_deck_init(deck, frames, frames_sz);
}

MunitResult test_osciln_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf[BUF_SZ] = {0.0};
    ftable ft;
    set_ftable_fixture(&ft, buf, BUF_SZ, fill_cos);
    ftable_init(&ft, buf, BUF_SZ);

    float sr = 48000;

    oscil osc;
    dsp_err err;
    err = oscil_init(&osc, &ft, 120.0, 0.0, sr);
    munit_assert_int(err, ==, 0);

    float out[NSMPS] = {0.0};
    float freq[NSMPS] = {0.0};

    fill_dc(freq, 440.0, NSMPS);

    osciln_tick_block(&osc, out, freq, NSMPS);

    check_range_inclusive(out, -1.0, 1.0, NSMPS);
    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}

MunitResult test_oscili_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf[BUF_SZ] = {0.0};
    ftable ft;
    set_ftable_fixture(&ft, buf, BUF_SZ, fill_cos);
    ftable_init(&ft, buf, BUF_SZ);

    float sr = 48000;

    oscil osc;
    dsp_err err;
    err = oscil_init(&osc, &ft, 120.0, 0.0, sr);
    munit_assert_int(err, ==, 0);

    float out[NSMPS] = {0.0};
    float freq[NSMPS] = {0.0};

    fill_dc(freq, 440.0, NSMPS);

    oscili_tick_block(&osc, out, freq, NSMPS);

    check_range_inclusive(out, -1.0, 1.0, NSMPS);
    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}

MunitResult test_oscil3_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf[BUF_SZ] = {0.0};
    ftable ft;
    set_ftable_fixture(&ft, buf, BUF_SZ, fill_cos);
    ftable_init(&ft, buf, BUF_SZ);

    float sr = 48000;

    oscil osc;
    dsp_err err;
    err = oscil_init(&osc, &ft, 120.0, 0.0, sr);
    munit_assert_int(err, ==, 0);

    float out[NSMPS] = {0.0};
    float freq[NSMPS] = {0.0};

    fill_dc(freq, 440.0, NSMPS);

    oscil3_tick_block(&osc, out, freq, NSMPS);

    check_range_inclusive(out, -1.0, 1.0, NSMPS);
    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}

MunitResult test_oscil3_pm_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf[BUF_SZ] = {0.0};
    ftable ft;
    set_ftable_fixture(&ft, buf, BUF_SZ, fill_cos);
    ftable_init(&ft, buf, BUF_SZ);

    float sr = 48000;

    oscil osc;
    dsp_err err;
    err = oscil_init(&osc, &ft, 120.0, 0.0, sr);
    munit_assert_int(err, ==, 0);

    float out[NSMPS] = {0.0};
    float freq[NSMPS] = {0.0};
    float phs[NSMPS] = {0.0};

    fill_dc(freq, 440.0, NSMPS);
    fill_line(phs, 0.0, 1.0, NSMPS);

    oscil3_pm_tick_block(&osc, out, freq, phs, NSMPS);

    check_range_inclusive(out, -1.0, 1.0, NSMPS);
    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}

MunitResult test_blxoscili_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    ftable ft1;
    float buf1[BUF_SZ] = {0.0};

    ftable ft2;
    float buf2[BUF_SZ] = {0.0};

    ftable* frames[2];

    float sr = 48000;
    wt_deck deck;
    set_deck_fixture(&deck, frames, 2, &ft1, &ft2, buf1, buf2, BUF_SZ, fill_cos,
                     fill_sin, sr);

    // TODO: this should really be refactored...
    // 1. Either bl/xoscil should own their oscils or the references should be
    // fully initialized by their updates .. this partial just feels like noise.
    dsp_err err;
    oscil l;
    err = oscil_init(&l, deck.frames[0], 440.0, 0.0, sr);

    oscil r;
    err = oscil_init(&r, deck.frames[1], 440.0, 0.0, sr);

    blxoscil osc;

    err = blxoscil_init(&osc, &deck, &l, &r, 120.0, 0.0);
    munit_assert_int(err, ==, 0);

    float out[NSMPS] = {0.0};
    float freq[NSMPS] = {0.0};

    // this line should be enough to cross the positional threshold for the xfade
    fill_line(freq, 100.0, 300.0, NSMPS);
    blxoscili_tick_block(&osc, out, freq, NSMPS);

    check_range_inclusive(out, -1.0, 1.0, NSMPS);
    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}

MunitResult test_blxoscil3_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    ftable ft1;
    float buf1[BUF_SZ] = {0.0};

    ftable ft2;
    float buf2[BUF_SZ] = {0.0};

    ftable* frames[2];

    float sr = 48000;
    wt_deck deck;
    set_deck_fixture(&deck, frames, 2, &ft1, &ft2, buf1, buf2, BUF_SZ, fill_cos,
                     fill_sin, sr);

    // TODO: this should really be refactored...
    // 1. Either bl/xoscil should own their oscils or the references should be
    // fully initialized by their updates .. this partial just feels like noise.
    dsp_err err;
    oscil l;
    err = oscil_init(&l, deck.frames[0], 440.0, 0.0, sr);

    oscil r;
    err = oscil_init(&r, deck.frames[1], 440.0, 0.0, sr);

    blxoscil osc;

    err = blxoscil_init(&osc, &deck, &l, &r, 120.0, 0.0);
    munit_assert_int(err, ==, 0);

    float out[NSMPS] = {0.0};
    float freq[NSMPS] = {0.0};

    // this line should be enough to cross the positional threshold for the xfade
    fill_line(freq, 100.0, 300.0, NSMPS);

    blxoscil3_tick_block(&osc, out, freq, NSMPS);

    check_range_inclusive(out, -1.0, 1.0, NSMPS);
    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}

MunitResult test_xoscili_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    ftable ft1;
    float buf1[BUF_SZ] = {0.0};

    ftable ft2;
    float buf2[BUF_SZ] = {0.0};

    ftable* frames[2];

    float sr = 48000;
    wt_deck deck;
    set_deck_fixture(&deck, frames, 2, &ft1, &ft2, buf1, buf2, BUF_SZ, fill_cos,
                     fill_sin, sr);

    // TODO: this should really be refactored...
    // 1. Either bl/xoscil should own their oscils or the references should be
    // fully initialized by their updates .. this partial just feels like noise.
    dsp_err err;
    oscil l;
    err = oscil_init(&l, deck.frames[0], 440.0, 0.0, sr);

    oscil r;
    err = oscil_init(&r, deck.frames[1], 440.0, 0.0, sr);

    xoscil osc;

    err = xoscil_init(&osc, &deck, &l, &r, 120.0, 0.0, 0.0);
    munit_assert_int(err, ==, 0);

    float out[NSMPS] = {0.0};
    float freq[NSMPS] = {0.0};
    float pos[NSMPS] = {0.0};

    fill_line(freq, 100.0, 300.0, NSMPS);
    fill_line(pos, 0.0, 1.0, NSMPS);

    xoscili_tick_block(&osc, out, freq, pos, NSMPS);

    check_range_inclusive(out, -1.0, 1.0, NSMPS);

    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}

MunitResult test_xoscil3_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    ftable ft1;
    float buf1[BUF_SZ] = {0.0};

    ftable ft2;
    float buf2[BUF_SZ] = {0.0};

    ftable* frames[2];

    float sr = 48000;
    wt_deck deck;
    set_deck_fixture(&deck, frames, 2, &ft1, &ft2, buf1, buf2, BUF_SZ, fill_cos,
                     fill_sin, sr);

    // TODO: this should really be refactored...
    // 1. Either bl/xoscil should own their oscils or the references should be
    // fully initialized by their updates .. this partial just feels like noise.
    dsp_err err;
    oscil l;
    err = oscil_init(&l, deck.frames[0], 440.0, 0.0, sr);

    oscil r;
    err = oscil_init(&r, deck.frames[1], 440.0, 0.0, sr);

    xoscil osc;

    err = xoscil_init(&osc, &deck, &l, &r, 120.0, 0.0, 0.0);
    munit_assert_int(err, ==, 0);

    float out[NSMPS] = {0.0};
    float freq[NSMPS] = {0.0};
    float pos[NSMPS] = {0.0};

    fill_line(freq, 100.0, 300.0, NSMPS);
    fill_line(pos, 0.0, 1.0, NSMPS);

    xoscil3_tick_block(&osc, out, freq, pos, NSMPS);

    check_range_inclusive(out, -1.0, 1.0, NSMPS);

    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}
