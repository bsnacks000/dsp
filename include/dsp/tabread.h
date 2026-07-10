/**
 * @file tabread.h
 * @brief tabread. Multipurpose table readers. Current impl is based on
 * puredata's simple tabread family of objects which zero clip past boundaries. These
 * reader's are basically just interpolating indexes into the wavetable buffer.
 *
 * A tabread is usually controlled by a phasor or some other signal
 * scaled to the range of the wt len to drive the index.
 *
 * Can be used to implement waveshaper transfer functions, samplers and other synthesis
 * methods that rely on having fine grained read pointer control.
 *
 * Note that unlike `oscil.h` we are not limited to pow2 sizes though
 * for interpolation we DO expect a +2 guardpoint applied to the table. This means
 * that for any buffer of size N tabread expects N[-2] and N[-1] to be the guardpoints.
 *
 * The xfade tabreader is similar to xoscil in that it takes a matrix deck and using
 * the pos signal can xfade between any number of tables.
 *
 * Based on numerous implementations:
 *   * tabread/tabread4 (pd)
 *   * table/tablei (csound)
 *   * Osc (sc3)
 */

// SPDX-License-Identifier: MIT

#ifndef DSP_TABREAD_H
#define DSP_TABREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <dsp/matrix.h>

/**
 * @brief tabread state. wt_len_ is always calculated as wt_sz - 2 and its expected
 * that if the user requires a guardpoint wrap around etc. that it is handled.
 */
typedef struct {
    float* wt;
    uint32_t wt_sz, wt_len_;
} tabread;

/**
 * @brief init tabread.
 */
void tabread_init(tabread* self, float* wt, uint32_t wt_sz);

/**
 * @brief truncating tabread.
 * @param out output signal
 * @param idx signal to drive the current index
 */
void tabreadn_tick_block(tabread* self,
                         float* out,
                         float* idx,
                         uint32_t start,
                         uint32_t nsmps);

/**
 * @brief linear interpolating tabread.
 * @param out output signal
 * @param idx signal to drive the current index
 */
void tabreadi_tick_block(tabread* self,
                         float* out,
                         float* idx,
                         uint32_t start,
                         uint32_t nsmps);

/**
 * @brief cubic interpolating tabread.
 * @param out output signal
 * @param idx signal to drive the current index.
 */
void tabread3_tick_block(tabread* self,
                         float* out,
                         float* idx,
                         uint32_t start,
                         uint32_t nsmps);

/**
 * @brief xfade tabreader.
 */
typedef struct {
    matrix* deck;
    tabread *l, *r;
    float pos, l_amp_, r_amp_;
} xtabread;

/**
 * @brief init xtabread. The init function handles will initialize tabread l/r to use
 * the matrix deck. pos should be initialized between 0 - 1.
 */
void xtabread_init(xtabread* self, matrix* deck, tabread* l, tabread* r, float pos);

/**
 * @brief linear interpolating xtabread
 * @param out output signal
 * @param idx signal to drive the current index
 * @param pos normalized unipolar signal to determine position within the wt deck.
 */
void xtabreadi_tick_block(xtabread* self,
                          float* out,
                          float* idx,
                          float* pos,
                          uint32_t start,
                          uint32_t nsmps);

/**
 * @brief cubic interpolating xtabread
 * @param out output signal
 * @param idx signal to drive the current index
 * @param pos normalized unipolar signal to determine position within the wt deck.
 *
 */
void xtabread3_tick_block(xtabread* self,
                          float* out,
                          float* idx,
                          float* pos,
                          uint32_t start,
                          uint32_t nsmps);

#ifdef __cplusplus
}
#endif

#endif
