/**
 * @ brief curve - generate a segment for the specified dur with a curve factor
 *  - A curve factor of 0.0 is calculated as a straight line.
 *  - Negative and positive indicate log/exp shape depending on direction.
 *  - based on sc3 Env
 */

#ifndef DSP_CURVE_H
#define DSP_CURVE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    // public
    float start, stop, dur_sec, factor, sr;
    // private
    double level_, a1_, a2_, b1_, grow_;  // state
    uint32_t nsmps_;                      // counter
    bool finished_;

} curve;

/**
 * @brief curve init.
 */
void curve_init(curve* self,
                float start,
                float stop,
                float dur_sec,
                float factor,
                float sr);

/**
 * @brief curve tick block.
 */
void curve_tick_block(curve* self, float* out, uint32_t nsmps);

#ifdef __cplusplus
}
#endif

#endif
