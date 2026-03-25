
#include <string.h>

#include <dsp/conv.h>

static inline float tick_one_(dconv* self, float xn) {
    float out = 0.0f;

    int j = self->wptr_ & self->mask_;
    self->dline_[j] = xn;

    for (uint32_t k = 0; k < self->ir_sz; k++, j--) {
        out += self->ir[k] * self->dline_[j & self->mask_];
    }
    self->wptr_++;

    return out;
}

void dconv_init(dconv* self,
                float* ir,
                uint32_t ir_sz,
                float* dline_,
                int32_t dline_sz_) {
    self->dline_ = dline_;
    self->dline_sz_ = dline_sz_;
    self->ir = ir;
    self->ir_sz = ir_sz;
    self->rptr_ = 0;
    self->wptr_ = 0;
    self->mask_ = dline_sz_ - 1;
}

void dconv_tick_block(dconv* self,
                      float* out,
                      float* in,
                      uint32_t start,
                      uint32_t nsmps) {

    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = tick_one_(self, in[i]);
    }
}

void mpconv_kernel_init(mpconv_kernel* self,
                        float* block,
                        float* ola,
                        dft_complex* h,
                        dft_complex* dline,
                        dft_complex* y,
                        rdft* rfft,
                        irdft* irfft,
                        mpconv_kernel_sz kernel_sz) {
    self->block = block;
    self->ola = ola;
    self->h = h;
    self->dline = dline;
    self->y = y;
    self->rfft = rfft;
    self->irfft = irfft;
    self->kernel_sz = kernel_sz;
    self->ptr = 0;
}

void mpconv_kernel_tick_frame(mpconv_kernel* self, float* out, float* in) {

    uint32_t frame_sz = self->kernel_sz.frame_sz;          // L
    uint32_t block_sz = self->kernel_sz.block_sz;          // N
    uint32_t n_partitions = self->kernel_sz.n_partitions;  // P
    uint32_t h_sz = self->kernel_sz.h_sz;                  // N/2 + 1

    // zero pad
    memset(self->block, 0, block_sz * sizeof(float));
    memcpy(self->block, in, frame_sz * sizeof(float));

    memset(self->y, 0, h_sz * sizeof(dft_complex));

    // add next frame into the spectral delay line
    self->forward_fft(self->rfft, self->dline + self->ptr * h_sz, self->block);

    // accumulate y
    for (uint32_t i = 0; i < n_partitions; i++) {
        uint32_t dl_idx = ((self->ptr + i) % n_partitions) * h_sz;
        uint32_t h_idx =
            (n_partitions - i - 1) * h_sz;  // look back this number of frames
        dft_complex_mult_frame_accumulate(self->y,
                                          (const dft_complex*) self->dline + dl_idx,
                                          (const dft_complex*) self->h + h_idx, h_sz);
    }
    // inverse
    self->inverse_fft(self->irfft, self->block, self->y);

    // overlap add
    for (uint32_t i = 0; i < frame_sz; i++) {
        out[i] = self->block[i] + self->ola[i];
    }

    for (uint32_t i = 0; i < frame_sz; i++) {
        self->ola[i] = self->block[i + frame_sz];
    }

    self->ptr = (self->ptr + 1) % n_partitions;
}

void mpconv_init(mpconv* self,
                 mpconv_kernel* kernel,
                 fifo_f* in_q,
                 fifo_f* out_q,
                 float* frame,
                 float* y) {
    self->kernel = kernel;
    self->in_q = in_q;
    self->out_q = out_q;
    self->frame_ = frame;
    self->y_ = y;
}

void mpconv_tick_block(mpconv* self,
                       float* out,
                       float* in,
                       uint32_t start,
                       uint32_t nsmps) {

    uint32_t frame_sz = self->kernel->kernel_sz.frame_sz;

    for (uint32_t i = start; i < nsmps; i++) {
        fifo_push(self->in_q, in[i]);

        if (fifo_n(self->in_q) >= frame_sz) {
            float* frame = self->frame_;
            float* y = self->y_;

            if (fifo_pop_block(self->in_q, frame, frame_sz)) {
                mpconv_kernel_tick_frame(self->kernel, y, frame);
            }
            fifo_push_block(self->out_q, y, frame_sz);
        }

        float o;
        if (fifo_pop(self->out_q, &o)) {
            out[i] = o;
        } else {
            out[i] = 0.0f;
        }
    }
}

static inline void mix_signal(float* out, float* in, uint32_t nsmps) {
    for (uint32_t i = 0; i < nsmps; i++) {
        out[i] += in[i];
    }
}

void zconv_init(zconv* self,
                dconv* head,
                mpconv* mid,
                mpconv* tail,
                float* tmp_out_,
                uint32_t tmp_sz_) {
    self->head = head;
    self->mid = mid;
    self->tail = tail;
    self->tmp_out_ = tmp_out_;
    self->tmp_sz_ = tmp_sz_;
}

// NOTE: out MUST be the same size as tmp_sz which should be set and enforced
// in the caller during the programs lifetime. Essentially this algorithm is best
// used in environments where the ksmps is guaranteed not to change ..
//
// it might be worth considering enforcing alignment on block boundaries (no start)
void zconv_tick_block(zconv* self,
                      float* out,
                      float* in,
                      uint32_t start,
                      uint32_t nsmps) {

    float* tmp_out = self->tmp_out_;
    memset(out, 0, sizeof(float) * self->tmp_sz_);
    memset(tmp_out, 0, sizeof(float) * self->tmp_sz_);

    dconv_tick_block(self->head, out, in, start, nsmps);

    mpconv_tick_block(self->mid, tmp_out, in, start, nsmps);
    mix_signal(out, tmp_out, nsmps);

    mpconv_tick_block(self->tail, tmp_out, in, start, nsmps);
    mix_signal(out, tmp_out, nsmps);
}
