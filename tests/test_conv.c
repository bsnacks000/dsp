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
#include "dsp/dft.h"

#define KSMPS 64
#define SIG_SZ KSMPS * 4

dconv* dconv_new(float* ir, int32_t ir_sz) {
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

void dconv_destroy(dconv* self) {
    free(self->dline_);
    free(self);
}

// mocks for fft
struct rdft {
    uint32_t n;
    bool scale;
};

struct irdft {
    uint32_t n;
    bool scale;
};

rdft* mock_forward_ctor(uint32_t n, bool scale) {
    rdft* obj = (rdft*) calloc(1, sizeof(rdft));
    if (!obj)
        abort();
    obj->n = n;
    obj->scale = scale;
    return obj;
}

irdft* mock_inverse_ctor(uint32_t n, bool scale) {
    irdft* obj = (irdft*) calloc(1, sizeof(irdft));
    if (!obj)
        abort();
    obj->n = n;
    obj->scale = scale;
    return obj;
}

void mock_forward_destroy(rdft* self) {
    free(self);
}
void mock_inverse_destroy(irdft* self) {
    free(self);
}

// NOTE: these don't really do anything .. the tests for mpconv
// are mostly smoke / coverage. True test prototypes for now must live
// in the csound implementation or downstream or in a downstream project.
//
// in the future if fftw3 is added to the test suite then a more realistic integration
// test can exist.

void mock_forward_execute(rdft* self, dft_complex* c_out, float* r_in) {
    (void) r_in;
    for (uint32_t i = 0; i < self->n / 2 + 1; i++) {
        c_out[i][0] = 0.0f;
        c_out[i][1] = 0.0f;
    }
}

void mock_inverse_execute(irdft* self, float* r_out, dft_complex* c_in) {
    (void) c_in;
    for (uint32_t i = 0; i < self->n; i++) {
        r_out[i] = 0.0f;
    }
}

mpconv* mpconv_new(const float* ir,
                   uint32_t ir_sz,
                   uint32_t part_sz,
                   uint32_t skip_samps,
                   uint32_t ir_len,
                   rdft_create forward_ctor,
                   irdft_create inverse_ctor,
                   rdft_execute forward_fft,
                   irdft_execute inverse_fft) {

    // if ir_len = 0 then we use the rest of the ir
    if (!ir_len) {
        ir_len = ir_sz - skip_samps;
    }

    if (skip_samps >= ir_sz) {
        ir_len = 0;
    } else if (skip_samps + ir_len > ir_sz) {
        ir_len = ir_sz - skip_samps;
    }
    // after the maths above we must have a valid ir_len to proceed else we get UB
    // assert(ir_len > 0);

    uint32_t ir_start = skip_samps;
    uint32_t ir_end = skip_samps + ir_len;

    mpconv_kernel* kernel = (mpconv_kernel*) malloc(sizeof(mpconv_kernel));
    mpconv* engine = (mpconv*) malloc(sizeof(mpconv));

    mpconv_kernel_sz kernel_sz = calculate_mpconv_kernel_sz(ir_len, part_sz);

    uint32_t frame_sz = kernel_sz.frame_sz;     // L
    uint32_t block_sz = kernel_sz.block_sz;     // N (2L)
    uint32_t n_parts = kernel_sz.n_partitions;  // P
    uint32_t h_sz = kernel_sz.h_sz;
    uint32_t h_buf_sz = kernel_sz.h_buf_sz;  // h_sz * P

    // rfft/irfft pair -- setup inverse scaling
    rdft* rfft = forward_ctor(block_sz, false);
    irdft* irfft = inverse_ctor(block_sz, true);

    dft_complex* h = (dft_complex*) calloc(h_buf_sz, sizeof(dft_complex));

    // fill partitions
    // TODO: this should be broken out into its own function in conv.h
    //  - it is critical for mpconv to function correctly.

    float* hh = calloc(block_sz, sizeof(float));

    for (uint32_t p = 0; p < n_parts; p++) {

        memset(hh, 0, block_sz * sizeof(float));
        uint32_t base = ir_start + p * part_sz;

        for (uint32_t i = 0; i < part_sz; i++) {
            uint32_t idx = base + i;
            if (idx < ir_end)
                hh[i] = ir[idx];
        }
        forward_fft(rfft, &h[p * h_sz], hh);
    }
    free(hh);

    // kernel buffers
    float* ola = (float*) calloc(frame_sz, sizeof(float));
    float* block = (float*) calloc(block_sz, sizeof(float));

    dft_complex* y = (dft_complex*) calloc(h_sz, sizeof(dft_complex));
    dft_complex* dline = (dft_complex*) calloc(h_buf_sz, sizeof(dft_complex));

    // pconv engine buffers and fifos
    float* in_ring = (float*) calloc(block_sz, sizeof(float));
    float* out_ring = (float*) calloc(block_sz, sizeof(float));

    float* frame_q_ = (float*) calloc(frame_sz, sizeof(float));
    float* frame_y_ = (float*) calloc(frame_sz, sizeof(float));

    fifo_f* in_q = (fifo_f*) malloc(sizeof(fifo_f));
    fifo_f* out_q = (fifo_f*) malloc(sizeof(fifo_f));

    mpconv_kernel_init(kernel, block, ola, h, dline, y, rfft, irfft, forward_fft,
                       inverse_fft, kernel_sz);

    fifo_init(in_q, in_ring, block_sz);
    fifo_init(out_q, out_ring, block_sz);

    mpconv_init(engine, kernel, in_q, out_q, frame_q_, frame_y_);
    return engine;
}

void mpconv_destroy(mpconv* self,
                    rdft_destroy forward_dtor,
                    irdft_destroy inverse_dtor) {
    forward_dtor(self->kernel->rfft);
    inverse_dtor(self->kernel->irfft);

    free(self->kernel->ola);
    free(self->kernel->y);
    free(self->kernel->block);
    free(self->kernel->h);
    free(self->kernel->dline);
    free(self->kernel);

    // engine
    free(self->in_q->ring);  // rings
    free(self->out_q->ring);
    free(self->in_q);  // fifos
    free(self->out_q);

    free(self->y_);  // internals
    free(self->frame_);

    free(self);
}

zconv* zconv_new(float* ir, uint32_t ir_sz, uint32_t part_sz, uint32_t block_sz) {

    // assert(is_pow2(part_sz));

    // mid_len = (next_part_sz - current_part_sz)
    // this looks like a gap but it covers the region  head = 64 + mid = 448 = 8
    // partitions = 512
    //
    // TODO: formulate some kind of setup machinary around this
    dconv* head = dconv_new(ir, part_sz);
    mpconv* mid =
        mpconv_new(ir, ir_sz, part_sz, part_sz, part_sz * 7, mock_forward_ctor,
                   mock_inverse_ctor, mock_forward_execute, mock_inverse_execute);
    mpconv* tail =
        mpconv_new(ir, ir_sz, part_sz * 8, part_sz * 8, 0, mock_forward_ctor,
                   mock_inverse_ctor, mock_forward_execute, mock_inverse_execute);

    zconv* engine = malloc(sizeof(zconv));
    float* tmp_out = calloc(block_sz, sizeof(float));

    zconv_init(engine, head, mid, tail, tmp_out, block_sz);

    return engine;
}

void zconv_destroy(zconv* self) {
    mpconv_destroy(self->tail, mock_forward_destroy, mock_inverse_destroy);
    mpconv_destroy(self->mid, mock_forward_destroy, mock_inverse_destroy);
    dconv_destroy(self->head);

    free(self->tmp_out_);
    free(self);
}

MunitResult test_dconv(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    float x[SIG_SZ] = {0.0};
    x[0] = 1.0;
    float h[2] = {0.5, 0.5};

    // this test is accurate based on an impulse response and basic lowpass
    // ir
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

    float ir[64] = {0.0f};
    uint32_t ir_sz = 64;
    uint32_t part_sz = 8;
    uint32_t skip_samps = 0;
    uint32_t ir_len = 0;

    mpconv* engine =
        mpconv_new(ir, ir_sz, part_sz, skip_samps, ir_len, mock_forward_ctor,
                   mock_inverse_ctor, mock_forward_execute, mock_inverse_execute);

    float out[64] = {0.0f};
    float in[64] = {0.0f};

    mpconv_tick_block(engine, out, in, 0, 64);

    mpconv_destroy(engine, mock_forward_destroy, mock_inverse_destroy);

    return MUNIT_OK;
}

MunitResult test_zconv(const MunitParameter params[], void* data) {

    (void) params;
    (void) data;

    float ir[512] = {0.0f};
    uint32_t ir_sz = 512;
    uint32_t part_sz = 8;
    uint32_t block_sz = 16;

    zconv* engine = zconv_new(ir, ir_sz, part_sz, block_sz);

    // NOTE that the current impl requires block_sz at init and runtime to be the
    // same ..
    float out[16] = {0.0f};
    float in[16] = {0.0f};

    zconv_tick_block(engine, out, in, 0, 16);

    zconv_destroy(engine);

    return MUNIT_OK;
}
