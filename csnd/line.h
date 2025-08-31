#ifndef DSP_CSND_LINE_H
#define DSP_CSND_LINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <csdl.h>
#include <dsp/line.h>

typedef struct {
    OPDS h;
    MYFLT *a_out, *i_start, *i_dur, *i_end;
    line l;
} lline;

int lline_init(CSOUND* csound, lline* obj);

int lline_vector(CSOUND* csound, lline* obj);

#ifdef __cplusplus
}
#endif

#endif
