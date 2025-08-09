/**
 * @file svf.h
 *
 * @brief Zavalishin's 2nd order state variable filter with drive.

 This only contains LPF, HPF, APF, BPF, BSF, and NBPF implementations.
 Shelving is not implemented.
 */
#ifndef DSP_SVF_H
#define DSP_SVF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    // public
    float freq, q, drive, sr, lp, hp, bp, bs, ap;
    // private
    float z0_, z1_, t_, t_ovr_2_, two_ovr_t_, two_r_, four_r_, g_, g1_, two_r_plus_g_;
} svf;

/**
 * @brief Initialize an svf.
 *
 */
void svf_init(svf* self, float freq, float q, float drive, float sr);

/**
 * @brief Tick a block for svf. Passing a non-null pointer will cause a write.
 */
void svf_tick_block(svf* self,
                    float* out_lp,
                    float* out_hp,
                    float* out_bp,
                    float* out_bs,
                    float* out_ap,
                    float* in,
                    float* freq,
                    float* q,
                    float* drive,
                    uint32_t sz);

#ifdef __cplusplus
}
#endif

#endif
