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
    return sr / 1000.0;
}

/**
 * @brief Convert decibel gain to amplitude (linear)
 *
 */
static inline float db_to_amp(float db) {
    return pow(10.0, db / 20.0);
}

/**
 * @brief Convert decibel gain to power (linear)
 *
 */
static inline float db_to_power(float db) {
    return pow(10.0, db / 10.0);
}

/**
 * @brief Convert linear amplitude to decibels
 *
 */
static inline float amp_to_db(float lin) {
    dsp_assert(lin > 0.0, "lin must be > 0.");
    return 20.0 * log10(lin);
}

/**
 * @brief Convert linear power to decibels
 *
 */
static inline float power_to_db(float power) {
    dsp_assert(power > 0.0, "power must be > 0.");
    return 10.0 * log10(power);
}

/**
 * @brief calculate peak gain for q
 *
 */
static inline float peak_gain_for_q(float q) {
    if (q <= 0.707)
        return 1.0;
    return (q * q) / pow((q * q - 0.25), 0.5);
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
    return 2.0 * val - 1.0;
}

/**
 * @brief convert bipolar to unipolar.
 *
 */
static inline float bipolar_to_unipolar(float val) {
    return 0.5 * val + 0.5;
}

#ifdef __cplusplus
}
#endif

#endif
