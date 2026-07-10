

#include <dsp/assert.h>
#include <dsp/stft.h>
#include <dsp/utils.h>

#include <string.h>

void stft_init(stft* self,
               rdft* dft,
               rdft_execute forward,
               float* win,
               float* buf,
               float* tmp_buf,
               uint32_t fft_sz,
               uint32_t hop_sz,
               uint32_t buf_sz) {

    dsp_assert(is_pow2(fft_sz), "fft_sz must be pow2");
    dsp_assert(hop_sz >= 1, "hop_sz must be >= 1");
    dsp_assert(buf_sz > fft_sz, "buf_sz must be > fft_sz");
    dsp_assert(buf_sz % fft_sz == 0, "buf_sz must be a multiple of fft_sz");

    self->dft = dft;
    self->forward = forward;
    self->buf = buf;
    self->tmp_buf_ = tmp_buf;
    self->win = win;
    self->fft_sz = fft_sz;
    self->hop_sz = hop_sz;
    self->buf_sz = buf_sz;

    // zero this guy to be safe
    memset(self->buf, 0, sizeof(float) * buf_sz);
    memset(self->tmp_buf_, 0, sizeof(float) * fft_sz);

    self->buf_mask_ = self->buf_sz - 1;
    self->write_ptr_ = 0;
    self->write_counter_ = 0;
}

bool stft_tick(stft* self, dft_complex* frame_out, float* real_in) {
    // NOTE: assume real_in should be hop_sz len
    // frame_out are fft_sz // 2 + 1 len

    for (uint32_t i = 0; i < self->hop_sz; i++) {
        self->buf[self->write_ptr_] = real_in[i];
        self->write_ptr_ = (self->write_ptr_ + 1) & self->buf_mask_;
    }

    self->write_counter_ += self->hop_sz;
    float* tmp = self->tmp_buf_;

    if (self->write_counter_ >= self->fft_sz) {
        for (uint32_t i = 0; i < self->fft_sz; i++) {
            uint32_t idx = (self->write_ptr_ - self->fft_sz + i) & self->buf_mask_;
            tmp[i] = self->win[i] * self->buf[idx];
        }

        self->forward(self->dft, frame_out, tmp);
        self->write_counter_ -= self->hop_sz;
        return true;
    }
    return false;
}

void istft_init(istft* self,
                irdft* idft,
                irdft_execute inverse,
                float* ola,
                float* win,
                float* ola_norm,
                float* tmp_buf_,
                uint32_t fft_sz,
                uint32_t hop_sz,
                uint32_t ola_sz) {

    dsp_assert(is_pow2(fft_sz), "fft_sz must be pow2");
    dsp_assert(hop_sz >= 1, "hop_sz must be >= 1");
    dsp_assert(ola_sz > fft_sz, "ola_sz must be at least 2x fft_sz");
    dsp_assert(ola_sz % fft_sz == 0, "ola_sz must be a multiple of fft_sz");

    self->idft = idft;
    self->inverse = inverse;
    self->ola = ola;
    self->win = win;
    self->ola_norm = ola_norm;
    self->tmp_buf_ = tmp_buf_;
    self->fft_sz = fft_sz;
    self->hop_sz = hop_sz;
    self->ola_sz = ola_sz;

    // zero this guy to be safe
    memset(self->ola, 0, sizeof(float) * ola_sz);
    memset(self->tmp_buf_, 0, sizeof(float) * fft_sz);

    self->ola_mask_ = ola_sz - 1;
    self->read_ptr_ = 0;
    self->write_ptr_ = 0;
}

void istft_tick(istft* self, float* real_out, dft_complex* frame_in) {
    // NOTE: frame_in sz assumed to fft_sz // 2 + 1 paired with stft
    //   real_out should be one hop length

    // do inverse fft
    float* tmp = self->tmp_buf_;
    self->inverse(self->idft, tmp, frame_in);

    // overlap add one fft len into the ola buffer (optional window)
    for (uint32_t i = 0; i < self->fft_sz; i++) {
        uint32_t idx = (self->write_ptr_ + i) & self->ola_mask_;
        self->ola[idx] += self->win != NULL ? tmp[i] * self->win[i] : tmp[i];
    }

    // increment write_ptr start by one hop sz
    self->write_ptr_ = (self->write_ptr_ + self->hop_sz) & self->ola_mask_;

    // write hop sz into real_out from read pos (normalize if applicable)
    for (uint32_t i = 0; i < self->hop_sz; i++) {
        uint32_t idx = (self->read_ptr_ + i) & self->ola_mask_;
        real_out[i] = self->ola_norm != NULL ? self->ola_norm[idx] * self->ola[idx]
                                             : self->ola[idx];
        self->ola[idx] = 0.0;
    }

    // increment the read ptr one hop length
    self->read_ptr_ = (self->read_ptr_ + self->hop_sz) & self->ola_mask_;
}
