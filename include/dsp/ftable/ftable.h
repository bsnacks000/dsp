/**
 * @brief ftable utilities
 */
#ifndef DSP_ftable_H
#define DSP_ftable_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/utils.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief A general purpose ftable. The buf_sz **should always include +2 slots** (8
 * bytes) for 2 guard points to support linear / cubic interpolation. buf_sz -2 is
 * stored in len and should be used in any linear / cubic to handle guardpoint. Using
 * ftable_fill will guarantee the guard points are set on the `buf_sz-1` and
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
} ftable;

/**
 * @brief initialize a ftable. buf size should be desired size + 2. Eg. 1024 -> 1026.
 * The len and mask are calculated from buf_sz. The `is_pow2` flag is also set.
 */
void ftable_init(ftable* self, float* buf, uint32_t buf_sz);

/**
 * @brief an interface that fills a ftable buffer.
 */
typedef dsp_err (*ft_f)(ftable* wt, void* args);

/**
 * @brief write nsmps at offset directly into the ftable buffer
 * from an external buffer. Does not respect guard point (last 2 indices of buf)
 */
void ftable_write(ftable* self, float* in, uint32_t nsmps, uint32_t offset);

/**
 * @brief add the guard point wrap around manually. This is automatically called if
 * using the ftable_func API.
 */
void ftable_add_guard_point(ftable* self);

/**
 * @brief runs the ft_f over the entire ftable, potentially filling or updating the
 * ftable buffer depending on the function. Propagates dsp_err from ft_f. Will
 * automatically add the guard points as a convenience.
 */
dsp_err ftable_func(ftable* self, ft_f f, void* args);

/**
 * @brief Safely copy other into self. Runtime check for equal buffer size.
 */
dsp_err ftable_shallow_copy(ftable* self, ftable* other);

/**
 * @brief Deep copy other into self. Runtime check for equal buffer size.
 */
dsp_err ftable_deep_copy(ftable* self, ftable* other);

/**
 * @brief Zero a ftable buffer.
 */
void ftable_zero_buf(ftable* self);

#ifdef __cplusplus
}
#endif

#endif
