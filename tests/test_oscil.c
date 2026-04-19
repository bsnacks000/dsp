

#include "test_oscil.h"
#include <dsp/oscil.h>
#include "dsp/matrix.h"
#include "helpers.h"

#include <assert.h>

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

MunitResult test_osciln_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf[BUF_SZ] = {0.0};
    set_ftable_fixture(buf, BUF_SZ, fill_cos);

    float sr = 48000;

    oscil osc;
    oscil_init(&osc, buf, BUF_SZ, 120.0, 0.0, sr);

    float out[NSMPS] = {0.0};
    float freq[NSMPS] = {0.0};

    fill_dc(freq, 440.0, NSMPS);

    osciln_tick_block(&osc, out, freq, 0, NSMPS);

    check_range_inclusive(out, -1.0, 1.0, NSMPS);
    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}

MunitResult test_oscili_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf[BUF_SZ] = {0.0};
    set_ftable_fixture(buf, BUF_SZ, fill_cos);

    float sr = 48000;

    oscil osc;
    oscil_init(&osc, buf, BUF_SZ, 120.0, 0.0, sr);

    float out[NSMPS] = {0.0};
    float freq[NSMPS] = {0.0};

    fill_dc(freq, 440.0, NSMPS);

    oscili_tick_block(&osc, out, freq, 0, NSMPS);

    check_range_inclusive(out, -1.0, 1.0, NSMPS);
    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}

MunitResult test_oscil3_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf[BUF_SZ] = {0.0};
    set_ftable_fixture(buf, BUF_SZ, fill_cos);

    float sr = 48000;

    oscil osc;
    oscil_init(&osc, buf, BUF_SZ, 120.0, 0.0, sr);

    float out[NSMPS] = {0.0};
    float freq[NSMPS] = {0.0};

    fill_dc(freq, 440.0, NSMPS);

    oscil3_tick_block(&osc, out, freq, 0, NSMPS);

    check_range_inclusive(out, -1.0, 1.0, NSMPS);
    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}

MunitResult test_oscili_pm_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf[BUF_SZ] = {0.0};
    set_ftable_fixture(buf, BUF_SZ, fill_cos);

    float sr = 48000;

    oscil osc;
    oscil_init(&osc, buf, BUF_SZ, 120.0, 0.0, sr);

    float out[NSMPS] = {0.0};
    float freq[NSMPS] = {0.0};
    float phs[NSMPS] = {0.0};

    fill_dc(freq, 440.0, NSMPS);
    fill_line(phs, 0.0, 1.0, NSMPS);

    oscili_pm_tick_block(&osc, out, freq, phs, 0, NSMPS);

    check_range_inclusive(out, -1.0, 1.0, NSMPS);
    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}
MunitResult test_oscil3_pm_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf[BUF_SZ] = {0.0};
    set_ftable_fixture(buf, BUF_SZ, fill_cos);

    float sr = 48000;

    oscil osc;
    oscil_init(&osc, buf, BUF_SZ, 120.0, 0.0, sr);

    float out[NSMPS] = {0.0};
    float freq[NSMPS] = {0.0};
    float phs[NSMPS] = {0.0};

    fill_dc(freq, 440.0, NSMPS);
    fill_line(phs, 0.0, 1.0, NSMPS);

    oscil3_pm_tick_block(&osc, out, freq, phs, 0, NSMPS);

    check_range_inclusive(out, -1.0, 1.0, NSMPS);
    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}

MunitResult test_blxoscili_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf1[BUF_SZ] = {0.0};
    float buf2[BUF_SZ] = {0.0};

    float sr = 48000;

    float d_buf[2 * BUF_SZ] = {0};
    matrix deck;
    matrix_init(&deck, d_buf, 2, BUF_SZ);

    set_deck_fixture(&deck, buf1, buf2, BUF_SZ, fill_cos, fill_sin);

    // TODO: this should really be refactored...
    // 1. Either bl/xoscil should own their oscils or the references should be
    // fully initialized by their updates .. this partial just feels like noise.
    oscil l;
    oscil_init(&l, matrix_get_row(&deck, 0), BUF_SZ, 440.0, 0.0, sr);

    oscil r;
    oscil_init(&r, matrix_get_row(&deck, 1), BUF_SZ, 440.0, 0.0, sr);

    blxoscil osc;

    float f0[2] = {20.0f, 1000.0f};

    blxoscil_init(&osc, &deck, &l, &r, f0, 120.0, 0.0);

    float out[NSMPS] = {0.0};
    float freq[NSMPS] = {0.0};

    // this line should be enough to cross the positional threshold for the xfade
    fill_line(freq, 100.0, 300.0, NSMPS);

    blxoscili_tick_block(&osc, out, freq, 0, NSMPS);

    check_range_inclusive(out, -1.0, 1.0, NSMPS);
    check_any_nonzero(out, NSMPS);
    return MUNIT_OK;
}

