#include <dsp/constants.h>
#include <dsp/rc.h>
#include <dsp/shape.h>
#include <dsp/utils.h>
#include "dsp/fasttrig.h"

// private
//
static inline void rc_one_pole_update_(rc_one_pole* self) {

    float omega_c = TWO_PI * self->freq;
    float omega_warped = self->two_over_t_ * tan(omega_c * self->t_over_2_);
    float g = omega_warped * self->t_over_2_ / 2.0;

    self->g_ = g / (1.0 + g);
}

static void rc_one_pole_tick_(rc_one_pole* self, float xn) {

    float vn = (xn - self->z0_) * self->g_;

    self->lp = vn + self->z0_;
    self->z0_ = vn + self->lp;

    self->hp = xn - self->lp;
    self->ap = self->lp - self->hp;
}

// public API
//

void rc_one_pole_init(rc_one_pole* self, float freq, float sr) {
    // public
    self->sr = sr;
    self->freq = freq;

    // private
    self->t_ = 1.0 / self->sr;
    self->t_over_2_ = self->t_ / 2.0;
    self->two_over_t_ = 2.0 / self->t_;

    self->g_ = 1e-9;
    self->z0_ = 1e-9;
    self->lp = 1e-9;
    self->hp = 1e-9;
    self->ap = 1e-9;

    rc_one_pole_update_(self);
}

void rc_one_pole_tick_block(rc_one_pole* self,
                            float* in,
                            float* out_lp,
                            float* out_hp,
                            float* out_ap,
                            float* freq,
                            uint32_t sz) {

    for (uint32_t i = 0; i < sz; i++) {

        float freq_ = freq[i];
        bool freq_eq = check_float_equal(freq_, self->freq);
        if (!freq_eq) {
            self->freq = freq_;
            rc_one_pole_update_(self);
        }

        rc_one_pole_tick_(self, in[i]);

        // write to valid vectors.
        if (out_lp)
            out_lp[i] = self->lp;
        if (out_hp)
            out_hp[i] = self->hp;
        if (out_ap)
            out_ap[i] = self->ap;
    }
}

static inline void rc_ladder_update_(rc_ladder* self) {
    float fc = self->freq;
    float q = self->q;

    for (int i = 0; i < 4; i++) {
        self->bank_[i].freq = fc;
        rc_one_pole_update_(&self->bank_[i]);
    }

    // scale Q -> k from (0-25) -> (0-4) -- from pirkle
    self->k_ = 4.0 * (q - 0.707) / (25.0 - 0.707);

    // zavalashin's warping
    float omega_c = TWO_PI * fc;
    float omega_warped = self->two_over_t_ * tan(omega_c * self->t_over_2_);
    float g = omega_warped * self->t_over_2_;

    // expand g
    self->g_ = g;
    self->g2_ = g * g;
    self->g3_ = g * g * g;
    self->g4_ = g * g * g * g;
}

static inline void rc_ladder_tick_(rc_ladder* self, float xn) {
    // we must access the previous output state registers
    // to create the feedback parameter S
    // See https://www.kvraudio.com/forum/viewtopic.php?t=571909

    float f1_z0 = self->bank_[0].z0_ / (1.0 + self->g_);
    float f2_z0 = self->bank_[1].z0_ / (1.0 + self->g_);
    float f3_z0 = self->bank_[2].z0_ / (1.0 + self->g_);
    float f4_z0 = self->bank_[3].z0_ / (1.0 + self->g_);

    // feedback param
    float s = self->g3_ * f1_z0 + self->g2_ * f2_z0 + self->g_ * f3_z0 + f4_z0;

    // solve for un
    float out = (xn - self->k_ * s) / (1.0 + self->k_ * self->g4_);

    // zavalishins non-linear drive
    out = fast_tanh(out);

    // tick the bank
    for (int i = 0; i < 4; i++) {
        rc_one_pole_tick_(&self->bank_[i], out);
    }

    // grab last lp and attenuvert to form hp signal.
    self->lp_ = self->bank_[3].lp;
    self->hp_ = self->lp_ * -1.0 + xn;
}

void rc_ladder_init(rc_ladder* self, float freq, float q, float sr) {
    self->freq = freq;
    self->sr = sr;
    self->q = q;

    self->t_ = 1.0 / self->sr;
    self->t_over_2_ = self->t_ / 2.0;
    self->two_over_t_ = 2.0 / self->t_;

    self->lp_ = 1e-9;
    self->hp_ = 1e-9;
    self->k_ = 1e-9;
    self->g_ = 1e-9;
    self->g2_ = 1e-9;
    self->g3_ = 1e-9;
    self->g4_ = 1e-9;

    // init the one pole bank
    for (int i = 0; i < 4; i++) {
        rc_one_pole_init(&self->bank_[i], freq, sr);
    }

    rc_ladder_update_(self);
}

/**
 * @brief tick one block of the rc_ladder.
 */
void rc_ladder_tick_block(rc_ladder* self,
                          float* out_lp,
                          float* out_hp,
                          float* in,
                          float* freq,
                          float* q,
                          uint32_t nsmps) {
    for (uint32_t i = 0; i < nsmps; i++) {

        float freq_ = freq[i];
        bool freq_eq = check_float_equal(freq_, self->freq);

        float q_ = q[i];
        bool q_eq = check_float_equal(q_, self->q);

        if (!freq_eq || !q_eq) {
            self->freq = freq_;
            self->q = q_;
            rc_ladder_update_(self);
        }
        rc_ladder_tick_(self, in[i]);

        // write to valid vectors.
        if (out_lp)
            out_lp[i] = self->lp_;
        if (out_hp)
            out_hp[i] = self->hp_;
    }
}
