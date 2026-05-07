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

#define DSP_PI 3.14159265358979323846
#define DSP_PI_F 3.14159274101257324219f

#define DSP_TWO_PI 6.28318530717958647692
#define DSP_TWO_PI_F 6.28318548202514648438f

#define DSP_HALF_PI_F 1.57079637050628662109f
#define DSP_QTR_PI_F 0.78539818525314331055f

#define DSP_SQRT_TWO 1.41421356237309504880
#define DSP_E 2.71828182845904523536

#define DSP_PI_SQUARED_F 9.86960506439208984375f

// avoids tangent asymptote pi/2
#define TANGENT_THRESHOLD_F 1.4922565104551517f

#ifdef __cplusplus
}
#endif

#endif
