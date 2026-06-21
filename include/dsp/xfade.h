/**
 * @brief crossfade primitives
 */
#ifndef DSP_XFADE_H
#define DSP_XFADE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/interpolate.h>
#include <dsp/maths.h>

#include <stdint.h>

/**
 * A left/right pair return value.
 */
typedef struct {
    float left;
    float right;
} xfade_pair;

/**
 * @brief equal power crossfade using sine/cos functions. Position should
 * be 0-1 (initial 0.5)
 *
 */
static inline xfade_pair xfade_sin(float position) {

    // NOTE: changed from original impl .. see py src
    float t = clamp(position, 0.0f, 1.0f);
    float a = t * DSP_PI_F * 0.5f;

    float left = fast_qcosf(a);
    float right = fast_qsinf(a);

    xfade_pair out = {.left = left, .right = right};
    return out;
}

/**
 * @brief given a position and deck_sz calculate a cross fade value over a given
 * frame/band pair.
 */
static inline xfade_pair xfade_from_pos(float pos, uint32_t deck_sz) {
    pos = clamp(pos, 0.0, 1.0);

    float scaled = pos * (float) (deck_sz - 1);
    float fader = scaled - floorf(scaled);
    return xfade_sin(fader);
}

/**
 * @brief query the l/r amplitudes for use with xfade given freq for bandlimited
 * crossfades.
 */
static inline xfade_pair xfade_from_freq(float freq, float lo, float hi) {
    if (freq < lo) {
        return (xfade_pair) {.left = 1.0, .right = 0.0};
    }

    if (freq > hi) {
        return (xfade_pair) {.left = 0.0, .right = 1.0};
    }

    float fader = linlin(freq, lo, hi, 0.0, 1.0);
    return xfade_sin(fader);
}

/**
 * @brief xfade state.
 */
typedef struct {
    float position;
    // private
    float l_coeff_, r_coeff_;
} xfade;

/**
 * @brief init xfade state. Position (0-1)
 */
void xfade_init(xfade* self, float position);

/**
 * @brief tick a block of xfade_sin. Position (0-1)
 */
void xfade_tick_block(xfade* self,
                      float* out,
                      float* in_l,
                      float* in_r,
                      float* position,
                      uint32_t start,
                      uint32_t nsmps);

#ifdef __cplusplus
}
#endif

#endif
