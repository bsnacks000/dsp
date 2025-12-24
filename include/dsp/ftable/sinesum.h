/**
 * @brief Sum of sinusoids.
 *
 * - Generates a buffers of summed sinusoids for use with ftable oscillators. Similar
 *  to GEN10 in csound.
 * - Based on implementations by Lazzarinni in Audio Programming Book.
 */

#ifndef DSP_ft_SINESUM_H
#define DSP_ft_SINESUM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <dsp/constants.h>
#include <dsp/ftable/ftable.h>

#include <dsp/xfade.h>

/**
 * @brief args for ft_sinesum.
 */
typedef struct {
    const float* amps;  // a list of harmonic amplitudes.
    uint32_t amps_sz;   // sz of amplitude buffer > 1
    float phase;        // btwn -1 and 1; normalized internally to TWO_PI
    bool smooth;        // w/ smoothing
    uint32_t nharms;    // should be 1 -> amps_sz; if 0 skipped and amps_sz is used.
} ft_sinesum_args;

/**
 * @brief init ft_sinesum_args. amps_sz should be pow2. Phase is clamped between (-1, 1)
 * runtime error:
 *  -  if amps_sz != pow2
 */
dsp_err ft_sinesum_args_init(ft_sinesum_args* self,
                             const float* amps,
                             uint32_t amps_sz,
                             float phase,
                             bool smooth,
                             uint32_t nharms);

/**
 * @brief fill the wt with a sum of weighted sinusoids.
 */
dsp_err ft_sinesum(ftable* wt, void* args);

/**
 * @brief helper functions
 * - Calculate the amplitude partials of the classic waveforms.
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
        amps[i] = 1.0 / (float) (i + 1);
}

/**
 * @brief calculate sqr buffer.
 */
static inline void sqr_amps(float* amps, uint32_t amps_sz) {
    for (uint32_t i = 0; i < amps_sz; i++) {
        if (i % 2 == 0) {
            amps[i] = 1.0 / (float) (i + 1);
        } else {
            amps[i] = 0.0;
        }
    }
}

/**
 * @brief calculate a triangle buffer
 */
static inline void tri_amps(float* amps, uint32_t amps_sz) {
    float scale = 8.0 / (DSP_PI * DSP_PI);
    for (uint32_t i = 0; i < amps_sz; i++) {
        int h = i + 1;
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
 * @brief even amplitude buffer. organ like tones.
 */
static inline void even_amps(float* amps, uint32_t amps_sz) {
    for (uint32_t i = 0; i < amps_sz; i++) {
        uint32_t n = i + 1;
        amps[i] = (n % 2 == 0) ? 1.0f / (float) n : 0.0f;
    }
}

/**
 * @brief exp decay buffer (0.5 decay). bell like tones.
 */
static inline void exp_decay_amps(float* amps, uint32_t amps_sz) {
    float decay = 0.5f;  // NOTE: if making as parameter should clamp to 1e-10 < 1.0
    for (uint32_t i = 0; i < amps_sz; i++) {
        amps[i] = expf(-decay * (float) (i + 1));
    }
}

/**
 * Below is functionality for generating decks with sinesum based on a fundamental
 * set of waveform amplitudes.
 *
 * This API can be used to create band limited ftable oscillators using
 * crossfading w/ blxoscil. In order to do this each ftable must set its max
 * fundamental frequency (f0) that should be used before aliasing.
 *
 * Usage to create a band limited deck:
 *  - calculate desired n_bands and build the appropriate number of ftables and args.
 *  - Share the desired *amps* amongst the args.
 *  - reduce the number of nharms across the arguments
 *      - Ex. nharms = 64, 32, 16, 8, 4, 2, 1
 *  - call sinesum_deck_generate(...)
 *  - wrap ftable** to a ft_deck and initialize with blxoscil.
 *
 * sinesum_deck_generate has the important side effect of setting f0 on the ftable
 * which will work with blxoscil's frequency lookup. The above reduction formula will
 * guarantee a deck with a continuous frequency range and gradual reduction of
 * harmonics up until nyquist.
 *
 * If we vary the above logic (different args/harmonics) per frame we will produce
 * a set of ftables that may not be guaranteed to work with blxoscil.
 *
 * Instead at this step these tables can be extracted for use with xoscil which does
 * crossfade morphing. Since no anti-aliasing is in effect care should be taken to
 * avoid freqs that will alias or use oversampling etc.
 */

/**
 * @brief Given the number of desired harmonics and sample rate return the maximum
 * fundmental before aliasing. Scale should be between 0-1.
 */
static inline float max_fundamental(uint32_t nharms, float sr, float scale) {
    return scale * sr / (2.0f * nharms);
}

/**
 * @brief Given the fundamental f0 and sr return the maximum harmonic before aliasing.
 * Inverse of max_fundamental.
 */
static inline uint32_t max_harmonic(float f0, float sr) {
    return (uint32_t) floorf((sr / 2.0f) / f0);
}

/**
 * @brief Given the desired numnber of harmonics return the number of bands. Helper
 * for constructing band limited decks.
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

/**
 * @brief generate a ftable deck with sinesum. Based on the nharms specified
 * in sinesum args will set sr and f0 on the target ftable frame.
 */
dsp_err sinesum_deck_generate(ftable** wt,
                              ft_sinesum_args** args,
                              uint32_t n_bands,
                              float sr);

#ifdef __cplusplus
}
#endif

#endif
