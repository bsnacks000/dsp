
#ifndef DSP_DFT_H
#define DSP_DFT_H

#ifdef __cpluplus
extern "C" {
#endif

#include <stdint.h>

typedef struct rdft rdft;
typedef struct irdft irdft;

typedef float dft_complex[2];

/**
 * @brief create rdft storage for size n real numbers and size n/2 + 1 complex frame
 * size. n must be a power of 2.
 */
typedef rdft* (*rdft_create)(uint32_t n);

/**
 * @brief execute the forward dft. Most implementations will expect
 * the complex result to be scaled by 1/n.
 */
typedef void (*rdft_execute)(rdft* self, dft_complex* c_out, float* r_in);

/**
 * @brief destroy the rdft object and clean up its memory.
 */
typedef void (*rdft_destroy)(rdft* self);

/**
 * @brief create irdft storage of size n real numbers and size n/2 + 1 complex frame.
 * n should be a power of 2. This object should be paired with a rdft object.
 */
typedef irdft* (*irdft_create)(uint32_t n);

/**
 * @brief execute the inverse dft. Most implementations will not expect the complex
 * result to be scaled.
 */
typedef void (*irdft_execute)(irdft* self, float* r_out, dft_complex* c_in);

/**
 * @brief destroy the irdft object and clean up its memory.
 */
typedef void (*irdft_destroy)(irdft* self);

#ifdef __cpluplus
}
#endif

#endif
