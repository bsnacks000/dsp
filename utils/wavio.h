/**
 * @brief sndfile utils.
 * - Reads and writes mono and stereo signals. Buffers for stereo seignals are always
 *  interleaved and calculated as nchans * buf_sz -> [l,r,l,r,l,r, ...]
 * - sndfile errors can be handled in the caller.
 * - read -> copy | fill -> write
 */
#ifndef DSP_WAVIO_H
#define DSP_WAVIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sndfile.h>

/**
 * @brief hold sndfile handle and info.
 */
typedef struct {
    SNDFILE* handle;
    SF_INFO info;
    uint32_t nframes;       // this is the kblock size
    float* block;           // interleaved block
    uint32_t block_sz;      // interleaved block sz (kvec_sz * nchans)
    uint32_t block_nbytes;  // sizeof (float) * blk_sz
} wavio;

/**
 * @brief open sf for write. This will always write a wav float32 format. Only works
 * with mono or stereo files.
 * NOTE: allocates memory for wavio struct via allocator.
 */
void wavio_open_write(wavio* self,
                      void* (*alloc)(uint64_t nbytes),
                      const char* path,
                      int sr,
                      int chans,
                      uint32_t nframes);

/**
 * @brief open sf for read. Supports wav and aiff PCM/float formats. Only works with
 * mono or stereo files.
 * NOTE: allocates memory for wavio struct via allocator.
 */
void wavio_open_read(wavio* self,
                     void* (*alloc)(uint32_t nbytes),
                     const char* path,
                     uint32_t nframes);

/**
 * @brief fill wavio with one block of interleaved audio.
 * NOTE: caller must guarentee in_sz == self->block_sz;
 */
void wavio_fill_block(wavio* self, const float* in);

/**
 * @brief copy one block (nframes) of interleaved audio into out.
 * NOTE: caller must guarentee out_sz == self->block_sz;
 */
void wavio_copy_block(wavio* self, float* out);

/**
 * @brief write the current block.
 */
void wavio_write_block(wavio* self);

/**
 * @brief read the next block. If sf_count_t returns 0 we have reached EOF. The
 * second to last read may return a partial block. Depending on the application may
 * have to pad zeros (or should have already!) on a partial.
 */
sf_count_t wavio_read_block(wavio* self);

/**
 * @brief cleanup the wavio object using dealloc. Fails fast on sf_close error.
 */
void wavio_close(wavio* self, void (*dealloc)(void*));

/**
 * @brief mux N chans into 1 interleaved signal.
 * Makes the following assumptions:
 *  1. interleaved size (nframes) == sum of the length of all channels
 *  2. each channel is the same size
 *  3. size of interleaved is divisible by num of channels
 */
void wavio_mux(float** channels, float* interleaved, uint32_t nchns, uint32_t nframes);

/**
 * @brief demux an interleaved signal into multiple channels.
 * Makes the following assumptions:
 *  1. interleaved size (nframes) == sum of the length of all channels
 *  2. each channel is the same size
 *  3. size of interleaved is divisible by num of channels
 */
void wavio_demux(float* interleaved,
                 float** channels,
                 uint32_t nchns,
                 uint32_t nframes);

#ifdef __cplusplus
}
#endif

#endif
