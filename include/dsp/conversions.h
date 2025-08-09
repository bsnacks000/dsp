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

#include <math.h>
#include <stdint.h>

/**
 * @brief Calculate the number of samples per millisecond given samplerate sr in samples
 *  per second.
 *
 */
inline float samps_per_ms(float sr) {
    return sr / 1000.0;
}

/**
 * @brief Convert decibel gain to amplitude (linear)
 *
 */
inline float db_to_amp(float db) {
    return pow(10.0, db / 20.0);
}

/**
 * @brief Convert decibel gain to power (linear)
 *
 */
inline float db_to_power(float db) {
    return pow(10.0, db / 10.0);
}

/**
 * @brief Convert linear amplitude to decibels
 *
 */
inline float amp_to_db(float lin) {
    return 20.0 * log10(lin);
}

/**
 * @brief Convert linear power to decibels
 *
 */
inline float power_to_db(float power) {
    return 10.0 * log10(power);
}

/**
 * @brief calculate peak gain for q
 *
 */
inline float peak_gain_for_q(float q) {
    if (q <= 0.707)
        return 1.0;
    return (q * q) / pow((q * q - 0.25), 0.5);
}

/**
 * @brief calculate peak gain for q and convert from linamp to db.
 *
 */
inline float db_peak_gain_for_q(float q) {
    q = peak_gain_for_q(q);
    return amp_to_db(q);
}

/**
 * @brief convert unipolar to bipolar.
 *
 */
inline float unipolar_to_bipolar(float val) {
    return 2.0 * val - 1.0;
}

/**
 * @brief convert bipolar to unipolar.
 *
 */
inline float bipolar_to_unipolar(float val) {
    return 0.5 * val + 0.5;
}

#ifdef __cplusplus
}
#endif

#endif
