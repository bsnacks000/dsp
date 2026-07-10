/**
 * @file stage.h
 * @brief envelope stages
 *
 * These are common across the line and curve modules.
 *
 * */
#ifndef DSP_STAGE_H
#define DSP_STAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief stages for an AR envelope.
 */
typedef enum {
    AR_IDLE = 0,
    AR_ATK,
    AR_REL,
} ar_stage;

/**
 * @brief stages for an ADSR envelope.
 */
typedef enum {
    ADSR_IDLE = 0,
    ADSR_ATK,
    ADSR_DCY,
    ADSR_SUS,
    ADSR_REL,
} adsr_stage;

#ifdef __cplusplus
}
#endif

#endif
