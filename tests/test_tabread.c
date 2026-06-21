#include "test_tabread.h"

#include <assert.h>
#include <dsp/tabread.h>
#include "helpers.h"

#define NSMPS 64
#define BUF_SZ 66

typedef void (*fill_fn)(float*, uint32_t);

static inline void set_ftable_fixture(float* buf, uint32_t buf_sz, fill_fn fn) {
    // fill a cosine and set the guardpoints
    fn(buf, buf_sz - 2);
    buf[buf_sz - 2] = buf[0];
    buf[buf_sz - 1] = buf[1];
}

static inline void set_deck_fixture(matrix* deck,
                                    float* buf1,
                                    float* buf2,
                                    uint32_t buf_sz,
                                    fill_fn fn1,
                                    fill_fn fn2) {

    set_ftable_fixture(buf1, buf_sz, fn1);
    set_ftable_fixture(buf2, buf_sz, fn2);

    matrix_set_row(deck, 0, buf1, buf_sz);
    matrix_set_row(deck, 1, buf2, buf_sz);
}

MunitResult test_tabread_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf[BUF_SZ] = {0.0};
    set_ftable_fixture(buf, BUF_SZ, fill_cos);

    tabread tb;
    tabread_init(&tb, buf, BUF_SZ);

    return MUNIT_OK;
}

MunitResult test_tabreadn_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf[BUF_SZ] = {0.0};
    set_ftable_fixture(buf, BUF_SZ, fill_cos);

    tabread tb;
    tabread_init(&tb, buf, BUF_SZ);

    float out[NSMPS] = {0.0};
    float idx[NSMPS] = {0.0};
    fill_line(idx, 0, BUF_SZ - 2, NSMPS);

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
    set_ftable_fixture(buf, BUF_SZ, fill_cos);

    tabread tb;
    tabread_init(&tb, buf, BUF_SZ);

    float out[NSMPS] = {0.0};
    float idx[NSMPS] = {0.0};
    fill_line(idx, 0, BUF_SZ - 2, NSMPS);

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
    set_ftable_fixture(buf, BUF_SZ, fill_cos);

    tabread tb;
    tabread_init(&tb, buf, BUF_SZ);

    float out[NSMPS] = {0.0};
    float idx[NSMPS] = {0.0};
    fill_line(idx, 0, BUF_SZ - 2, NSMPS);

    tabread3_tick_block(&tb, out, idx, 0, NSMPS);

    /// TODO worth asserting exact
    check_range_inclusive(out, -1.0, 1.0, NSMPS);
    check_any_nonzero(out, NSMPS);
    return MUNIT_OK;
}

MunitResult test_xtabread_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf1[BUF_SZ] = {0.0};
    float buf2[BUF_SZ] = {0.0};

    float d_buf[2 * BUF_SZ] = {0};
    matrix deck;
    matrix_init(&deck, d_buf, 2, BUF_SZ);

    set_deck_fixture(&deck, buf1, buf2, BUF_SZ, fill_cos, fill_sin);

    tabread l;
    tabread_init(&l, buf1, BUF_SZ);
    tabread r;
    tabread_init(&r, buf2, BUF_SZ);

    xtabread xtb;

    xtabread_init(&xtb, &deck, &l, &r, 0.5);

    return MUNIT_OK;
}

MunitResult test_xtabreadi_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;
    float buf1[BUF_SZ] = {0.0};
    float buf2[BUF_SZ] = {0.0};

    float d_buf[2 * BUF_SZ] = {0};
    matrix deck;
    matrix_init(&deck, d_buf, 2, BUF_SZ);
    set_deck_fixture(&deck, buf1, buf2, BUF_SZ, fill_cos, fill_sin);

    tabread l;
    tabread_init(&l, buf1, BUF_SZ);
    tabread r;
    tabread_init(&r, buf2, BUF_SZ);
    xtabread xtb;

    xtabread_init(&xtb, &deck, &l, &r, 0.5);

    float out[NSMPS] = {0.0};
    float idx[NSMPS] = {0.0};
    fill_line(idx, 0, BUF_SZ - 2, NSMPS);

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

    float buf1[BUF_SZ] = {0};
    float buf2[BUF_SZ] = {0};

    float d_buf[2 * BUF_SZ] = {0};
    matrix deck;
    matrix_init(&deck, d_buf, 2, BUF_SZ);

    set_deck_fixture(&deck, buf1, buf2, BUF_SZ, fill_cos, fill_sin);

    tabread l;
    tabread_init(&l, buf1, BUF_SZ);
    tabread r;
    tabread_init(&r, buf2, BUF_SZ);
    xtabread xtb;

    xtabread_init(&xtb, &deck, &l, &r, 0.5);

    float out[NSMPS] = {0.0};
    float idx[NSMPS] = {0.0};
    fill_line(idx, 0, BUF_SZ - 2, NSMPS);

    float pos[NSMPS] = {0.0};
    fill_line(pos, 0.0, 1.0, NSMPS);

    xtabread3_tick_block(&xtb, out, idx, pos, 0, NSMPS);

    check_range_inclusive(out, -1.0, 1.0, NSMPS);
    check_any_nonzero(out, NSMPS);
    return MUNIT_OK;
}
