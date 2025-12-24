#include <dsp/ftable/ftable.h>
#include <dsp/utils.h>

void ftable_zero_buf(ftable* self) {
    zero_buf(self->buf, self->buf_sz);
}

void ftable_init(ftable* self, float* buf, uint32_t buf_sz) {
    self->buf = buf;
    self->buf_sz = buf_sz;

    self->len = buf_sz - 2;
    self->mask = self->len - 1;
    self->is_pow2 = is_pow2(self->len);

    // safety first..
    self->nharms = 0.0;
    self->f0 = 0.0;
    self->sr = 0.0;
}

dsp_err ftable_shallow_copy(ftable* self, ftable* other) {
    if (self->buf_sz != other->buf_sz)
        return DSP_ERR;
    ftable_init(self, other->buf, other->buf_sz);
    return DSP_OK;
}

dsp_err ftable_deep_copy(ftable* self, ftable* other) {
    if (self->buf_sz != other->buf_sz)
        return DSP_ERR;
    copy_nsmps(self->buf, other->buf, self->buf_sz);
    return DSP_OK;
}

void ftable_write(ftable* self, float* in, uint32_t nsmps, uint32_t offset) {
    for (uint32_t i = offset; i < nsmps; i++) {
        self->buf[i] = in[i];
    }
}

void ftable_add_guard_point(ftable* self) {
    self->buf[self->len] = self->buf[0];
    self->buf[self->len + 1] = self->buf[1];
}

dsp_err ftable_func(ftable* self, ft_f f, void* args) {
    ftable_zero_buf(self);

    dsp_err err;
    if ((err = f(self, args)) != DSP_OK) {
        return err;
    }

    // guardpoint
    ftable_add_guard_point(self);
    return DSP_OK;
}
