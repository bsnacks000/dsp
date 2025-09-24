#ifndef DSP_WT_NORMALIZE_H
#define DSP_WT_NORMALIZE_H

#include <math.h>
#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/utils.h>
#include <stdint.h>

/**
 * @brief normalize the values in buf
 */
static inline void normalize(float* buf, uint32_t buf_sz) {
    float max = 0.0;
    for (uint32_t i = 0; i < buf_sz; i++) {
        float a = fabs(buf[i]);  // use absolute value (handle bipolar signals)
        if (a > max) {
            max = a;
        }
    }
    if (DSP_LIKELY(max)) {
        float s = 1.0 / max;
        for (uint32_t i = 0; i < buf_sz; i++) {
            buf[i] *= s;
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
