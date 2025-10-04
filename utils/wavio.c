
#include <sndfile.h>
#include <stdlib.h>
#include <string.h>

#include "wavio.h"

/**
 * @brief open sf for write. This will always write a wav float32 format. Only works
 * with mono or stereo files.
 * NOTE: allocates memory for wavio struct via allocator.
 */
void wavio_open_write(wavio* self,
                      void* (*alloc)(uint64_t nbytes),
                      const char* path,
                      int sr,
                      int nchns,
                      uint32_t nframes) {

    memset(self, 0, sizeof(*self));
    self->info.samplerate = sr;
    self->info.channels = nchns;
    self->info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;

    self->nframes = nframes;
    self->block_sz = nframes * nchns;
    self->block_nbytes = sizeof(float) * self->block_sz;

    self->block = (float*) alloc(sizeof(float) * nframes * self->info.channels);
    if (self->block == NULL) {
        fprintf(stderr, "alloc failed.");
        exit(EXIT_FAILURE);
    }

    if ((self->handle = sf_open(path, SFM_WRITE, &self->info)) == NULL) {
        fprintf(stderr, "sf_open: %s\n", sf_strerror(self->handle));
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief open sf for read. Supports wav and aiff PCM/float formats. Only works with
 * mono or stereo files.
 * NOTE: allocates memory for wavio struct via allocator.
 */
void wavio_open_read(wavio* self,
                     void* (*alloc)(uint32_t nbytes),
                     const char* path,
                     uint32_t nframes) {

    memset(self, 0, sizeof(*self));
    if ((self->handle = sf_open(path, SFM_READ, &self->info)) == NULL) {
        fprintf(stderr, "sf_open: %s\n", sf_strerror(self->handle));
        exit(EXIT_FAILURE);
    }

    self->nframes = nframes;
    self->block_sz = nframes * self->info.channels;
    self->block_nbytes = sizeof(float) * self->block_sz;

    self->block = (float*) alloc(sizeof(float) * nframes * self->info.channels);

    if (self->block == NULL) {
        fprintf(stderr, "alloc failed.");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief fill wavio with one block of interleaved audio.
 * NOTE: in_sz must equal self->block_sz;
 */
void wavio_fill_block(wavio* self, const float* in) {
    memcpy(self->block, in, self->block_nbytes);
}

/**
 * @brief copy one block (nframes) of interleaved audio into out.
 * NOTE: out_sz must equal self->block_sz;
 */
void wavio_copy_block(wavio* self, float* out) {
    memcpy(out, self->block, self->block_nbytes);
}

/**
 * @brief write nframes of block into the file handle.
 */
void wavio_write_block(wavio* self) {
    sf_count_t count;
    if ((count = sf_writef_float(self->handle, self->block, self->nframes) !=
                 self->nframes)) {
        fprintf(stderr, "sf_write: %s\n", sf_strerror(self->handle));
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief read nframes from the file into block.
 */
sf_count_t wavio_read_block(wavio* self) {
    memset(self->block, 0, self->block_nbytes);
    return sf_readf_float(self->handle, self->block, self->nframes);
}

/**
 * @brief cleanup the wavio object using dealloc.
 */
void wavio_close(wavio* self, void (*dealloc)(void*)) {
    dealloc(self->block);
    int err;
    if ((err = sf_close(self->handle)) != 0) {
        fprintf(stderr, "sf_close: %s\n", sf_strerror(self->handle));
        exit(EXIT_FAILURE);
    }
}

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
