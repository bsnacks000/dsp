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
    uint32_t nsmps_;                // counter
    bool finished_, level_, step_;  // state
} line;

/**
 * @brief line init.
 */
void line_init(line* self, float start, float stop, float dur_sec, float sr);

/**
 * @brief line tick block.
 */
void line_tick_block(line* self, float* out, uint32_t nsmps);

#ifdef __cplusplus
}
#endif

#endif
