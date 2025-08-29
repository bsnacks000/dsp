/**
 * @brief wavetable utilities
 */
#ifndef DSP_WAVETABLE_H
#define DSP_WAVETABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/utils.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief A general purpose wavetable. The buf_sz **should always include +2 slots** (8
 * bytes) for 2 guard points to support linear / cubic interpolation. buf_sz -2 is
 * stored in len and should be used in any linear / cubic to handle guardpoint. Using
 * wavetable_fill will guarantee the guard points are set on the `buf_sz-1` and
 * `buf_sz-2`. If len is pow2 then a bit mask is provided for algos that want to use
 * fast wrap around.
 *
 * f0 and sr are initialized to 0 and may be filled later by other utilities to help
 * provide neccessary metadata for band limiting or playback.
 */
typedef struct {
    float* buf;
    uint32_t buf_sz, len, mask;
    bool is_pow2;
    float f0, sr, nharms;  // set by utilities
} wavetable;

/**
 * @brief initialize a wavetable. buf size should be desired size + 2. Eg. 1024 -> 1026.
 * The len and mask are calculated from buf_sz. The `is_pow2` flag is also set.
 */
void wavetable_init(wavetable* self, float* buf, uint32_t buf_sz);

/**
 * @brief an interface that fills a wavetable buffer.
 */
typedef dsp_err (*wt_f)(wavetable* wt, void* args);

/**
 * @brief write nsmps at offset directly into the wavetable buffer
 * from an external buffer. Does not respect guard point (last 2 indices of buf)
 */
void wavetable_write(wavetable* self, float* in, uint32_t nsmps, uint32_t offset);

/**
 * @brief add the guard point wrap around manually. This is automatically called if
 * using the wavetable_func API.
 */
void wavetable_add_guard_point(wavetable* self);

/**
 * @brief runs the wt_f over the entire wavetable, potentially filling or updating the
 * wavetable buffer depending on the function. Propagates dsp_err from wt_f. Will
 * automatically add the guard points as a convenience.
 */
dsp_err wavetable_func(wavetable* self, wt_f f, void* args);

/**
 * @brief Safely copy other into self. Runtime check for equal buffer size.
 */
dsp_err wavetable_shallow_copy(wavetable* self, wavetable* other);

/**
 * @brief Deep copy other into self. Runtime check for equal buffer size.
 */
dsp_err wavetable_deep_copy(wavetable* self, wavetable* other);

#ifdef __cplusplus
}
#endif

#endif
