/**
 * @brief stft - short time fourier transform primitives
 *
 * NOTES on invariants:
 *  - fft_sz must be power of 2
 *  - complex fft_frame_sz is calculated as fft_sz // 2 + 1
 *  - hop_sz must be greater then 1.
 *  - If hop sz becomes greater then fft_sz then buf_sz must be large enough to handle
 *      the max overlap. An engine that handles hop sz changes during runtime should
 * assure there is enough buffer space to handle the required hops.
 *  - real_in and real_out are assumed to be hop_sz in length.
 *  - c_in and c_out are always the fft_sz  // 2 + 1
 *  - window sz is fft_sz
 *  - windows are optional for istft. An stft must always take a window. A window of
 * [1.0] * fft_sz will compute a rectangular window.
 *  - some applications may require ola normalization during resynthesis. This should be
 * computed in advance as a function of fft_sz, hop_sz
 */
#ifndef DSP_STFT_H
#define DSP_STFT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/dft.h>
#include <stdbool.h>

typedef struct {
    rdft* dft;             // rdft state
    rdft_execute forward;  // forward dft function ptr
    float* buf;            // fft_sz * n internal buffer
    float* win;            // fft_sz window for overlap add
    float* tmp_buf_;       // fft_sz ... used internally
    uint32_t fft_sz;       // pow2 fft frame size
    uint32_t hop_sz;       // must be >= 1 .. usually some factor of fft_sz. See notes.
    uint32_t buf_sz;       // internal circle buf -- fft_sz * n where n is >= fft_sz

    uint32_t buf_mask_;       // calc'd as buf_sz - 1 for circular masking
    uint32_t write_ptr_;      // the current buffer write pointer location
    uint32_t write_counter_;  // accumulate until we get fftsz then fire trigger
} stft;

/**
 * @brief Initialize stft state.
 */
void stft_init(stft* self,
               rdft* dft,
               rdft_execute forward,
               float* win,
               float* buf,
               float* tmp_buf,
               uint32_t fft_sz,
               uint32_t hop_sz,
               uint32_t buf_sz);

/**
 * @brief tick a spectrum at analysis rate. Each tick loads hop sz real input into
 * the stft buffer. A positive integer is returned if an fft frame was produced this
 * tick, otherwise stft_tick returns 0
 */
bool stft_tick(stft* self, dft_complex* frame_out, float* real_in);

/*
 * @brief istft state
 */
typedef struct {

    irdft* idft;            // idft wrapper
    irdft_execute inverse;  // inverse fft function
    float* ola;             // overlap add buffer (fft_sz * n)
    float* win;             // resynthesis window (optional)
    float* ola_norm;        // ola normalization buffer (optional) same as ola_sz
    float* tmp_buf_;
    uint32_t fft_sz;  // pow2 fft siz
    uint32_t hop_sz;  // >= 1 ; usually some factor of fft_sz.
    uint32_t ola_sz;  // size of ola and ola norm circular buffers(fft_sz * n)

    uint32_t ola_mask_;   // bit mask for circ buf
    uint32_t read_ptr_;   // the current ola read pointer location
    uint32_t write_ptr_;  // the current write pointer location
} istft;

/**
 * @brief initialize istft state.
 */
void istft_init(istft* self,
                irdft* idft,
                irdft_execute inverse,
                float* ola,
                float* win,
                float* ola_norm,
                float* tmp_buf_,
                uint32_t fft_sz,
                uint32_t hop_sz,
                uint32_t ola_sz);

/**
 * @brief resynthesize one frame, overlap add and fill one hop size of real_out.
 */
void istft_tick(istft* self, float* real_out, dft_complex* frame_in);

#ifdef __cplusplus
}
#endif

#endif
