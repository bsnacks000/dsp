#ifndef DSP_CSOUND_ENV_H
#define DSP_CSOUND_ENV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <csdl.h>
#include <dsp/line.h>
/**
 * @brief AR envelope
 */
typedef struct {
    OPDS h;
    MYFLT *a_out, *a_gate, *a_gate_thresh, *a_start_level, *a_atk_sec, *a_atk_level,
        *a_rel_sec, *a_rel_level;
    line_ar env;
} ar_line;

int ar_line_init(CSOUND* csound, ar_line* obj);

int ar_line_vector(CSOUND* csound, ar_line* obj);

/**
 * @brief ADSR envelope
 */
typedef struct {
    OPDS h;
    MYFLT *a_out, *a_gate, *a_gate_thresh, *a_start_level, *a_atk_sec, *a_atk_level,
        *a_dcy_sec, *a_sustain_level, *a_rel_sec, *a_rel_level;
    line_adsr env;
} adsr_line;

int adsr_line_init(CSOUND* csound, adsr_line* obj);

int adsr_line_vector(CSOUND* csound, adsr_line* obj);

#ifdef __cplusplus
}
#endif

#endif
