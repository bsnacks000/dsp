/**
 * @brief fast (pow2) ftable oscillators
 */

#ifndef DSP_OSCIL_H
#define DSP_OSCIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/matrix.h>
#include <stdint.h>

typedef struct {
    float* wt;
    float freq, phase, sr;

    double incr_, index_;
    uint32_t wt_sz, wt_len_, mask_;
} oscil;

void oscil_init(oscil* self,
                float* wt,
                uint32_t wt_sz,
                float freq,
                float phase,
                float sr);

/**
 * @brief truncating oscil. guard point = 0
 */
void osciln_tick_block(oscil* self,
                       float* out,
                       float* freq,
                       uint32_t start,
                       uint32_t nsmps);

/**
 * @brief linear interpolating oscil. guard point = 1
 */
void oscili_tick_block(oscil* self,
                       float* out,
                       float* freq,
                       uint32_t start,
                       uint32_t nsmps);

/**
 * @brief linear interpolating oscil that allows phase mod.
 */
void oscili_pm_tick_block(oscil* self,
                          float* out,
                          float* freq,
                          float* mod,
                          uint32_t start,
                          uint32_t nsmps);

/**
 * @brief cubic interpolating oscil. guard point = 2
 */
void oscil3_tick_block(oscil* self,
                       float* out,
                       float* freq,
                       uint32_t start,
                       uint32_t nsmps);

/**
 * @brief cubic interpolating oscil that allows phase mod.
 */
void oscil3_pm_tick_block(oscil* self,
                          float* out,
                          float* freq,
                          float* mod,
                          uint32_t start,
                          uint32_t nsmps);

typedef struct {
    matrix* deck;
    oscil *l, *r;
    float freq, pos, phase, sr, l_amp_, r_amp_;
} xoscil;

/**
 * @brief initialize a xoscil.
 */
void xoscil_init(xoscil* self,
                 matrix* deck,
                 oscil* l,
                 oscil* r,
                 float freq,
                 float pos,
                 float phase,
                 float sr);

void xoscili_tick_block(xoscil* self,
                        float* out,
                        float* freq,
                        float* pos,
                        uint32_t start,
                        uint32_t nsmps);

void xoscil3_tick_block(xoscil* self,
                        float* out,
                        float* freq,
                        float* pos,
                        uint32_t start,
                        uint32_t nsmps);

/**
 * @brief cross fading ftable oscil that uses frequency to perform xfade. Designed to
 * be used with a band limited deck.
 */
typedef struct {
    matrix* deck;
    float* f0;  // max fundamental per row
    oscil *l, *r;
    float freq, phase, sr, l_amp_, r_amp_;
} blxoscil;

/**
 * @brief init blxoscil state. The deck should be generated using the sinesum API.
 *
 * Runtime check for l and r to be configured with the same sample rate.
 */
void blxoscil_init(blxoscil* self,
                   matrix* deck,
                   oscil* l,
                   oscil* r,
                   float* f0,
                   float freq,
                   float phase,
                   float sr);

/**
 * @brief linear interpolating blxoscil.
 */
void blxoscili_tick_block(blxoscil* self,
                          float* out,
                          float* freq,
                          uint32_t start,
                          uint32_t nsmps);

/**
 * @brief cubic interpolating blxoscil.
 */
void blxoscil3_tick_block(blxoscil* self,
                          float* out,
                          float* freq,
                          uint32_t start,
                          uint32_t nsmps);
#ifdef __cplusplus
}
#endif

#endif
