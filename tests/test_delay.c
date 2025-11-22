#include "test_delay.h"
#include <dsp/delay.h>
#include "helpers.h"

#define BUF_SZ 1026
#define NSMPS 64

MunitResult test_delay_line_calculate_buf_sz(const MunitParameter params[],
                                             void* data) {
    (void) params;
    (void) data;

    // buf size for delay line is next pow2 + 2
    uint32_t x = delay_line_calculate_buf_sz(48000.0, 1000.0);
    munit_assert_uint32(x, ==, 65536 + 2);

    return MUNIT_OK;
}

MunitResult test_delay_line_init(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    delay_line d;
    float buf[BUF_SZ];

    delay_line_init(&d, buf, BUF_SZ);

    munit_assert_uint32(d.buf_sz, ==, BUF_SZ);        // pow2 + 2 (1026)
    munit_assert_uint32(d.wrap_sz_, ==, BUF_SZ - 2);  // wrap 1024
    munit_assert_uint32(d.mask_, ==, BUF_SZ - 3);     // mask 1023
    munit_assert_uint32(d.write_idx, ==, 0);          // init write idx

    return MUNIT_OK;
}

MunitResult test_delay_line_advance(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    delay_line d;
    float buf[BUF_SZ] = {0.0};
    delay_line_init(&d, buf, BUF_SZ);

    delay_line_advance(&d, 1024);  // we should wrap here given 1026 buf sz
    munit_assert_uint32(d.write_idx, ==, 0);

    return MUNIT_OK;
}

MunitResult test_delay_line_write(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    delay_line d;
    float buf[BUF_SZ] = {0.0};
    delay_line_init(&d, buf, BUF_SZ);

    float in[NSMPS];
    fill_dc(in, 1.0, NSMPS);

    delay_line_advance(&d, NSMPS);
    delay_line_write(&d, in, NSMPS);

    // start writing 64 nsmps in
    for (int i = NSMPS; i < NSMPS; i++)
        munit_assert_double_equal(d.buf[i], 1.0, 1);

    // assure we hit guard point branch arm...
    // run over BUF_SZ and make sure we set
    for (int i = 0; i < 100; i++) {
        delay_line_advance(&d, NSMPS);
        delay_line_write(&d, in, NSMPS);
    }

    munit_assert_double_equal(d.buf[d.wrap_sz_], 1.0, 1);
    munit_assert_double_equal(d.buf[d.wrap_sz_ + 1], 1.0, 1);

    return MUNIT_OK;
}

MunitResult test_delay_line_tapi(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    delay_line d;
    float buf[BUF_SZ] = {0.0};
    delay_line_init(&d, buf, BUF_SZ);

    float out[NSMPS] = {0.0};

    float offset[NSMPS];
    fill_dc(offset, 423.1234, NSMPS);

    float in[NSMPS];
    fill_dc(in, 1.0, NSMPS);

    for (int i = 0; i < 100; i++) {
        delay_line_advance(&d, NSMPS);
        delay_line_tapi(&d, out, offset, NSMPS);
        delay_line_write(&d, in, NSMPS);
    }

    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}

MunitResult test_delay_line_tap3(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    delay_line d;
    float buf[BUF_SZ] = {0.0};
    delay_line_init(&d, buf, BUF_SZ);

    float out[NSMPS] = {0.0};

    float offset[NSMPS];
    fill_dc(offset, 423.1234, NSMPS);

    float in[NSMPS];
    fill_dc(in, 1.0, NSMPS);

    for (int i = 0; i < 100; i++) {
        delay_line_advance(&d, NSMPS);
        delay_line_tap3(&d, out, offset, NSMPS);
        delay_line_write(&d, in, NSMPS);
    }

    check_any_nonzero(out, NSMPS);

    return MUNIT_OK;
}
