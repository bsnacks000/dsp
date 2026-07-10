/**
 * @file oscil.h
 * @brief fast (pow2) table oscillators.
 *
 * These are mainly influenced by Csound + Lazzarinni/Dobson: APB implementations as
 * well as Pirkle: Synth plugins.
 *
 * The cubic implementation is closer to what you would find in Pure Data and we enforce
 * that regardless of the interpolation strategy that initializers expect a pow2 + 2
 * table.
 *
 * The xfade oscillator designs were mainly inspired by Serum and various series on
 * the Ear Level Engineering blog. Extra tooling is provided in other modules in the lib
 * to accomodate constructing interpolated and band limited decks. (See matrix.h,
 * sinesum.h and interpolate.h for tooling and test suite for simple example setups)
 *
 * Across the board wavetable buffers should be initialized as a pow2 + 2 for use and
 * its assumed that they are.
 *
 * If using these oscils as a one shot the wt function should be calculated
 * out to the full pow2 + 2 so that it runs to the end.
 *
 * If using as an interpolating oscillator (most cases) we should calculate only the
 * first pow2 values and then copy the first and second indices into the last two to
 * assure proper continuity on wrap around.
 *
 *
 * Ex.
 * (x[0], x[1], x[2], ... x[1023], x[0], x[1])        <---- wrapped!
 * (x[0], x[1], x[2], ... x[1023], x[1024], x[1025])  <---- one shot!
 *
 */

// SPDX-License-Identifier: MIT

#ifndef DSP_OSCIL_H
#define DSP_OSCIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/matrix.h>
#include <stdint.h>

/**
 * @brief oscil state.
 * Internally wt_len is calculated as the wt_sz - 2 and mask
 * as wt_len - 1 to assure proper wrapping.
 *
 * */
typedef struct {
    float* wt;
    float freq, iphs, sr;

    double incr_, index_;
    uint32_t wt_sz, wt_len_, mask_;
} oscil;

/**
 * @brief initialize oscil state.
 * @note wt_sz must be a pow2 + 2 with a cubic guard point wrapper on
 * the last two indices in order to cover interpolation.
 *
 * */
void oscil_init(oscil* self,
                float* wt,
                uint32_t wt_sz,
                float freq,
                float iphs,
                float sr);

/**
 * @brief truncating oscil. This is the only oscillator that does
 * not use the guard point and will truncate to the nearest index.
 * Useful in some contexts as an LFO or step sequencer though for most cases
 * we should be using one of the interpolated oscils.
 */
void osciln_tick_block(oscil* self,
                       float* out,
                       float* freq,
                       uint32_t start,
                       uint32_t nsmps);

/**
 * @brief linear interpolating oscil. This is uses only 1 slot of the guard point
 * as wrapper.
 */
void oscili_tick_block(oscil* self,
                       float* out,
                       float* freq,
                       uint32_t start,
                       uint32_t nsmps);

/**
 * @brief linear interpolating oscil that allows phase mod.
 * @note mod is expected to be a normalized unipolar signal (0-1)
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
 * @note mod is expected to be a normalized unipolar signal (0-1)
 */
void oscil3_pm_tick_block(oscil* self,
                          float* out,
                          float* freq,
                          float* mod,
                          uint32_t start,
                          uint32_t nsmps);

/**
 * @brief A crossfading table oscillator. By modulating the pos parameter
 * between 0 and 1 the signal is crossfaded across tables in the matrix deck.
 *
 * */
typedef struct {
    matrix* deck;
    oscil *l, *r;
    float freq, pos, iphs, sr, l_amp_, r_amp_;
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
                 float iphs,
                 float sr);

/**
 * @brief linear interpolation.
 * @note pos should be normalized unipolar signal between (0-1)
 *
 * */
void xoscili_tick_block(xoscil* self,
                        float* out,
                        float* freq,
                        float* pos,
                        uint32_t start,
                        uint32_t nsmps);

/**
 * @brief cubic interpolation.
 * @note pos should be normalized unipolar signal between (0-1)
 *
 * */
void xoscil3_tick_block(xoscil* self,
                        float* out,
                        float* freq,
                        float* pos,
                        uint32_t start,
                        uint32_t nsmps);

/**
 * @brief xoscil that uses frequency to modulate a deck that is assumed to
 * contain a series of gradually band limited tables.
 */
typedef struct {
    matrix* deck;
    float* f0;  // max fundamental per row
    oscil *l, *r;
    float freq, iphs, sr, l_amp_, r_amp_;
} blxoscil;

/**
 * @brief init blxoscil state. Its implied that the harmonic weights decrease from
 * left to right. This lib provides various helpers
 *
 */
void blxoscil_init(blxoscil* self,
                   matrix* deck,
                   oscil* l,
                   oscil* r,
                   float* f0,
                   float freq,
                   float iphs,
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
