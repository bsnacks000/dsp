#include <dsp/stft.h>
#include <string.h>

void stft_init(stft* self,
               rdft* dft,
               rdft_execute forward,
               float* win,
               float* buf,
               uint32_t fft_sz,
               uint32_t hop_sz,
               uint32_t buf_sz) {
    self->dft = dft;
    self->forward = forward;
    self->buf = buf;
    self->win = win;
    self->fft_sz = fft_sz;
    self->hop_sz = hop_sz;
    self->buf_sz = buf_sz;

    // zero this guy to be safe
    memset(self->buf, 0, sizeof(float) * buf_sz);

    self->buf_mask_ = self->buf_sz - 1;
    self->write_ptr_ = 0;
    self->write_counter_ = 0;
}

bool stft_tick(stft* self, dft_complex* frame_out, float* real_in) {
    for (uint32_t i = 0; i < self->hop_sz; i++) {
        self->buf[self->write_ptr_] = real_in[i];
        self->write_ptr_ = (self->write_ptr_ + 1) & self->buf_mask_;
    }

    if (self->write_counter_ >= self->fft_sz) {
        float tmp[self->fft_sz];

        for (uint32_t i = 0; i < self->fft_sz; i++) {
            uint32_t idx = (self->write_ptr_ - self->hop_sz + i) & self->buf_mask_;
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
                uint32_t fft_sz,
                uint32_t hop_sz,
                uint32_t ola_sz) {
    self->idft = idft;
    self->inverse = inverse;
    self->ola = ola;
    self->win = win;
    self->ola_norm = ola_norm;
    self->fft_sz = fft_sz;
    self->hop_sz = hop_sz;
    self->ola_sz = ola_sz;

    // zero this guy to be safe
    memset(self->ola, 0, sizeof(float) * ola_sz);

    self->ola_mask_ = ola_sz - 1;
    self->read_ptr_ = 0;
    self->write_ptr_ = 0;
}

void istft_tick(istft* self, float* real_out, dft_complex* frame_in) {
    // do inverse fft
    float tmp[self->fft_sz];
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
