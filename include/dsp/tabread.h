/**
 * @brief tabread. Multipurpose table readers. Current impl is based on
 * puredata's simple tabread family of objects which zero clip past boundaries. These
 * reader's are basically just interpolating indexes into the ftable buffer.
 *
 * They should be controlled by a signal scaled to the range of the ftable len and
 * can be used to implement waveshaper transfer functions, samplers and other synthesis
 * methods that rely on having fine grained read pointer control.
 *
 * Note that unlike `oscil.h` we are not limited to pow2 sizes though
 * for interpolation we DO expect a +2 guardpoint applied to the table. This means
 * that for any buffer of size N tabread expects N[-2] and N[-1] to be the guardpoints.
 *
 *
 * Based on numerous implementations:
 *   * tabread/tabread4 (pd)
 *   * table/tablei (csound)
 *
 */

#ifndef DSP_TABREAD_H
#define DSP_TABREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <dsp/matrix.h>

/**
 * @brief tabread
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
 */
void tabreadn_tick_block(tabread* self,
                         float* out,
                         float* idx,
                         uint32_t start,
                         uint32_t nsmps);

/**
 * @brief linear interpolating tabread.
 */
void tabreadi_tick_block(tabread* self,
                         float* out,
                         float* idx,
                         uint32_t start,
                         uint32_t nsmps);

/**
 * @brief cubic interpolating tabread.
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
 * @brief init xtabread.
 */
void xtabread_init(xtabread* self, matrix* deck, tabread* l, tabread* r, float pos);

/**
 * @brief linear interpolating xtabread
 */
void xtabreadi_tick_block(xtabread* self,
                          float* out,
                          float* idx,
                          float* pos,
                          uint32_t start,
                          uint32_t nsmps);

/**
 * @brief cubic interpolating xtabread
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
