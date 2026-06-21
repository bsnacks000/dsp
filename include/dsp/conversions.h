/**
 * @file conversions.h
 * @brief Various conversion maths.
 *
 */

#ifndef DSP_CONVERSIONS_H
#define DSP_CONVERSIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/assert.h>
#include <math.h>
#include <stdint.h>

/**
 * @brief Calculate the number of samples per millisecond given samplerate sr in samples
 *  per second.
 *
 */
static inline float samps_per_ms(float sr) {
    return sr / 1000.0f;
}

/**
 * @breif calculate the ms for nsmps
 */
static inline float ms_per_samps(float nsmps, float sr) {
    return nsmps / sr * 1000.0f;
}

/**
 * @brief Convert decibel gain to amplitude (linear)
 *
 */
static inline float db_to_amp(float db) {
    return powf(10.0f, db / 20.0f);
}

/**
 * @brief Convert decibel gain to power (linear)
 *
 */
static inline float db_to_power(float db) {
    return powf(10.0f, db / 10.0f);
}

/**
 * @brief Convert linear amplitude to decibels
 *
 */
static inline float amp_to_db(float lin) {
    dsp_assert(lin > 0.0f, "lin must be > 0.");
    return 20.0f * log10f(lin);
}

/**
 * @brief Convert linear power to decibels
 *
 */
static inline float power_to_db(float power) {
    dsp_assert(power > 0.0f, "power must be > 0.");
    return 10.0f * log10f(power);
}

/**
 * @brief calculate peak gain for q
 *
 */
static inline float peak_gain_for_q(float q) {
    if (q <= 0.707f)
        return 1.0f;
    return (q * q) / powf((q * q - 0.25f), 0.5f);
}

/**
 * @brief calculate peak gain for q and convert from linamp to db.
 *
 */
static inline float db_peak_gain_for_q(float q) {
    q = peak_gain_for_q(q);
    return amp_to_db(q);
}

/**
 * @brief convert unipolar to bipolar.
 *
 */
static inline float unipolar_to_bipolar(float val) {
    return 2.0f * val - 1.0f;
}

/**
 * @brief convert unipolar to bipolar.
 *
 */
static inline double unipolar_to_bipolar_d(double val) {
    return 2.0 * val - 1.0;
}

/**
 * @brief convert bipolar to unipolar.
 *
 */
static inline float bipolar_to_unipolar(float val) {
    return 0.5f * val + 0.5f;
}

/**
 * @brief midi / freq conversion:
 * - helpers cover freq / velocity / pitch bend conversions
 *
 * sender:
 *  - freq (Hz)
 *  - ftom(freq) -> midinote(float)
 *  - frac_to_pb(midinote) -> pitch_bend (int16_t)
 *  - midi_send((uint8_t)note, pitch_bend) --> wire boundary
 *
 * recv:
 *  - wire boundary --> midi_recv((uint8_t)note, (int16_t)pitch_bend)
 *  - pb_to_frac(pitch_bend) -> frac(float between 0-1)
 *  - midinote(float) = (float)note + frac
 *  - mtof(midinote) -> freq (Hz)
 *
 */

/**
 * @brief mtof - midi to freq - assume 69 = A4 = 440
 */
static inline float mtof(float m) {
    return 440.0f * powf(2.0f, (m - 69.0f) / 12.0f);
}

/**
 * @brief ftom - freq to midi - assume 69 = A4 = 440
 */
static inline float ftom(float f) {
    return 12.0f * log2f(f / 440.0f) + 69.0f;
}

/**
 * @brief fractional part to pitch bend centered on 0.5 = 8191 = no bend
 */
static inline int16_t frac_to_pb(float f) {
    return (int16_t) roundf(f * 16383.0f);
}

/**
 * @brief pitch bend value to fractional part
 */
static inline float pb_to_frac(uint16_t pb) {
    return (float) pb * (1.0f / 16383.0f);
}

/**
 * @brief amp to midi velocity (linear)
 */
static inline uint8_t amp_to_midivel_lin(float v) {
    return (uint8_t) (v * 127.0f + 0.5f);
}

/**
 * @brief amp to midi velocity (pow)
 */
static inline uint8_t amp_to_midivel_pow(float v) {
    return (uint8_t) (powf(v, 0.5f) * 127.0f);
}

/**
 * @brief midi velocity to amp (linear)
 */
static inline float midivel_to_amp_lin(uint8_t v) {
    return (float) v / 127.0f;
}

/**
 * @brief midi velocity to amp (pow)
 */
static inline float midivel_to_amp_pow(uint8_t v) {
    return powf((float) v / 127.0f, 2.0f);
}

#ifdef __cplusplus
}
#endif

#endif