MunitResult test_blxoscil3_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf1[BUF_SZ] = {0.0};
    float buf2[BUF_SZ] = {0.0};

    float sr = 48000;

    float d_buf[2 * BUF_SZ] = {0};
    matrix deck;
    matrix_init(&deck, d_buf, 2, BUF_SZ);

    set_deck_fixture(&deck, buf1, buf2, BUF_SZ, fill_cos, fill_sin);

    // TODO: this should really be refactored...
    // 1. Either bl/xoscil should own their oscils or the references should be
    // fully initialized by their updates .. this partial just feels like noise.
    oscil l;
    oscil_init(&l, matrix_get_row(&deck, 0), BUF_SZ, 440.0, 0.0, sr);

    oscil r;
    oscil_init(&r, matrix_get_row(&deck, 1), BUF_SZ, 440.0, 0.0, sr);

    blxoscil osc;

    float f0[2] = {20.0f, 1000.0f};

    blxoscil_init(&osc, &deck, &l, &r, f0, 120.0, 0.0);

    float out[NSMPS] = {0.0};
    float freq[NSMPS] = {0.0};

    // this line should be enough to cross the positional threshold for the xfade
    fill_line(freq, 100.0, 300.0, NSMPS);

    blxoscil3_tick_block(&osc, out, freq, 0, NSMPS);

    check_range_inclusive(out, -1.0, 1.0, NSMPS);
    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}

MunitResult test_xoscili_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf1[BUF_SZ] = {0.0};
    float buf2[BUF_SZ] = {0.0};

    float sr = 48000;

    float d_buf[2 * BUF_SZ] = {0};
    matrix deck;
    matrix_init(&deck, d_buf, 2, BUF_SZ);

    set_deck_fixture(&deck, buf1, buf2, BUF_SZ, fill_cos, fill_sin);

    // TODO: this should really be refactored...
    // 1. Either bl/xoscil should own their oscils or the references should be
    // fully initialized by their updates .. this partial just feels like noise.
    oscil l;
    oscil_init(&l, matrix_get_row(&deck, 0), BUF_SZ, 440.0, 0.0, sr);

    oscil r;
    oscil_init(&r, matrix_get_row(&deck, 1), BUF_SZ, 440.0, 0.0, sr);

    xoscil osc;
    xoscil_init(&osc, &deck, &l, &r, 120.0, 0.0, 0.0);

    float out[NSMPS] = {0.0};
    float freq[NSMPS] = {0.0};
    float pos[NSMPS] = {0.0};

    fill_line(freq, 100.0, 300.0, NSMPS);
    fill_line(pos, 0.0, 1.0, NSMPS);

    xoscili_tick_block(&osc, out, freq, pos, 0, NSMPS);

    check_range_inclusive(out, -1.0, 1.0, NSMPS);

    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}

MunitResult test_xoscil3_tick_block(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float buf1[BUF_SZ] = {0.0};
    float buf2[BUF_SZ] = {0.0};

    float sr = 48000;

    float d_buf[2 * BUF_SZ] = {0};
    matrix deck;
    matrix_init(&deck, d_buf, 2, BUF_SZ);

    set_deck_fixture(&deck, buf1, buf2, BUF_SZ, fill_cos, fill_sin);

    // TODO: this should really be refactored...
    // 1. Either bl/xoscil should own their oscils or the references should be
    // fully initialized by their updates .. this partial just feels like noise.
    oscil l;
    oscil_init(&l, matrix_get_row(&deck, 0), BUF_SZ, 440.0, 0.0, sr);

    oscil r;
    oscil_init(&r, matrix_get_row(&deck, 1), BUF_SZ, 440.0, 0.0, sr);

    xoscil osc;
    xoscil_init(&osc, &deck, &l, &r, 120.0, 0.0, 0.0);

    float out[NSMPS] = {0.0};
    float freq[NSMPS] = {0.0};
    float pos[NSMPS] = {0.0};

    fill_line(freq, 100.0, 300.0, NSMPS);
    fill_line(pos, 0.0, 1.0, NSMPS);

    xoscil3_tick_block(&osc, out, freq, pos, 0, NSMPS);

    check_range_inclusive(out, -1.0, 1.0, NSMPS);

    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}
