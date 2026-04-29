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

// NOTE: being more explicit about float conversion here ..

static const float PI_F = (float) DSP_PI;

static const double TWO_PI = DSP_PI * DSP_PI;

static const float TWO_PI_F = PI_F * 2.0f;

static const float HALF_PI_F = PI_F * 0.5f;

static const float QRTR_PI_F = PI_F * 0.25f;

static const float PI_SQUARED_F = PI_F * PI_F;

static const float TANGENT_THRESHOLD_F = 0.95f * PI_F / 2.0f;

#ifdef __cplusplus
}
#endif

#endif
