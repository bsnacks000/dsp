/**
 * @brief fifo_f - a float fifo using a ring buffer.
 *
 */

#ifndef DSP_FIFO_H
#define DSP_FIFO_H

#include <stdbool.h>
#include <stdint.h>

#include <dsp/assert.h>

/**
 * @brief simple fifo backed by ring buffer.
 */
typedef struct {
    float* ring;
    uint32_t ring_sz;
    // internal
    uint32_t rptr_, wptr_, mask_;
} fifo_f;

/**
 * @brief init a fifo. set cap_ to the size of the ring buffer.
 */
static inline void fifo_init(fifo_f* self, float* ring, uint32_t ring_sz) {
    dsp_assert((ring_sz & (ring_sz - 1)) == 0, "fifo_init: ring_sz must be pow2.");
    self->ring = ring;
    self->ring_sz = ring_sz;
    self->rptr_ = self->wptr_ = 0;
    self->mask_ = ring_sz - 1;
}

/**
 * @brief number of items in the fifo.
 */
static inline uint32_t fifo_n(fifo_f* self) {
    return self->wptr_ - self->rptr_;
}

/**
 * @brief available space in the fifo.
 */
static inline uint32_t fifo_avail(fifo_f* self) {
    return self->ring_sz - fifo_n(self);
}

/**
 * @brief push an item onto the end of the fifo and return true.
 * If we are full return false.
 */
static inline bool fifo_push(fifo_f* self, float val) {
    if (!fifo_avail(self)) {
        return false;
    }
    self->ring[self->wptr_ & self->mask_] = val;
    self->wptr_++;
    return true;
}

/**
 * @brief remove an item from the top of the fifo and return true.
 * If we are empty return false.
 */
static inline bool fifo_pop(fifo_f* self, float* out) {
    if (!fifo_n(self)) {
        return false;
    }
    *out = self->ring[self->rptr_ & self->mask_];
    self->rptr_++;

    return true;
}

/**
 * @brief push a block onto the fifo and return true. If there is not enough
 * space for the entire block (determined by buf_sz) then return false. This means
 * no partial writes.
 */
static inline bool fifo_push_block(fifo_f* self,
                                   const float* block,
                                   uint32_t block_sz) {
    if (fifo_avail(self) < block_sz) {
        return false;
    }

    for (uint32_t i = 0; i < block_sz; i++) {
        self->ring[self->wptr_ & self->mask_] = block[i];
        self->wptr_++;
    }

    return true;
}

/**
 * @brief pop a block off the fifo and return true. If there are not enough items in
 * the fifo to fill the entire block return false. Like push no partial writes.
 */
static inline bool fifo_pop_block(fifo_f* self, float* block, uint32_t block_sz) {
    if (fifo_n(self) < block_sz) {
        return false;
    }

    for (uint32_t i = 0; i < block_sz; i++) {
        block[i] = self->ring[self->rptr_ & self->mask_];
        self->rptr_++;
    }

    return true;
}

#endif
