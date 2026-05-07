/**
 * @brief this is an integration test to cover a very specific resynthesis.
 * It requires a hop size factor of 2 (fft_sz//2) and a one way hanning window on a
 * DC signal.
 *
 * We will expect the signal to start on index 16.. we will see the upslope of the
 * first window rise and then reconstruct the DC signal with a non-trivial epsilon.
 *
 * This will satisfy the COLA constraint without the need to normalize. A second
 * integration test will be provided when we implement cola normalization that should
 * allow us to satisfy other hop sizes.
 *
 * https://ccrma.stanford.edu/~jos/OLA/COLA_Windows.html
 * https://ccrma.stanford.edu/~jos/OLA/COLA_Window_Hop_Size_Examples.html
 */
#include <dsp/stft.h>

#include "test_stft.h"

#include <stdio.h>

#define FFT_SZ 16
#define SIG_SZ FFT_SZ * 10

#include <math.h>
#include <stdlib.h>

// makes an asymmetrical window for an even sized sz
static inline void hanning(float* buf, uint32_t sz) {
    for (uint32_t i = 0; i < sz; i++)
        buf[i] = 0.5f - (0.5f * cosf(2.0f * M_PI * i / sz));
}

// mock these two
struct rdft {
    uint32_t n;
};

struct irdft {
    uint32_t n;
};

// return the spectral frame of a DC
void mock_rdft_execute(rdft* self, dft_complex* c_out, float* r_in) {
    (void) r_in;
    for (uint32_t i = 0; i < self->n; i++) {
        c_out[i][0] = 0.0;
        c_out[i][1] = 0.0;
    }
    c_out[0][0] = 1.0;
}

void mock_irdft_execute(irdft* self, float* r_out, dft_complex* c_in) {
    (void) c_in;
    float tmp[self->n];
    hanning(tmp, self->n);
    // for (uint32_t i = 0; i < self->n; i++) {
    //     printf("\t%.7f\n", tmp[i]);
    // }
    for (uint32_t i = 0; i < self->n; i++) {
        r_out[i] = tmp[i];
    }
}

typedef struct {
    float* in_q;
    float* out_q;
    stft* fft;
    istft* ifft;
    size_t fft_sz;
    size_t hop_sz;
    size_t in_ptr;
    size_t out_ptr;
} engine;

void engine_destroy(engine* self) {
    if (self->in_q)
        free(self->in_q);
    if (self->out_q)
        free(self->out_q);
    if (self->fft) {
        free(self->fft->dft);
        free(self->fft->buf);
        free(self->fft->win);
        free(self->fft);
    }
    if (self->ifft) {
        free(self->ifft->idft);
        free(self->ifft->ola);
        free(self->ifft);
    }
    free(self);
}

engine* engine_create(uint32_t fft_sz, uint32_t hop_sz) {

    // NOTE: this engine will take ownership of the win float* buf
    float* win = (float*) malloc(sizeof(float) * fft_sz);
    hanning(win, fft_sz);

    // temp bufs for v0.3
    float* stft_tmp_buf_ = (float*) malloc(sizeof(float) * fft_sz);
    float* istft_tmp_buf_ = (float*) malloc(sizeof(float) * fft_sz);

    // set up these mocks ..
    rdft* dft = (rdft*) malloc(sizeof(rdft));
    dft->n = fft_sz;

    irdft* idft = (irdft*) malloc(sizeof(irdft));
    idft->n = fft_sz;

    engine* obj = (engine*) malloc(sizeof(engine));
    if (!obj) {
        return NULL;
    }

    stft* forward = (stft*) malloc(sizeof(stft));
    if (!forward) {
        return NULL;
    }
    uint32_t buf_sz = fft_sz * 2;
    float* buf = (float*) malloc(sizeof(float) * buf_sz);
    float* ola = (float*) malloc(sizeof(float) * buf_sz);

    stft_init(forward, dft, mock_rdft_execute, win, buf, stft_tmp_buf_, fft_sz, hop_sz,
              buf_sz);

    istft* backward = (istft*) malloc(sizeof(istft));
    if (!backward) {
        return NULL;
    }

    istft_init(backward, idft, mock_irdft_execute, ola, NULL, NULL, istft_tmp_buf_,
               fft_sz, hop_sz, buf_sz);

    float* in_q = (float*) malloc(sizeof(float) * hop_sz);
    if (!in_q) {
        return NULL;
    }
    float* out_q = (float*) malloc(sizeof(float) * hop_sz);
    if (!out_q) {
        return NULL;
    }
    memset(in_q, 0, sizeof(float) * hop_sz);
    memset(out_q, 0, sizeof(float) * hop_sz);

    obj->fft = forward;
    obj->ifft = backward;
    obj->fft_sz = fft_sz;
    obj->hop_sz = hop_sz;
    obj->in_ptr = 0;
    obj->out_ptr = 0;

    obj->in_q = in_q;
    obj->out_q = out_q;

    return obj;
}

