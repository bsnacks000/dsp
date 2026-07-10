/**
 * @file resample.h
 * @brief utilities for resampling / oversampling.
 *
 * Based on Pirkle(2019) and others.
 *
 */

// SPDX-License-Identifier: MIT

#ifndef DSP_SAMPLE_H
#define DSP_SAMPLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/assert.h>
#include <dsp/utils.h>

#include <stdint.h>

/**
 * @brief upsample by back fill.
 * @note out_sz/in_sz must be a power of 2.
 */
static inline void upsample_back_fill(float* out,
                                      uint32_t out_sz,
                                      float* in,
                                      uint32_t in_sz) {
    uint32_t factor = out_sz / in_sz;
    dsp_assert(is_pow2(factor),
               "upsample_zero_pad: out_sz/in_sz must be a power of 2.");

    for (uint32_t i = 0; i < in_sz; i++) {
        for (uint32_t j = 0; j < factor; j++) {
            out[i * factor + j] = in[i];
        }
    }
}

/**
 * @brief upsample by zero padding.
   @note out_sz/in_sz must be a power of 2.
*/

static inline void upsample_zero_pad(float* out,
                                     uint32_t out_sz,
                                     float* in,
                                     uint32_t in_sz) {
    uint32_t factor = out_sz / in_sz;
    dsp_assert(is_pow2(factor),
               "upsample_zero_pad: out_sz/in_sz must be a power of 2.");

    memset(out, 0, out_sz * sizeof(float));
    for (uint32_t i = 0; i < in_sz; i++) {
        out[i * factor] = in[i];
    }
}

/**
 * @brief downsample by decimation.
 * @note in_sz/out_sz must be a power of 2.
 */
static inline void downsample_decimate(float* out,
                                       uint32_t out_sz,
                                       float* in,
                                       uint32_t in_sz) {
    uint32_t factor = in_sz / out_sz;
    dsp_assert(is_pow2(factor),
               "downsample_decimate: in_sz/out_sz must be a power of 2.");

    for (uint32_t i = 0; i < out_sz; i++) {
        out[i] = in[i * factor];
    }
}

/**
 * @brief downsample by averaging.
 * @note in_sz/out_sz must be a power of 2.
 */
static inline void downsample_avg(float* out,
                                  uint32_t out_sz,
                                  float* in,
                                  uint32_t in_sz) {
    uint32_t factor = in_sz / out_sz;
    dsp_assert(is_pow2(factor), "downsample_avg: in_sz/out_sz must be a power of 2.");

    for (uint32_t i = 0; i < out_sz; i++) {
        float s = 0.0;
        for (uint32_t j = 0; j < factor; j++) {
            s += in[i * factor + j];
        }
        out[i] = s / (float) factor;
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
    dsp_assert(is_pow2(factor), "downsample_lin: in_sz/out_sz must be a power of 2.");

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
