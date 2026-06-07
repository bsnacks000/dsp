/**
 * @brief line - generate one segment of a line for dur seconds. This can create
 * a basic line similar to pure data/csound/sc3 and other dsp platforms.
 *
 * The sampi object is based on csound and Dodge/Jerse and can be used to create
 * a RANDI when set up appropriately.
 *
 */

#ifndef DSP_LINE_H
#define DSP_LINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/utils.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    // public
    float start, stop, dur_sec, sr;
    // private
    bool finished_;
    uint32_t nsmps_;      // counter
    float level_, step_;  // state
} line;

/**
 * @brief line init.
 */
void line_init(line* self, float start, float stop, float dur_sec, float sr);

/**
 * @brief line tick block.
 */
void line_tick_block(line* self, float* out, uint32_t start, uint32_t nsmps);

/**
 * @brief sample and linearly interpolate. Influenced by csound's randi and based
 * on Dodge and Jerse RANDI.
 */
typedef struct {
    float start, stop, dur_sec, gate_thresh, sr;
    float prev_gate_, curr_gate_, curr_dur_, curr_out_;
    line state_;
} sampi;

/**
 * @brief sampi_init - initial line trajectory. Internally this uses a line to
 * object to interpolate.
 */
void sampi_init(sampi* self, float start, float stop, float dur_sec, float sr);

/*
 * @brief tick one block of sampi. Instanteous values of `in` and `dur_sec` are
 * captured using a gate. The gate threshold may also be modulated.
 *
 * If using random signals the effects would create the Dodge/Jerse
 *
 * Note that sampi does not clamp, normalize or "finish" the line segments, instead
 * creating a rolling trajectory similar to a csound line opcode. Do the results may go
 * out of a desired range depending on the speed of the gate and moving in/dur signals.
 */
void sampi_tick_block(sampi* self,
                      float* out,
                      float* in,
                      float* dur_sec,
                      float* gate,
                      float* gate_thresh,
                      uint32_t start,
                      uint32_t nsmps);

/**
 * @brief state for a linear AR envelope.
 */
typedef struct {
    float gate_thresh, start_level, atk_sec, atk_level, rel_sec, rel_level, sr;
    float curr_gate_, prev_gate_;
    line state_;
    ar_stage stage_;
} line_ar;

/**
 * @brief init linear AR envelope.
 */
void line_ar_init(line_ar* self,
                  float gate_thresh,
                  float start_level,
                  float atk_sec,
                  float atk_level,
                  float rel_sec,
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
void line_ar_tick_block(line_ar* self,
                        float* out,
                        float* gate,
                        float* gate_thresh,
                        float* start_level,
                        float* atk_sec,
                        float* atk_level,
                        float* rel_sec,
                        float* rel_level,
                        uint32_t start,
                        uint32_t nsmps);

/**
 * @brief state for a linear ADSR envelope.
 */
typedef struct {
    float gate_thresh, start_level, atk_sec, atk_level, dcy_sec, sustain_level, rel_sec,
        rel_level, sr;

    float curr_gate_, prev_gate_;
    line state_;
    adsr_stage stage_;

} line_adsr;

/**
 * @brief initialize a linear ADSR envelope.
 */
void line_adsr_init(line_adsr* self,
                    float gate_thresh,
                    float start_level,
                    float atk_sec,
                    float atk_level,
                    float dcy_sec,
                    float sustain_level,
                    float rel_sec,
                    float rel_level,
                    float sr);

/**
 * @brief tick one block of the linear ADSR envelope. The envelope fires when the
 *  gate is above the threshold and runs through atk/dec stages, holding at sustain
 *  level. When the gate is released it will move directly to the release stage. It
 *  will wait on the final release level until retriggered.
 *
 *  The envelope only grabs the instanteous values of the signals at stage transitions.
 *
 *  For typical envelopes the start and release levels should be the same.
 */
void line_adsr_tick_block(line_adsr* self,
                          float* out,
                          float* gate,
                          float* gate_thresh,
                          float* start_level,
                          float* atk_sec,
                          float* atk_level,
                          float* dcy_sec,
                          float* sustain_level,
                          float* rel_sec,
                          float* rel_level,
                          uint32_t start,
                          uint32_t nsmps);

#ifdef __cplusplus
}
#endif

#endif