void engine_tick_block(engine* self, float* out, float* in, size_t nsmps) {
    for (size_t i = 0; i < nsmps; i++) {
        self->in_q[self->in_ptr++] = in[i];
        // for (size_t i = 0; i < self->hop_sz; i++) {
        //     printf("%.7f, ", self->in_q[i]);
        // }
        // printf("\n");
        if (self->in_ptr == self->hop_sz) {

            dft_complex spec[self->fft_sz];
            memset(spec, 0, sizeof(dft_complex) * self->fft_sz);

            bool ready = stft_tick(self->fft, spec, self->in_q);
            // printf("ready: %d\n", ready);

            if (ready) {
                istft_tick(self->ifft, self->out_q, spec);
                self->out_ptr = self->hop_sz;
            }
            self->in_ptr = 0;
        }

        if (self->out_ptr > 0) {
            out[i] = self->out_q[self->hop_sz - self->out_ptr];
            self->out_ptr--;
        } else {
            out[i] = 0.0;
        }
    }
}

MunitResult test_stft_engine(const MunitParameter params[], void* data) {
    (void) params;
    (void) data;

    engine* eng = engine_create(FFT_SZ, FFT_SZ / 2);

    float test_sig[SIG_SZ] = {0.0};
    float out_sig[SIG_SZ] = {0.0};
    for (int i = 0; i < SIG_SZ; i++) {
        test_sig[i] = 1.0;
    }

    int krate = FFT_SZ / 2;

    int ptr = 0;
    while (ptr < SIG_SZ) {
        float in_block[krate];
        float out_block[krate];

        for (int i = 0; i < krate; i++, ptr++) {
            in_block[i] = test_sig[ptr];
        }
        // printf("engine tick: \n");
        engine_tick_block(eng, out_block, in_block, krate);

        // for (int i = 0; i < krate; i++)
        //     printf("%.5f\n", out_block[i]);

        int current = ptr - krate;
        for (int i = 0; i < krate; i++, current++) {
            out_sig[current] = out_block[i];
        }
    }

    for (int i = 0; i < SIG_SZ; i++) {

        // printf("%.7f\n", out_sig[i]);
        if (i <= 15)
            munit_assert_float(out_sig[i], ==, 0.0);
        switch (i) {
            case 16:
                munit_assert_double_equal(out_sig[i], 0.0380602, 6);
                break;
            case 17:
                munit_assert_double_equal(out_sig[i], 0.1464466, 6);
                break;
            case 18:
                munit_assert_double_equal(out_sig[i], 0.3086583, 6);
                break;
            case 19:
                munit_assert_double_equal(out_sig[i], 0.5000000, 6);
                break;
            case 20:
                munit_assert_double_equal(out_sig[i], 0.6913417, 6);
                break;
            case 21:
                munit_assert_double_equal(out_sig[i], 0.8535534, 6);
                break;
            case 22:
                munit_assert_double_equal(out_sig[i], 0.9619398, 6);
                break;
            default:
                break;
        }
        if (i >= 23) {
            munit_assert_double_equal(out_sig[i], 1.0, 6);
        }
    }
    engine_destroy(eng);
    return MUNIT_OK;
}
