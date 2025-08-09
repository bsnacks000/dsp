
#include <dsp/conversions.h>
#include <dsp/delay.h>
#include <dsp/maths.h>
#include <dsp/utils.h>
#include <stdint.h>
#include "csdl.h"

#include "ddelay.h"

int ddelay2_init(CSOUND* csound, ddelay2* obj) {
    float delmax_ms = *obj->i_delmax_sec * 1000.0;

    float sr = GetLocalSr(&obj->h);
    size_t nsmps = GetLocalKsmps(&obj->h);

    obj->nsmps_per_ms = samps_per_ms(sr);
    size_t buf_sz = delay_line_calculate_buf_sz(sr, delmax_ms);

    csound->AuxAlloc(csound, buf_sz * sizeof(MYFLT), &obj->buf);
    if (obj->buf.auxp == NULL) {
        return csound->InitError(csound, "AuxAlloc failed: buf\n");
    }

    csound->AuxAlloc(csound, nsmps * sizeof(MYFLT), &obj->yn);
    if (obj->yn.auxp == NULL) {
        return csound->InitError(csound, "AuxAlloc failed: yn\n");
    }

    csound->AuxAlloc(csound, nsmps * sizeof(MYFLT), &obj->dn);
    if (obj->dn.auxp == NULL) {
        return csound->InitError(csound, "AuxAlloc failed: dn\n");
    }

    obj->buf_sz = buf_sz;
    delay_line_init(&obj->dl, (float*) obj->buf.auxp, buf_sz);

    return OK;
}

int ddelay2_vector(CSOUND* csound, ddelay2* obj) {
    (void) csound;
    uint32_t nsmps = GetLocalKsmps(&obj->h);

    float* ms = obj->a_ms;
    float* fb = obj->a_fb;

    // intermediate buffers
    float* yn = obj->yn.auxp;
    float* dn = obj->dn.auxp;

    // float del_smps = ms * obj->nsmps_per_ms;
    scale(ms, ms, obj->nsmps_per_ms, nsmps);

    delay_line_advance(&obj->dl, nsmps);

    delay_line_tap3(&obj->dl, yn, ms, nsmps);

    // feedback .. dn = in + fb * yn;
    mult_block(dn, yn, fb, nsmps);
    add_block(dn, dn, obj->a_in, nsmps);

    delay_line_write(&obj->dl, dn, nsmps);
    copy_nsmps(obj->a_out, yn, nsmps);

    return OK;
}
