/**
 * @file constants.h
 * @author bsnacks000
 * @brief Useful constants
 * @version 0.1
 * @date 2025-02-12
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef DSP_CONSTANTS_H
#define DSP_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>

// sometimes not in math.h
#ifdef M_PI
#    define DSP_PI M_PI
#else
#    define DSP_PI 3.14159265358979323846
#endif

#define SQRT_TWO 1.41421356237309504880

#define E 2.718281828459

static const float TWO_PI = DSP_PI * 2.0;

static const float HALF_PI = DSP_PI * 0.5;

static const float QRTR_PI = DSP_PI * 0.25;

static const float DSP_PI_SQUARED = DSP_PI * DSP_PI;

static const float TANGENT_THRESHOLD = 0.95 * DSP_PI / 2.0;

#ifdef __cplusplus
}
#endif

#endif
