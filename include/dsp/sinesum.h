/**
 * @brief sinesum
 *
 * Functions for constructing wavetables based on a sum of sinusoids.
 *
 * The current implementation is based on GEN10 in Csound. There is currently no
 * feature to adjust phases per harmonic amplitude
 *
 */

#ifndef DSP_SINESUM_H
#define DSP_SINESUM_H

#include "dsp/constants.h"
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <dsp/assert.h>
#include <dsp/maths.h>
#include <dsp/utils.h>

/**
 * @brief The lanczos smoothing interpolation formula. Mainly used to round off
 * waveforms with high harmonics during waveform generation.
 *
 * This is adapted from Pirkle (synth book)
 *
 * @param i  index (current harmonic)
 * @param n  total harmonics  (can not be zero)
 */
static inline float lanczos_smoothing(uint32_t i, uint32_t n) {
    dsp_assert(n > 0, "n must be greater then zero.");
    float x = (float) (i + 1) * PI_F / (float) n;
    return sinf(x) / x;
}

static inline void sinesum(float* buf,
                           uint32_t buf_sz,
                           float* amps,
                           uint32_t amps_sz,
                           float phase,
                           bool smooth) {

    uint32_t nharms = amps_sz;
    // NOTE: nharms is an alias used elsewhere pointing to amps_sz
    // https://github.com/bsnacks000/dsp/blob/f8e61a3afa8756d59f1d762474eb727baf423e03/utils/sinesum.c#L98

    phase *= TWO_PI_F;
    float sigma = 1.0f;
    for (uint32_t i = 0; i < nharms; i++) {
        if (smooth) {
            sigma = lanczos_smoothing(i, nharms);
        }
        for (uint32_t j = 0; j < buf_sz; j++) {
            float freq = ((float) i + 1.0f) * ((float) j * TWO_PI_F / (float) buf_sz);
            buf[j] += amps[i] * sinf(freq + phase) * sigma;
        }
    }
    normalize(buf, buf_sz);
}

/**
 * @brief Class Waveform calculations
 *
 */

/**
 * @brief calculate a sine wave. It is really just enforcing the first harmonic
 * is set to 1.0 and there rest zeroed.
 */
static inline void sine_amps(float* amps, uint32_t amps_sz) {
    // we are assuming amps_sz >=1
    amps[0] = 1.0;
    for (uint32_t i = 1; i < amps_sz; i++) {
        amps[i] = 0.0;
    }
}

/**
 * @brief calculate a buzz buffer. This is 1.0 for all amplitudes.
 */
static inline void buzz_amps(float* amps, uint32_t amps_sz) {
    for (uint32_t i = 0; i < amps_sz; i++) {
        amps[i] = 1.0;
    }
}

/**
 * @brief calculate saw buffer.
 */
static inline void saw_amps(float* amps, uint32_t amps_sz) {
    for (uint32_t i = 0; i < amps_sz; i++)
        amps[i] = 1.0f / (float) (i + 1);
}

/**
 * @brief calculate sqr buffer.
 */
static inline void sqr_amps(float* amps, uint32_t amps_sz) {
    for (uint32_t i = 0; i < amps_sz; i++) {
        if (i % 2 == 0) {
            amps[i] = 1.0f / (float) (i + 1);
        } else {
            amps[i] = 0.0;
        }
    }
}

/**
 * @brief calculate a triangle buffer
 */
static inline void tri_amps(float* amps, uint32_t amps_sz) {
    float scale = 8.0f / TWO_PI_F;
    for (uint32_t i = 0; i < amps_sz; i++) {
        int h = (int) i + 1;
        if (h % 2 == 1) {  // loop odd harmonic
            int harm = (h - 1) / 2;
            float sign = (harm % 2 == 0) ? 1.0f : -1.0f;
            float hf = (float) h;
            amps[i] = sign * scale / (hf * hf);
        } else {
            amps[i] = 0.0;
        }
    }
}

/**
 * Below is functionality for generating decks with sinesum based on a
 * fundamental set of waveform amplitudes.
 *
 * This API can be used to create band limited ftable oscillators using
 * crossfading w/ blxoscil. In order to do this each ftable must set its max
 * fundamental frequency (f0) that should be used before aliasing.
 *
 * Usage to create a band limited deck:
 *  - calculate desired n_bands and build the appropriate number of ftables and
 * args.
 *  - Share the desired *amps* amongst the args.
 *  - reduce the number of nharms across the arguments
 *      - Ex. nharms = 64, 32, 16, 8, 4, 2, 1
 *  - call sinesum_deck_generate(...)
 *  - wrap ftable** to a ft_deck and initialize with blxoscil.
 *
 * sinesum_deck_generate has the important side effect of setting f0 on the
 * ftable which will work with blxoscil's frequency lookup. The above reduction
 * formula will guarantee a deck with a continuous frequency range and gradual
 * reduction of harmonics up until nyquist.
 *
 * If we vary the above logic (different args/harmonics) per frame we will
 * produce a set of ftables that may not be guaranteed to work with blxoscil.
 *
 * Instead at this step these tables can be extracted for use with xoscil which
 * does crossfade morphing. Since no anti-aliasing is in effect care should be
 * taken to avoid freqs that will alias or use oversampling etc.
 */

/**
 * @brief Given the number of desired harmonics and sample rate return the
 * maximum fundmental before aliasing. Scale should be between 0-1.
 */
static inline float max_fundamental(uint32_t nharms, float sr, float scale) {
    return scale * sr / (2.0f * (float) nharms);
}

/**
 * @brief Given the fundamental f0 and sr return the maximum harmonic before
 * aliasing. Inverse of max_fundamental.
 */
static inline uint32_t max_harmonic(float f0, float sr) {
    return (uint32_t) floorf((sr / 2.0f) / f0);
}

/**
 * @brief Given the desired numnber of harmonics return the number of bands.
 * Helper for constructing band limited decks.
 */
static inline uint32_t calculate_n_bands(uint32_t nharms) {
    if (nharms <= 1)
        return 1;

    uint32_t count = 1;
    for (;;) {
        if (nharms < 2)
            break;
        nharms /= 2;
        count++;
    }

    return count;
}

#ifdef __cplusplus
}
#endif

#endif
