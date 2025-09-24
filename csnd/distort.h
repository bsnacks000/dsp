#ifndef DSP_CSND_DISTORT_H
#define DSP_CSND_DISTORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <csdl.h>
#include <dsp/balance.h>
#include <dsp/tabread.h>
#include <dsp/wavetable/wavetable.h>
#include <dsp/xfade.h>

/**
 * @brief chebyshev polynomial saw generator.
 */
typedef struct {
    OPDS h;
    MYFLT *a_out, *a_in;

    tabread tr;  // read cheby table
} chebsaw;

int chebsaw_tab_init(CSOUND* csound);
int chebsaw_init(CSOUND* csound, chebsaw* obj);
int chebsaw_vector(CSOUND* csound, chebsaw* obj);

#ifdef __cplusplus
}
#endif

#endif
