/**
 * @brief direct and fast convolution kernels/engines.
 *
 * Based mainly on lazzarini - spectral music design (2023)
 */
#ifndef DSP_CONV_H
#define DSP_CONV_H

#include <dsp/dft.h>
#include <dsp/fifo.h>

#include <dsp/assert.h>
#include <dsp/utils.h>

/**
 * @brief dconv - direct convolve
 *
 */
typedef struct {
    float* ir;       // h
    uint32_t ir_sz;  // h_sz

    // delay line -- pow2 + support negative indexing
    float* dline_;
    int32_t dline_sz_, mask_, rptr_, wptr_;
} dconv;

/**
 * @brief dconv_init
 */
void dconv_init(dconv* self,
                float* ir,
                uint32_t ir_sz,
                float* dline_,
                int32_t dline_sz_);

/**
 * @brief dconv_tick_block
 */
void dconv_tick_block(dconv* self,
                      float* out,
                      float* in,
                      uint32_t start,
                      uint32_t nsmps);

/**
 * @brief mpconv - multi partitioned convolution using a spectral delay line
 *
 */

typedef struct {
    uint32_t ir_sz;         // IR - original
    uint32_t frame_sz;      // L
    uint32_t block_sz;      // N = 2L
    uint32_t h_sz;          // (N / 2 + 1)
    uint32_t h_buf_sz;      // h_sz * n_partitions
    uint32_t part_sz;       // some pow2 > IR
    uint32_t n_partitions;  // P - calc'd as length of the offset buffer
} mpconv_kernel_sz;

static inline mpconv_kernel_sz calculate_mpconv_kernel_sz(uint32_t ir_len,
                                                          uint32_t part_sz) {
    dsp_assert(is_pow2(part_sz), "part_sz must be pow2.");
    dsp_assert(part_sz <= ir_len, "part_sz must be <= ir_len.");

    uint32_t frame_sz = part_sz;
    uint32_t block_sz = 2 * frame_sz;
    uint32_t h_sz = (block_sz / 2) + 1;
    uint32_t n_partitions = (ir_len + part_sz - 1) / part_sz;
    uint32_t h_buf_sz = n_partitions * h_sz;

    return (mpconv_kernel_sz) {
        .ir_sz = ir_len,
        .frame_sz = frame_sz,
        .block_sz = block_sz,
        .h_sz = h_sz,
        .h_buf_sz = h_buf_sz,
        .part_sz = part_sz,
        .n_partitions = n_partitions,
    };
}

/**
 * @brief mpconv_kernel
 */
typedef struct {
    mpconv_kernel_sz kernel_sz;

    float* block;  // block_sz (N)
    float* ola;    // frame_sz (L)

    dft_complex* h;      // H buffer (holds partitions) h[P][N/2+1]
    dft_complex* dline;  // circular buffer w/ %
    dft_complex* y;      // accumulate
    uint32_t ptr;        // pointer into circular buffer

    rdft* rfft;
    rdft_execute forward_fft;
    irdft* irfft;
    irdft_execute inverse_fft;

} mpconv_kernel;

/**
 * @brief mpconv_kernel_init
 */
void mpconv_kernel_init(mpconv_kernel* self,
                        float* block,
                        float* ola,
                        dft_complex* h,
                        dft_complex* dline,
                        dft_complex* y,
                        rdft* rfft,
                        irdft* irfft,
                        rdft_execute forward_fft,
                        irdft_execute inverse_fft,
                        mpconv_kernel_sz kernel_sz);

/**
 * @brief mpconv_kernel_tick_frame - tick one frame of block_sz samples. No
 * checks performed
 */
void mpconv_kernel_tick_frame(mpconv_kernel* self, float* out, float* in);

/**
 * @brief the mpconv engine.
 */
typedef struct {
    mpconv_kernel* kernel;
    fifo_f* in_q;
    fifo_f* out_q;

    // internal buffering when tick frame fires
    float *frame_, *y_;
} mpconv;

void mpconv_init(mpconv* self,
                 mpconv_kernel* kernel,
                 fifo_f* in_q,
                 fifo_f* out_q,
                 float* frame,
                 float* y);

void mpconv_tick_block(mpconv* self,
                       float* out,
                       float* in,
                       uint32_t start,
                       uint32_t nsmps);

/**
 * @brief zconv - zero latency fast convolution.
 *
 *  Allows for non uniform multi partitioning with a direct convolution head.
 *
 */
typedef struct {
    dconv* head;
    mpconv* mid;
    mpconv* tail;
} zconv;

/**
 * @brief zconv_init - initialize zconv
 */
void zconv_init(zconv* self, dconv* head, mpconv* mid, mpconv* tail);

/**
 * @brief zconv_tick_block - tick one block of zconv. A tmp buffer is provided
 * to allow the 3 convolved signals to be mixed correctly.
 *
 */
void zconv_tick_block(zconv* self,
                      float* out,
                      float* in,
                      float* tmp,
                      uint32_t start,
                      uint32_t nsmps);

#endif
