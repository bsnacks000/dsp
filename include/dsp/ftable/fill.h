/**
 * @brief Fill a ftable buffer with samples.
 *  - Differences with ftable_write
 *      - nsmps must equal wt->len
 *      - the entire buffer is filled using copy_nsmps (memcpy).
 */

#ifndef DSP_WT_FILL_H
#define DSP_WT_FILL_H

#include "dsp/utils.h"
#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/utils.h>

#include <dsp/ftable/ftable.h>
#include <stdint.h>

/**
 * @brief wt_fill args.
 */
typedef struct {
    const float* in;
    uint32_t nsmps;
} wt_fill_args;

/**
 * @brief fill nsmps if nsmps must equal wt->len.
 */
dsp_err wt_fill(ftable* wt, void* args);

/**
 * @brief fill nsmps and normalize if nsmps must equal wt->len.
 */
dsp_err wt_fill_normalize(ftable* wt, void* args);

#ifdef __cplusplus
}
#endif

#endif
