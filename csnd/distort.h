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
 * @brief simple distortion based on a 3rd degree chebyshev polynomial.
 *  - amt is a dry/wet control.
 *  - drive is pre gain
 *  - table is constructed once at startup
 */
typedef struct {
    OPDS h;
    MYFLT *a_out, *a_in, *a_amt;

    tabread tr;  // read cheby table
} cheby3;

int cheby3_tab_init(CSOUND* csound);
int cheby3_init(CSOUND* csound, cheby3* obj);
int cheby3_vector(CSOUND* csound, cheby3* obj);

#ifdef __cplusplus
}
#endif

#endif
