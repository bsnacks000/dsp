#include "line.h"
#include "csdl.h"
#include "dsp/line.h"

int lline_init(CSOUND* csound, lline* obj) {
    (void) csound;
    MYFLT sr = GetLocalSr(&obj->h);

    csound->Message(csound, "i_start: %f\n", *obj->i_start);
    csound->Message(csound, "i_end: %f\n", *obj->i_end);
    csound->Message(csound, "i_dur: %f\n", *obj->i_dur);

    line_init(&obj->l, *obj->i_start, *obj->i_end, *obj->i_dur, sr);
    return OK;
}

int lline_vector(CSOUND* csound, lline* obj) {
    (void) csound;
    uint32_t nsmps = GetLocalKsmps(&obj->h);

    line_tick_block(&obj->l, obj->a_out, nsmps);
    return OK;
}
