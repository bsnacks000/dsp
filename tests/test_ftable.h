#ifndef DSP_TEST_ftable
#define DSP_TEST_ftable

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/ftable/ftable.h>
#include "munit.h"

MunitResult test_ftable_init(const MunitParameter params[], void* data);

MunitResult test_ftable_shallow_copy(const MunitParameter params[], void* data);

MunitResult test_ftable_deep_copy(const MunitParameter params[], void* data);

MunitResult test_ftable_write(const MunitParameter params[], void* data);

MunitResult test_ftable_func(const MunitParameter params[], void* data);

MunitResult test_ftable_add_guard_point(const MunitParameter params[], void* data);

MunitResult test_normalize(const MunitParameter params[], void* data);

// register tests
static MunitTest test_ftable_module[] = {
    {"/ftable_init", test_ftable_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/ftable_shallow_copy", test_ftable_shallow_copy, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/ftable_deep_copy", test_ftable_deep_copy, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/ftable_write", test_ftable_write, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/ftable_func", test_ftable_func, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/ftable_add_guard_point", test_ftable_add_guard_point, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},

    {"/normalize", test_normalize, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    // need this NULL stub or we segfault
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

// entrypoint
static const MunitSuite ftable_test_suite = {
    "/ftable",              /* name */
    test_ftable_module,     /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

#ifdef __cplusplus
}
#endif
#endif
