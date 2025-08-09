/***
 * @file assert.h
 * @brief A nicer assert macro
 *
 *
 */
#ifndef DSP_ASSERT_H
#define DSP_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NDEBUG
#    define dsp_assert(cond, msg) ((void) 0)
#else
#    include <stdio.h>
#    include <stdlib.h>
#    include <string.h>
#    define dsp_assert(cond, msg)                                             \
        do {                                                                  \
            if (!(cond)) {                                                    \
                fprintf(stderr,                                               \
                        "Assertion Error: %s\nFile: %s\nFunction: %s\nLine: " \
                        "%d\nMessage: %s\n",                                  \
                        #cond, __FILE__, __func__, __LINE__, msg);            \
                abort();                                                      \
            }                                                                 \
        } while (0)
#endif

#ifdef __cplusplus
}
#endif

#endif
