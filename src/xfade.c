#include <dsp/utils.h>
#include <dsp/xfade.h>
#include <stdint.h>

static inline void update_(xfade* self) {
    xfade_pair pair = xfade_sin(self->position);
    self->l_coeff_ = pair.left;
    self->r_coeff_ = pair.right;
}

void xfade_init(xfade* self, float position) {
    self->position = position;
    self->last_ = 0.0;
    self->l_coeff_ = 0.0;
    self->r_coeff_ = 0.0;

    update_(self);
}

void xfade_tick_block(xfade* self,
                      float* out,
                      float* in_l,
                      float* in_r,
                      float* position,
                      uint32_t nsmps) {
    for (uint32_t i = 0; i < nsmps; i++) {
        float position_ = position[i];
        bool pos_eq = check_float_equal(position_, self->position);
        if (!pos_eq) {
            self->position = position_;
            update_(self);
            out[i] = in_l[i] * self->l_coeff_ + in_r[i] * self->r_coeff_;
        } else {
            out[i] = self->last_;
        }
    }
}
