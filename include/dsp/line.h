/**
 * @brief line - generate one segment of a line for dur seconds.
 *
 */

#ifndef DSP_LINE_H
#define DSP_LINE_H

#ifdef __cplusplus
extern "C" {
#endif

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
void line_tick_block(line* self, float* out, uint32_t nsmps);

typedef enum {
    ARLINE_OFF = 0,
    ARLINE_ATK,
    ARLINE_REL,
} arline_stage;

typedef struct {
    float gate_thresh, start_level, atk_sec, atk_level, rel_sec, rel_level, sr;
    float curr_gate_, prev_gate_;
    line state_;
    arline_stage stage_;
} arline;

void arline_init(arline* self,
                 float gate_thresh,
                 float start_level,
                 float atk_sec,
                 float atk_level,
                 float rel_sec,
                 float rel_level,
                 float sr);

void arline_tick_block(arline* self,
                       float* out,
                       float* gate,
                       float* gate_thresh,
                       float* start_level,
                       float* atk_sec,
                       float* atk_level,
                       float* rel_sec,
                       float* rel_level,
                       uint32_t nsmps);

#ifdef __cplusplus
}
#endif

#endif
