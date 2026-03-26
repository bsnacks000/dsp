/**
 * @brief - test convolution engines
 *
 * NOTE: provides constructors that can be used as a reference for downstream
 * applications. Sizing things for these objects are particularly tricky.
 *
 * These constructors were vetted with valgrind and worked for the `zconv` standalone
 * application that was developed outside of this project.
 */

#include "test_conv.h"

#define KSMPS 64
#define SIG_SZ KSMPS * 4

static inline dconv* dconv_new(float* ir, int32_t ir_sz) {
    dconv* engine = (dconv*) malloc(sizeof(dconv));

    // calculate delay line size
    int32_t dline_sz = 1;
    while (dline_sz < ir_sz)
        dline_sz <<= 1;
    dline_sz <<= 1;  // double it

    float* dline = (float*) calloc(dline_sz, sizeof(float));

    dconv_init(engine, ir, ir_sz, dline, dline_sz);

    return engine;
}

static inline void dconv_destroy(dconv* self) {
    free(self->dline_);
    free(self);
}

MunitResult test_dconv(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float x[SIG_SZ] = {0.0};
    x[0] = 1.0;
    float h[2] = {0.5, 0.5};

    dconv* engine = dconv_new(h, 2);

    float out[KSMPS] = {0.0};
    float in[KSMPS] = {0.0};

    float expected[SIG_SZ] = {0.0};

    for (int i = 0; i < SIG_SZ; i += KSMPS) {
        for (int j = 0; j < KSMPS; j++)
            in[j] = x[j + i];
        dconv_tick_block(engine, out, in, 0, KSMPS);
        for (int j = 0; j < KSMPS; j++)
            expected[i + j] = out[j];
    }

    for (uint32_t i = 0; i < SIG_SZ; i++) {
        if (i == 0 || i == 1)
            munit_assert_double_equal(expected[i], 0.5, 7);
        else
            munit_assert_double_equal(expected[i], 0.0, 7);
    }

    dconv_destroy(engine);

    return MUNIT_OK;
}
MunitResult test_mpconv(const MunitParameter params[], void* data) {

    (void) params;
    (void) data;
    return MUNIT_OK;
}
MunitResult test_zconv(const MunitParameter params[], void* data) {

    (void) params;
    (void) data;
    return MUNIT_OK;
}
