/**
 * @brief lag - smoothing filter
 *  - based sc3 Lag class
 */

#ifndef DSP_LAG_H
#define DSP_LAG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief lag state. t_sec is the time in seconds to converge to -60db of its target.
 */
typedef struct {
    float t_sec, sr;
    // private
    double alpha_, last_;
} lag;

/**
 * @brief lag init
 */
void lag_init(lag* self, float t_sec, float sr);

/**
 * @brief lag tick block. t is a signal of time in seconds to converge to -60db of the
 * input signal
 */
void lag_tick_block(lag* self,
                    float* out,
                    float* in,
                    float* t_sec,
                    uint32_t start,
                    uint32_t nsmps);

#ifdef __cplusplus
}
#endif

#endif
