
#include <dsp/constants.h>
#include <dsp/shape.h>
#include <dsp/svf.h>
#include <dsp/utils.h>

static inline void update_(svf* self) {
    float freq = self->freq;
    float q = self->q;

    // warp freq
    float omega_c = TWO_PI * freq;
    float omega_warped = self->two_ovr_t_ * tanf(omega_c * self->t_ovr_2_);
    float g = omega_warped * self->t_ovr_2_;

    // dampening
    float r = 1.0 / (2.0 * q);
    float two_r = 2.0 * r;
    float four_r = 4.0 * r;
    float two_r_plus_g = two_r + g;
    float g1 = 1.0 / (1.0 + two_r * g + g * g);

    self->g_ = g;
    self->g1_ = g1;
    self->two_r_ = two_r;
    self->four_r_ = four_r;
    self->two_r_plus_g_ = two_r_plus_g;
}

static inline void tick_(svf* self, float xn) {
    float hpf = self->g1_ * (xn - self->two_r_plus_g_ * self->z0_ - self->z1_);

    float bpf = self->g_ * hpf + self->z0_;
    bpf = self->drive ? fast_tanh_clip(bpf, self->drive) : bpf;

    float lpf = self->g_ * bpf + self->z1_;
    self->z0_ = self->g_ * hpf + bpf;
    self->z1_ = self->g_ * bpf + lpf;

    self->hp = hpf;
    self->bp = bpf;
    self->lp = lpf;
    self->bs = hpf + lpf;
    self->ap = xn - self->four_r_ * bpf;
}

void svf_init(svf* self, float freq, float q, float drive, float sr) {
    self->sr = sr;
    self->freq = freq;
    self->q = q;
    self->drive = drive;

    // init T w/ sr for freq warping.
    self->t_ = 1.0 / sr;
    self->t_ovr_2_ = self->t_ / 2.0;
    self->two_ovr_t_ = 2.0 / self->t_;

    // memory
    self->z0_ = 1e-9;
    self->z1_ = 1e-9;

    // let's cook...
    update_(self);
}

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
                    uint32_t sz) {

    for (uint32_t i = 0; i < sz; i++) {
        float freq_ = freq[i];
        bool freq_eq = check_float_equal(freq_, self->freq);

        float q_ = q[i];
        bool q_eq = check_float_equal(q_, self->q);

        float drive_ = drive[i];
        bool drive_eq = check_float_equal(drive_, self->drive);

        if (!freq_eq || !q_eq || !drive_eq) {
            self->freq = freq_;
            self->q = q_;
            self->drive = drive_;
            update_(self);
        }

        tick_(self, in[i]);

        if (out_lp)
            out_lp[i] = self->lp;
        if (out_hp)
            out_hp[i] = self->hp;
        if (out_bp)
            out_bp[i] = self->bp;
        if (out_bs)
            out_bs[i] = self->bs;
        if (out_ap)
            out_ap[i] = self->ap;
    }
}
