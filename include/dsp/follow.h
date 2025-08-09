/**
 * @brief full wave rectified (+/-) peak/rms envelope follower from Pirkle
 * (AudioDetector) which is based on Reiss (2011)
 */
#ifndef DSP_FOLLOW_H
#define DSP_FOLLOW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    float atk_ms, rel_ms, sr;
    // private
    float last_envelope_, atk_time_, rel_time_;
} follow;

void follow_init(follow* self, float atk_ms, float rel_ms, float sr);

void follow_rms_tick_block(follow* self,
                           float* out,
                           float* in,
                           float* atk_ms,
                           float* rel_ms,
                           uint32_t nsmps);

void follow_peak_tick_block(follow* self,
                            float* out,
                            float* in,
                            float* atk_ms,
                            float* rel_ms,
                            uint32_t nsmps);

#ifdef __cplusplus
}
#endif

#endif
