#include "test_tabread.h"

#include <assert.h>
#include <dsp/ftable.h>
#include <dsp/tabread.h>
#include "helpers.h"

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

static inline void set_deck_fixture(ft_deck* deck,
                                    ftable** frames,
                                    uint32_t frames_sz,
                                    ftable* ft1,
                                    ftable* ft2,
                                    float* buf1,
                                    float* buf2,
                                    uint32_t buf_sz,
                                    fill_fn fn1,
                                    fill_fn fn2) {

    assert(frames_sz == 2);

    set_ftable_fixture(ft1, buf1, buf_sz, fn1);
    set_ftable_fixture(ft2, buf2, buf_sz, fn2);

    frames[0] = ft1;
    frames[1] = ft2;

    ft_deck_init(deck, frames, frames_sz);
}

MunitResult test_tabread_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf[BUF_SZ] = {0.0};
    ftable ft;
    set_ftable_fixture(&ft, buf, BUF_SZ, fill_cos);

    ftable_init(&ft, buf, BUF_SZ);

    tabread tb;
    tabread_init(&tb, &ft);

    return MUNIT_OK;
}
MunitResult test_tabreadn_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf[BUF_SZ] = {0.0};
    ftable ft;
    set_ftable_fixture(&ft, buf, BUF_SZ, fill_cos);

    ftable_init(&ft, buf, BUF_SZ);

    tabread tb;
    tabread_init(&tb, &ft);

    float out[NSMPS] = {0.0};
    float idx[NSMPS] = {0.0};
    fill_line(idx, 0, ft.len, NSMPS);

    tabreadn_tick_block(&tb, out, idx, 0, NSMPS);

    /// TODO worth asserting exact
    check_range_inclusive(out, -1.0, 1.0, NSMPS);
    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}
MunitResult test_tabreadi_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf[BUF_SZ] = {0.0};
    ftable ft;
    set_ftable_fixture(&ft, buf, BUF_SZ, fill_cos);

    ftable_init(&ft, buf, BUF_SZ);

    tabread tb;
    tabread_init(&tb, &ft);

    float out[NSMPS] = {0.0};
    float idx[NSMPS] = {0.0};
    fill_line(idx, 0, ft.len, NSMPS);

    tabreadi_tick_block(&tb, out, idx, 0, NSMPS);

    /// TODO worth asserting exact
    check_range_inclusive(out, -1.0, 1.0, NSMPS);
    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}
MunitResult test_tabread3_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf[BUF_SZ] = {0.0};
    ftable ft;
    set_ftable_fixture(&ft, buf, BUF_SZ, fill_cos);

    ftable_init(&ft, buf, BUF_SZ);

    tabread tb;
    tabread_init(&tb, &ft);

    float out[NSMPS] = {0.0};
    float idx[NSMPS] = {0.0};
    fill_line(idx, 0, ft.len, NSMPS);

    tabread3_tick_block(&tb, out, idx, 0, NSMPS);

    check_range_inclusive(out, -1.0, 1.0, NSMPS);
    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}
MunitResult test_xtabread_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    ftable ft1;
    float buf1[BUF_SZ] = {0.0};

    ftable ft2;
    float buf2[BUF_SZ] = {0.0};

    ftable* frames[2];

    ft_deck deck;

    set_deck_fixture(&deck, frames, 2, &ft1, &ft2, buf1, buf2, BUF_SZ, fill_cos,
                     fill_sin);

    tabread l;
    tabread r;
    xtabread xtb;

    xtabread_init(&xtb, &deck, &l, &r, 0.5);

    return MUNIT_OK;
}
MunitResult test_xtabreadi_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;
    ftable ft1;
    float buf1[BUF_SZ] = {0.0};

    ftable ft2;
    float buf2[BUF_SZ] = {0.0};

    ftable* frames[2];

    ft_deck deck;

    set_deck_fixture(&deck, frames, 2, &ft1, &ft2, buf1, buf2, BUF_SZ, fill_cos,
                     fill_sin);

    tabread l;
    tabread r;
    xtabread xtb;

    xtabread_init(&xtb, &deck, &l, &r, 0.5);

    float out[NSMPS] = {0.0};
    float idx[NSMPS] = {0.0};
    fill_line(idx, 0, ft1.len, NSMPS);

    float pos[NSMPS] = {0.0};
    fill_line(pos, 0.0, 1.0, NSMPS);

    xtabreadi_tick_block(&xtb, out, idx, pos, 0, NSMPS);

    check_range_inclusive(out, -1.0, 1.0, NSMPS);
    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}

MunitResult test_xtabread3_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;
    ftable ft1;
    float buf1[BUF_SZ] = {0.0};

    ftable ft2;
    float buf2[BUF_SZ] = {0.0};

    ftable* frames[2];

    ft_deck deck;

    set_deck_fixture(&deck, frames, 2, &ft1, &ft2, buf1, buf2, BUF_SZ, fill_cos,
                     fill_sin);

    tabread l;
    tabread r;
    xtabread xtb;

    xtabread_init(&xtb, &deck, &l, &r, 0.5);

    float out[NSMPS] = {0.0};
    float idx[NSMPS] = {0.0};
    fill_line(idx, 0, ft1.len, NSMPS);

    float pos[NSMPS] = {0.0};
    fill_line(pos, 0.0, 1.0, NSMPS);

    xtabread3_tick_block(&xtb, out, idx, pos, 0, NSMPS);

    check_range_inclusive(out, -1.0, 1.0, NSMPS);
    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}
