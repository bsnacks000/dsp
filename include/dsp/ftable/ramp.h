/**
 * @brief simple ramp function generators
 *  - modeled on numpy *space functions.
 */
#ifndef DSP_WT_RAMP_H
#define DSP_WT_RAMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/ftable/ftable.h>
#include <dsp/utils.h>
#include <stdint.h>

/**
 * @brief ramp args. If endpoint is true, stop is the last sample. Otherwise, it is not
 * included.
 */
typedef struct {
    float start, stop;
    bool endpoint;
} wt_ramp_args;

/**
 * @brief generates a list of evenly spaced numbers over a specified interval
 *  - requires a pow2 ftable.
 */
dsp_err wt_linspace(ftable* wt, void* args);

/**
 * @brief generates a list of numbers spaced evenly on a log scale (a geometric
 * progression). Start and stop should be positive and > 0.0
 *  - A tolerance is added to avoid 0.0 in case it is given.
 *  - Negative start and stop sign will be flipped.
 */
dsp_err wt_geomspace(ftable* wt, void* args);

#ifdef __cplusplus
}
#endif

#endif
