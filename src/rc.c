#include <dsp/constants.h>
#include <dsp/fasttrig.h>
#include <dsp/rc.h>
#include <dsp/shape.h>
#include <dsp/utils.h>

// private
//
static inline void rc_one_pole_update_(rc_one_pole* self) {

    double omega_c = (double) TWO_PI * (double) self->freq;
    double omega_warped = self->two_over_t_ * tan(omega_c * self->t_over_2_);
    double g = omega_warped * self->t_over_2_ / 2.0;

    self->g_ = g / (1.0 + g);
}

static void rc_one_pole_tick_(rc_one_pole* self, double xn) {

    double vn = (xn - self->z0_) * self->g_;

    self->lp_ = vn + self->z0_;
    self->z0_ = vn + self->lp_;

    self->hp_ = xn - self->lp_;
    self->ap_ = self->lp_ - self->hp_;
}

// public API
//

void rc_one_pole_init(rc_one_pole* self, float freq, float sr) {
    // public
    self->sr = sr;
    self->freq = freq;

    // private
    self->t_ = 1.0 / (double) self->sr;
    self->t_over_2_ = self->t_ / 2.0;
    self->two_over_t_ = 2.0 / self->t_;

    self->g_ = 1e-9;
    self->z0_ = 1e-9;
    self->lp_ = 1e-9;
    self->hp_ = 1e-9;
    self->ap_ = 1e-9;

    rc_one_pole_update_(self);
}

void rc_one_pole_tick_block(rc_one_pole* self,
                            float* in,
                            float* lp_out,
                            float* hp_out,
                            float* ap_out,
                            float* freq,
                            uint32_t start,
                            uint32_t nsmps) {

    for (uint32_t i = start; i < nsmps; i++) {

        float freq_ = freq[i];
        bool freq_eq = check_float_equal(freq_, self->freq);
        if (!freq_eq) {
            self->freq = freq_;
            rc_one_pole_update_(self);
        }

        rc_one_pole_tick_(self, (double) in[i]);

        // write to valid vectors.
        if (lp_out)
            lp_out[i] = (float) self->lp_;
        if (hp_out)
            hp_out[i] = (float) self->hp_;
        if (ap_out)
            ap_out[i] = (float) self->ap_;
    }
}

static inline void rc_ladder_update_(rc_ladder* self) {
    float fc = self->freq;
    float q = self->q;

    // scale Q -> k ... Q= 0.1->25.0, k = 0->4 ...  from pirkle/yi
    self->k_ = 4.0 * ((double) q - 0.1) / (25.0 - 0.1);

    // zavalashin's warping
    double omega_c = (double) TWO_PI * (double) fc;
    double omega_warped = self->two_over_t_ * tan(omega_c * self->t_over_2_);
    double g = omega_warped * self->t_over_2_;

    // we already calculate g so give it here to the one_pole bank
    for (int i = 0; i < 4; i++) {
        self->bank_[i].g_ = g;
    }

    // need to de-warp .. yay math..
    self->one_plus_g_ = (1.0 + g);

    // expand g
    self->g_ = g / (1.0 + g);
    self->g2_ = g * g;
    self->g3_ = self->g2_ * g;
    self->g4_ = self->g3_ * g;
}

static inline float rc_ladder_tick_(rc_ladder* self, float xn) {
    // we must access the previous output state registers
    // to create the feedback parameter S
    // See https://www.kvraudio.com/forum/viewtopic.php?t=571909
    double one_plus_g = self->one_plus_g_;

    double f1_z0 = self->bank_[0].z0_ / one_plus_g;
    double f2_z0 = self->bank_[1].z0_ / one_plus_g;
    double f3_z0 = self->bank_[2].z0_ / one_plus_g;
    double f4_z0 = self->bank_[3].z0_ / one_plus_g;

    // feedback param
    double s = self->g3_ * f1_z0 + self->g2_ * f2_z0 + self->g_ * f3_z0 + f4_z0;

    // solve for un
    double un = ((double) xn - self->k_ * s) / (1.0 + self->k_ * self->g4_);

    // zavalishins non-linear drive
    un = fast_tanh_d(un);

    // tick the bank
    rc_one_pole_tick_(&self->bank_[0], un);
    rc_one_pole_tick_(&self->bank_[1], self->bank_[0].lp_);
    rc_one_pole_tick_(&self->bank_[2], self->bank_[1].lp_);
    rc_one_pole_tick_(&self->bank_[3], self->bank_[2].lp_);

    return (float) self->bank_[3].lp_;
}

void rc_ladder_init(rc_ladder* self, float freq, float q, float sr) {
    self->freq = freq;
    self->sr = sr;
    self->q = q;

    self->t_ = 1.0 / (double) self->sr;
    self->t_over_2_ = self->t_ / 2.0;
    self->two_over_t_ = 2.0 / self->t_;

    self->k_ = 1e-9;
    self->g_ = 1e-9;
    self->g2_ = 1e-9;
    self->g3_ = 1e-9;
    self->g4_ = 1e-9;
    self->one_plus_g_ = 1e-9;

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
                          float* out,
                          float* in,
                          float* freq,
                          float* q,
                          uint32_t start,
                          uint32_t nsmps) {
    for (uint32_t i = start; i < nsmps; i++) {

        float freq_ = freq[i];
        bool freq_eq = check_float_equal(freq_, self->freq);

        float q_ = q[i];
        bool q_eq = check_float_equal(q_, self->q);

        if (!freq_eq || !q_eq) {
            self->freq = freq_;
            self->q = q_;
            rc_ladder_update_(self);
        }

        out[i] = rc_ladder_tick_(self, in[i]);
    }
}
