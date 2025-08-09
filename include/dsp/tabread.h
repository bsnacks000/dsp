/**
 * @brief tabread. Multipurpose table readers. Current impl is based on
 * puredata's simple tabread family of objects which zero clip past boundaries. These
 * reader's are basically just interpolating indexes into the wavetable buffer.
 *
 * They should be controlled by a signal scaled to the range of the wavetable len and
 * can be used to implement waveshaper transfer functions, samplers and other synthesis
 * methods that rely on having fine grained read pointer control.
 *
 */

#ifndef DSP_TABREAD_H
#define DSP_TABREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/wavetable/wavetable.h>

/**
 * @brief tabread
 */
typedef struct {
    wavetable* wt;
} tabread;

/**
 * @brief init tabread.
 */
void tabread_init(tabread* self, wavetable* wt);

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

#ifdef __cplusplus
}
#endif

#endif
