#!/usr/bin/env python

import argparse
import pathlib


class Template:
    def __init__(self, name: str, prefix: str = ""):
        self.name = name
        self.prefix = prefix

    def template(self) -> str: ...


class Header(Template):
    def template(self) -> str:
        prefix = self.prefix.upper()
        upper_name = self.name.upper()
        return f"""
#ifndef {prefix}_TEST_{upper_name}_H
#define {prefix}_TEST_{upper_name}_H

#ifdef __cplusplus
extern "C" {{
#endif

#include "munit.h"

MunitResult test_something(const MunitParameter params[], void* data);


static MunitTest {self.name}_test_module[] = {{
    {{"/something", test_something, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }},
    {{NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}},
}};

static const MunitSuite {self.name}_test_suite = {{
    "/curve",               /* name */
    {self.name}_test_module,      /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
}};


#ifdef __cplusplus
}}
#endif
#
#endif
"""


class TranslationUnit(Template):
    def template(self) -> str:
        return f"""

#include "test_{self.name}.h"

MunitResult test_something(const MunitParameter params[], void* data){{
    (void) params;
    (void) data;

    return MUNIT_OK;
}}
"""


def main(
    dir: pathlib.Path,
    prefix: str,
    name: str,
):
    dir = dir.resolve()

    if not dir.is_dir():
        raise OSError("dir: not a dir")

    if not dir.exists():
        raise OSError("dir: does not exist.")

    header_location = dir / f"test_{name}.h"
    trunit_location = dir / f"test_{name}.c"

    header = Header(name, prefix)
    trunit = TranslationUnit(name, prefix)

    with open(header_location, "w") as f:
        f.write(header.template())

    with open(trunit_location, "w") as f:
        f.write(trunit.template())


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Make an munit test module.")
    parser.add_argument(
        "-d",
        "--dir",
        type=pathlib.Path,
        required=True,
        help="relpath to output directory.",
    )
    parser.add_argument(
        "-p ",
        "--prefix",
        required=True,
        help="package level header prefix.",
    )
    parser.add_argument(
        "-n",
        "--name",
        required=True,
        help="name of test module",
    )

    args = parser.parse_args()

    main(args.dir, args.prefix, args.name)
