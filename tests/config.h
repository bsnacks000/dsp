#ifndef DSP_TEST_CONFIG
#define DSP_TEST_CONFIG

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>

// global k block size to test tick_block
#define TEST_KBLK_SZ 64

/**
 * @brief zero out a float array
 */
static inline void zero(float* buf, size_t sz) {
    memset(buf, 0, sizeof(float) * sz);
}

#ifdef __cplusplus
}
#endif

#endif
