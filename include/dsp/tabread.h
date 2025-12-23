/**
 * @brief tabread. Multipurpose table readers. Current impl is based on
 * puredata's simple tabread family of objects which zero clip past boundaries. These
 * reader's are basically just interpolating indexes into the ftable buffer.
 *
 * They should be controlled by a signal scaled to the range of the ftable len and
 * can be used to implement waveshaper transfer functions, samplers and other synthesis
 * methods that rely on having fine grained read pointer control.
 *
 */

#ifndef DSP_TABREAD_H
#define DSP_TABREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/ftable/deck.h>
#include <dsp/ftable/ftable.h>

/**
 * @brief tabread
 */
typedef struct {
    ftable* wt;
} tabread;

/**
 * @brief init tabread.
 */
void tabread_init(tabread* self, ftable* wt);

/**
 * @brief truncating tabread.
 */
void tabreadn_tick_block(tabread* self, float* out, float* idx, uint32_t nsmps);

/**
 * @brief linear interpolating tabread.
 */
void tabreadi_tick_block(tabread* self, float* out, float* idx, uint32_t nsmps);

/**
 * @brief cubic interpolating tabread.
 */
void tabread3_tick_block(tabread* self, float* out, float* idx, uint32_t nsmps);

/**
 * @brief xfade tabreader.
 */
typedef struct {
    wt_deck* deck;
    tabread *l, *r;
    float pos, l_amp_, r_amp_;
} xtabread;

/**
 * @brief init xtabread.
 */
void xtabread_init(xtabread* self, wt_deck* deck, tabread* l, tabread* r, float pos);

/**
 * @brief linear interpolating xtabread
 */
void xtabreadi_tick_block(xtabread* self,
                          float* out,
                          float* idx,
                          float* pos,
                          uint32_t nsmps);

/**
 * @brief cubic interpolating xtabread
 */
void xtabread3_tick_block(xtabread* self,
                          float* out,
                          float* idx,
                          float* pos,
                          uint32_t nsmps);

#ifdef __cplusplus
}
#endif

#endif
