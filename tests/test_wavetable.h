#ifndef DSP_TEST_WAVETABLE
#define DSP_TEST_WAVETABLE

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/wavetable/wavetable.h>
#include "munit.h"

MunitResult test_wavetable_init(const MunitParameter params[], void* data);

MunitResult test_wavetable_shallow_copy(const MunitParameter params[], void* data);

MunitResult test_wavetable_deep_copy(const MunitParameter params[], void* data);

MunitResult test_wavetable_write(const MunitParameter params[], void* data);

MunitResult test_wavetable_func(const MunitParameter params[], void* data);

MunitResult test_wavetable_add_guard_point(const MunitParameter params[], void* data);

MunitResult test_normalize(const MunitParameter params[], void* data);

// register tests
static MunitTest test_wavetable_module[] = {
    {"/wavetable_init", test_wavetable_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/wavetable_shallow_copy", test_wavetable_shallow_copy, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/wavetable_deep_copy", test_wavetable_deep_copy, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/wavetable_write", test_wavetable_write, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/wavetable_func", test_wavetable_func, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/wavetable_add_guard_point", test_wavetable_add_guard_point, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},

    {"/normalize", test_normalize, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    // need this NULL stub or we segfault
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

// entrypoint
static const MunitSuite wavetable_test_suite = {
    "/wavetable",           /* name */
    test_wavetable_module,  /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif
#endif
