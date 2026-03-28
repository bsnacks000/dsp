/**
 * @brief utils for oversampling
 */

#ifndef DSP_SAMPLE_H
#define DSP_SAMPLE_H

#include <sys/types.h>
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>

/**
 * @brief upsample by back fill.
 */
static inline void upsample_back_fill(float* out,
                                      uint32_t out_sz,
                                      float* in,
                                      uint32_t in_sz) {
    uint32_t factor = out_sz / in_sz;
    // TODO: assert pow2
    for (uint32_t i = 0; i < in_sz; i++) {
        for (uint32_t j = 0; j < factor; j++) {
            out[i * factor + j] = in[i];
        }
    }
}

/**
 * @brief upsample by zero padding.
 */
static inline void upsample_zero_pad(float* out,
                                     uint32_t out_sz,
                                     float* in,
                                     uint32_t in_sz) {
    uint32_t factor = out_sz / in_sz;
    // TODO: assert pow2

    memset(out, 0, out_sz * sizeof(float));
    for (uint32_t i = 0; i < in_sz; i++) {
        out[i * factor] = in[i];
    }
}

/**
 * @brief downsample by decimation.
 */
static inline void downsample_decimate(float* out,
                                       uint32_t out_sz,
                                       float* in,
                                       uint32_t in_sz) {
    uint32_t factor = in_sz / out_sz;
    // TODO: assert pow2

    for (uint32_t i = 0; i < out_sz; i++) {
        out[i] = in[i * factor];
    }
}

/**
 * @brief downsample by averaging.
 */
static inline void downsample_avg(float* out,
                                  uint32_t out_sz,
                                  float* in,
                                  uint32_t in_sz) {
    uint32_t factor = in_sz / out_sz;
    // TODO: assert pow2

    for (uint32_t i = 0; i < out_sz; i++) {
        float s = 0.0;
        for (uint32_t j = 0; j < factor; j++) {
            s += in[i * factor + j];
        }
        out[i] = s / factor;
    }
}

/**
 * @brief downsample by linear_interpolation.
 */
static inline void downsample_lin(float* out,
                                  uint32_t out_sz,
                                  float* in,
                                  uint32_t in_sz) {
    uint32_t factor = in_sz / out_sz;
    // TODO: assert pow2

    for (uint32_t i = 0; i < out_sz; i++) {
        uint32_t base = i * factor;
        uint32_t left = base + factor / 2 - 1;
        uint32_t right = base + factor / 2;
        out[i] = in[left] + 0.5f * (in[right] - in[left]);
    }
}

#ifdef __cplusplus
}
#endif
#endif
