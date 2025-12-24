/**
 * @brief windowing maths. Based on csound gen20.
 *  - wt should be pow2
 *
 */

#ifndef DSP_ft_WINDOW_H
#define DSP_ft_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/ftable/ftable.h>

typedef enum {
    WINDOW_HAMMING,
    WINDOW_HANNING,
    WINDOW_BLACKMAN,
    WINDOW_BLACKMAN_HARRIS,
    WINDOW_GAUSS,
    WINDOW_SINC
} ft_window_type;

typedef struct {
    ft_window_type type;
} ft_window_args;

dsp_err ft_window(ftable* wt, void* args);

#ifdef __cplusplus
}
#endif

#endif
