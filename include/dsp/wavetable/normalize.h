#ifndef DSP_WT_NORMALIZE_H
#define DSP_WT_NORMALIZE_H

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
        if (buf[i] > max) {
            max = buf[i];
        }
    }
    if (DSP_LIKELY(max)) {
        for (uint32_t i = 0; i < buf_sz; i++) {
            buf[i] /= max;
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif
