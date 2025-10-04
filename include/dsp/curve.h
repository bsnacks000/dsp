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

#include <dsp/utils.h>
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

/**
 * @brief curve AR envelope. Similar to `line_ar` but each segment has a curve factor.
 */
typedef struct {
    float gate_thresh, start_level, atk_sec, atk_crv, atk_level, rel_sec, rel_crv,
        rel_level, sr;
    float curr_gate_, prev_gate_;
    curve state_;
    ar_stage stage_;
} curve_ar;

/**
 * @brief init linear AR envelope.
 */
void curve_ar_init(curve_ar* self,
                   float gate_thresh,
                   float start_level,
                   float atk_sec,
                   float atk_crv,
                   float atk_level,
                   float rel_sec,
                   float rel_crv,
                   float rel_level,
                   float sr);

/**
 * @brief tick one block of the linear AR envelope. The envelope fires when the
 *  gate rises above the gate threshold. If the gate falls below and then rises
 *  again before the envelope is finished it will reset. The envelope only grabs
 *  the instanteous values of the signals at stage transitions.
 *
 *  For typical AR envelopes the start and release segments should be the same.
 */
void curve_ar_tick_block(curve_ar* self,
                         float* out,
                         float* gate,
                         float* gate_thresh,
                         float* start_level,
                         float* atk_sec,
                         float* atk_crv,
                         float* atk_level,
                         float* rel_sec,
                         float* rel_crv,
                         float* rel_level,
                         uint32_t nsmps);

/**
 * @brief curve ADSR envelope. Similar to `line_adsr` but each segment has a curve
 * factor.
 */
typedef struct {
    float gate_thresh, start_level, atk_sec, atk_crv, atk_level, dcy_sec, dcy_crv,
        sustain_level, rel_sec, rel_crv, rel_level, sr;

    float curr_gate_, prev_gate_;
    curve state_;
    adsr_stage stage_;
} curve_adsr;

/**
 * @brief initialize a curve ADSR envelope.
 */
void curve_adsr_init(curve_adsr* self,
                     float gate_thresh,
                     float start_level,
                     float atk_sec,
                     float atk_crv,
                     float atk_level,
                     float dcy_sec,
                     float dcy_crv,
                     float sustain_level,
                     float rel_sec,
                     float rel_crv,
                     float rel_level,
                     float sr);

/**
 * @brief tick one block of the curve ADSR envelope. The envelope fires when the
 *  gate is above the threshold and runs through atk/dec stages, holding at sustain
 *  level. When the gate is released it will move directly to the release stage. It
 *  will wait on the final release level until retriggered.
 *
 *  The envelope only grabs the instanteous values of the signals at stage transitions.
 *
 *  For typical envelopes the start and release levels should be the same.
 */
void curve_adsr_tick_block(curve_adsr* self,
                           float* out,
                           float* gate,
                           float* gate_thresh,
                           float* start_level,
                           float* atk_sec,
                           float* atk_crv,
                           float* atk_level,
                           float* dcy_sec,
                           float* dcy_crv,
                           float* sustain_level,
                           float* rel_sec,
                           float* rel_crv,
                           float* rel_level,
                           uint32_t nsmps);

#ifdef __cplusplus
}
#endif

#endif
