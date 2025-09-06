/**
 * @brief crossfade primitives
 */
#ifndef DSP_XFADE_H
#define DSP_XFADE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/constants.h>
#include <dsp/shape.h>
#include <math.h>
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
    float t = clamp(position, 0.0, 1.0);

    // TODO: fast cosine impl
    float left = cosf(t * DSP_PI * 0.5);
    float right = sinf(t * DSP_PI * 0.5);

    xfade_pair out = {.left = left, .right = right};
    return out;
}

/**
 * @brief xfade state.
 */
typedef struct {
    float position;
    // private
    float l_coeff_, r_coeff_, last_;
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
                      uint32_t nsmps);

#ifdef __cplusplus
}
#endif

#endif
