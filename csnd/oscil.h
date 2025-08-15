#ifndef DSP_CSND_OSCIL_H
#define DSP_CSND_OSCIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <csdl.h>

#include <dsp/bq.h>
#include <dsp/oscil.h>
#include <dsp/wavetable/wavetable.h>

typedef struct {
    OPDS h;
    MYFLT *a_out, *a_freq, *i_phase, *i_fn;  // a-aii
    FUNC* ftp;                               // the ftable pointer
    AUXCH aux;                               // hold our own copy of ftable
    wavetable wt;
    oscil state;
} ftoscil3;

int ftoscil3_init(CSOUND* csound, ftoscil3* obj);
int ftoscil3_vector(CSOUND* csound, ftoscil3* obj);

#define OFTOSCIL_FILTER_N 8

// extension of the above but using oversampling
// provided with i_osrate;
typedef struct {
    OPDS h;
    MYFLT *a_out, *a_freq, *i_phase, *i_fn, *i_osfactor;  // a-aiio (optional irate)
    FUNC* ftp;                                            // the ftable pointer
    AUXCH ft;                                             // hold our own copy of ftable
    AUXCH os_out_buf;                                     // oversamp output buffer
    AUXCH os_freq_buf;                                    // oversamp freq buffer
    size_t os_buf_sz;
    bq_non_resonant bank[OFTOSCIL_FILTER_N];  // anti-aliasing bank
    float os_fc;                              // calculated decimate cutoff
    wavetable wt;
    oscil state;
    // bq_non_resonant smooth;  // smooth freq signal
    float prev;
} oftoscil3;

int oftoscil3_init(CSOUND* csound, oftoscil3* obj);
int oftoscil3_vector(CSOUND* csound, oftoscil3* obj);

/**
 * @brief band limited saw impl
 */
typedef struct {
    OPDS h;
    MYFLT *a_out, *a_freq, *i_phase;

    // mem block
    AUXCH mem;
    size_t mem_sz;

    // deck components
    wavetable** frames;
    size_t frames_sz;
    wt_deck deck;

    // blxoscil components
    oscil left;
    oscil right;
    blxoscil saw;

} blsaw;

int blsaw_init(CSOUND* csound, blsaw* obj);
int blsaw_vector(CSOUND* csound, blsaw* obj);

#ifdef __cplusplus
}
#endif

#endif
