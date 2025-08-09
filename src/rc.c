#include <dsp/constants.h>
#include <dsp/rc.h>
#include <dsp/utils.h>

// private
//
static inline void update_(rc_one_pole* self) {

    float omega_c = TWO_PI * self->freq;
    float omega_warped = self->two_over_t_ * tan(omega_c * self->t_over_2_);
    float g = omega_warped * self->t_over_2_ / 2.0;

    self->g_ = g / (1.0 + g);
}

static void tick_(rc_one_pole* self, float xn) {

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

    update_(self);
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
            update_(self);
        }

        tick_(self, in[i]);

        // write to valid vectors.
        if (out_lp)
            out_lp[i] = self->lp;
        if (out_hp)
            out_hp[i] = self->hp;
        if (out_ap)
            out_ap[i] = self->ap;
    }
}
