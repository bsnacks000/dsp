
#include <dsp/maths.h>
#include <dsp/shape.h>
#include <dsp/svf.h>
#include <dsp/utils.h>

static inline void update_(svf* self) {

    // warp freq
    double omega_c = DSP_TWO_PI * (double) self->freq;
    double omega_warped = self->two_ovr_t_ * tan(omega_c * self->t_ovr_2_);
    double g = omega_warped * self->t_ovr_2_;

    // dampening
    double r = 1.0 / (2.0 * (double) self->q);
    double two_r = 2.0 * r;
    double four_r = 4.0 * r;
    double two_r_plus_g = two_r + g;
    double g1 = 1.0 / (1.0 + two_r * g + g * g);

    self->g_ = g;
    self->g1_ = g1;
    self->two_r_ = two_r;
    self->four_r_ = four_r;
    self->two_r_plus_g_ = two_r_plus_g;
}

static inline void tick_(svf* self, float xn) {
    double x = (double) xn;
    double drive = (double) self->drive;
    double hpf = self->g1_ * (x - self->two_r_plus_g_ * self->z0_ - self->z1_);

    double bpf = self->g_ * hpf + self->z0_;
    bpf = (int) drive ? fast_tanh_clip_d(bpf, drive) : bpf;

    double lpf = self->g_ * bpf + self->z1_;
    self->z0_ = self->g_ * hpf + bpf;
    self->z1_ = self->g_ * bpf + lpf;

    self->hp = hpf;
    self->bp = bpf;
    self->lp = lpf;
    self->bs = hpf + lpf;
    self->ap = x - self->four_r_ * bpf;
}

void svf_init(svf* self, float freq, float q, float drive, float sr) {
    self->sr = assure_gt_zero(sr);
    self->freq = freq;
    self->q = q;
    self->drive = drive;

    // init T w/ sr for freq warping.
    self->t_ = 1.0 / (double) sr;
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
                    uint32_t start,
                    uint32_t nsmps) {

    for (uint32_t i = start; i < nsmps; i++) {
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
            out_lp[i] = (float) self->lp;
        if (out_hp)
            out_hp[i] = (float) self->hp;
        if (out_bp)
            out_bp[i] = (float) self->bp;
        if (out_bs)
            out_bs[i] = (float) self->bs;
        if (out_ap)
            out_ap[i] = (float) self->ap;
    }
}
