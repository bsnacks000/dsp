/**
 * @file bqf.h
 *
 * @brief Classic first and second order biquad filters.
 * - Most from Pirkle - Designing Audio Effect Plugins in C++
 * - Second order shelving filters are directly from Bristow-Johnson Audio EQ Cookbook.
 *
 *  These use the transposed direct form II calculation
 *  (https://ccrma.stanford.edu/~jos/fp/Transposed_Direct_Forms.html) and check
 *  for float underflow. This API is exposed for debugging purposes but is more
 *  or less internal alongside the design equations.
 *
 * The API uses function pointers to pass a `dfII_xxx_design_equation` for first
 * and second order filter types. This runs on a generic dfII state struct. Filters are
 * grouped by the allowed modulation parameters in the design equations.
 *
 */

#ifndef BQF_H
#define BQF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief dfII state
 */
typedef struct {
    float a0, a1, a2, b1, b2, c0, d0, x_z1, x_z2, sr;
} dfII;

/**
 * @brief Initialize the dfII state.
 */
void dfII_init(dfII* self, float sr);

/**
 * @brief Tick a step of the dfII equation.
 */
float dfII_tick(dfII* self, float xn);

/**
 * @brief Design equations for freq parametric
 */
typedef void (*dfII_non_resonant_design_equation)(dfII* self, float freq);

/**
 * Simple filter
 */
typedef enum {
    BQ_FIRST_ORDER_LPF = 0,
    BQ_FIRST_ORDER_HPF,
    BQ_FIRST_ORDER_APF,
    BQ_BUTTERWORTH,
    BQ_LINKWITZ_RILEY
} bq_non_resonant_type;

//
typedef struct {
    float sr, freq;
    bq_non_resonant_type t;
    dfII state_;
    dfII_non_resonant_design_equation update_;
} bq_non_resonant;

/**
 * @brief Initialize a bq_non_resonant of some type t
 *  Will match type and assure the correct design equation is set.
 */
void bq_non_resonant_init(bq_non_resonant* self,
                          bq_non_resonant_type t,
                          float freq,
                          float sr);

/**
 * @brief Tick the bq_non_resonant one block.
 */
void bq_non_resonant_tick_block(bq_non_resonant* self,
                                float* out,
                                float* in,
                                float freq,
                                uint32_t start,
                                uint32_t nsmps);

/***
 * Classic Resonant two pole implementations.
 *  - lowpass, highpass, bandpass, bandstop, reson (Smith-Angell) and allpass
 *  - the interface allows parameters for fc and q
 */

typedef enum {
    BQ_SECOND_ORDER_LPF = 0,  //  low pass
    BQ_SECOND_ORDER_HPF,      //  hi pass
    BQ_SECOND_ORDER_BPF,      //  band pass
    BQ_SECOND_ORDER_BSF,      //  band stop
    BQ_SECOND_ORDER_RESON,    // smith-angell reson
    BQ_SECOND_ORDER_APF       // all pass
} bq_resonant_type;

/**
 * @brief Design equations for resonant second order filters
 *
 */
typedef void (*dfII_resonant_design_equation)(dfII* self, float freq, float q);

/**
 * @brief 2nd order resonant IIR filters
 *
 */
typedef struct {
    float sr, freq, q;
    bq_resonant_type t;
    dfII state_;
    dfII_resonant_design_equation update_;
} bq_resonant;

/**
 * @brief
 */
void bq_resonant_init(bq_resonant* self,
                      bq_resonant_type t,
                      float freq,
                      float q,
                      float sr);

/**
 * @brief Calculate one sample of the bq_resonant. This will update dfII
 * state.
 *
 * @param self
 * @param xn
 * @return float
 */
void bq_resonant_tick_block(bq_resonant* self,
                            float* out,
                            float* in,
                            float freq,
                            float q,
                            uint32_t start,
                            uint32_t nsmps);

/**
 * Shelving and parametric designs (2nd order)
 *  - low shelf, hi shelf, peak
 *
 */

typedef enum {
    BQ_PARA_LSHLF = 0,  // second order low shelf
    BQ_PARA_HSHLF,      // second order hi shelf
    BQ_PARA_PEAK,       // second order band
} bq_para_eq_type;

/**
 * @brief prototype for a parametric design equation.
 *
 */
typedef void (*dfII_para_eq_design_equation)(dfII* self,
                                             float freq,
                                             float q,
                                             float gain);

/**
 * @brief bq_para_eq state.
 */
typedef struct bq_para_eq_ {
    float freq, q, gain, sr;
    bq_para_eq_type t;
    dfII state_;
    dfII_para_eq_design_equation update_;
} bq_para_eq;

/**
 * @brief Initialize a bq_para_eq of some type t at sample rate sr.
 */
void bq_para_eq_init(bq_para_eq* self,
                     bq_para_eq_type t,
                     float freq,
                     float q,
                     float gain,
                     float sr);

/**
 * @brief Tick one block.
 */
void bq_para_eq_tick_block(bq_para_eq* self,
                           float* out,
                           float* in,
                           float freq,
                           float q,
                           float gain,
                           uint32_t start,
                           uint32_t nsmps);

#ifdef __cplusplus
}
#endif

#endif
