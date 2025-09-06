/**
 * @brief polyblep saw and square oscillators
 *  - based on
 https://www.martin-finke.de/articles/audio-plugins-018-polyblep-oscillator/
 */
#ifndef DSP_PBLEP_H
#define DSP_PBLEP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/phasor.h>
#include <stdint.h>

static inline double polyblep(double t, double dt) {
    // TODO: maybe add unlikely?
    if (t < dt) {
        t /= dt;
        return t + t - t * t - 1.0;  // 2t - t^2 - 1
    } else if (t > 1.0 - dt) {
        t = (t - 1.0) / dt;
        return t * t + t + t + 1.0;  // t^2 + 2t + 1
    }
    return 0.0;
}

typedef struct {
    float freq, iphase, sr;
    // private
    double incr_, phase_;
} blepsaw;

/**
 * @brief init the blepsaw. iphs is guaranteed wrapped betwen 0 and 1
 */
void blepsaw_init(blepsaw* self, float freq, float iphs, float sr);

/**
 * @brief tick one block of blepsaw.
 */
void blepsaw_tick_block(blepsaw* self, float* out, float* freq, uint32_t sz);

typedef struct {
    float freq, iphase, duty, sr;
    // private
    double incr_, phase_;
} blepsqr;

/**
 * @brief init the blepsqr. iphs is guaranteed wrapped betwen 0 and 1
 */
void blepsqr_init(blepsqr* self, float freq, float duty, float iphs, float sr);

/**
 * @brief tick one block of blepsqr.
 */
void blepsqr_tick_block(blepsqr* self,
                        float* out,
                        float* freq,
                        float* duty,
                        uint32_t sz);

#ifdef __cplusplus
}
#endif

#endif
