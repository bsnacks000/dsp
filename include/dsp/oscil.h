/**
 * @brief fast (pow2) ftable oscillators
 */

#ifndef DSP_OSCIL_H
#define DSP_OSCIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/ftable.h>
#include <dsp/utils.h>

#include <stdint.h>

/**
 * @brief oscil state. This object is generic over its tick method. The provided
 * ftable must be tuned with the appropriate guard points to be able to use
 * the linear and cubic methods.
 */
typedef struct {
    ftable* wt;
    float freq, phase, sr;
    // private
    double incr_, index_;
    uint32_t offset_;
} oscil;

/**
 * @brief Initialize oscil state. Checks ftable at runtime for pow2 len. phase is
 * normalized between 0-1.
 */
dsp_err oscil_init(oscil* self, ftable* wt, float freq, float phase, float sr);

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
                          float* phs,
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
                          float* phs,
                          uint32_t start,
                          uint32_t nsmps);

/**
 * @brief cross fading ftable oscil that uses frequency to perform xfade. Designed to
 * be used with a band limited deck.
 */
typedef struct {
    ft_deck* deck;
    oscil *l, *r;
    float freq, phase, sr, l_amp_, r_amp_;
} blxoscil;

/**
 * @brief init blxoscil state. The deck should be generated using the sinesum API.
 *
 * Runtime check for l and r to be configured with the same sample rate.
 */
dsp_err blxoscil_init(blxoscil* self,
                      ft_deck* deck,
                      oscil* l,
                      oscil* r,
                      float freq,
                      float phase);

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

/**
 * @brief General purpse xfade oscillator by position. Useful for creative spectral
 * morphing and crossfade synthesis
 */
typedef struct {
    ft_deck* deck;
    oscil *l, *r;
    float freq, pos, phase, sr, l_amp_, r_amp_;
} xoscil;

/**
 * @brief initialize a xoscil. Runtime check for l and r to be configured
 * with the same sample rate.
 */
dsp_err xoscil_init(xoscil* self,
                    ft_deck* deck,
                    oscil* l,
                    oscil* r,
                    float freq,
                    float pos,
                    float phase);

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

#ifdef __cplusplus
}
#endif

#endif
