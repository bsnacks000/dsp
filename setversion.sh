#!/usr/bin/env bash
set -euo pipefail

make build

# path to generated header
VERSION_HEADER="./build/include/dsp/version.h"

# extract DSP_VERSION string using awk
DSP_VERSION=$(awk -F'"' '/#define DSP_VERSION/ && $2 != "" {print $2}' "$VERSION_HEADER")

if [[ -z "$DSP_VERSION" ]]; then
    echo "Error: DSP_VERSION not found in $VERSION_HEADER"
    exit 1
fi

echo "Tag version: $DSP_VERSION"

# create annotated tag
git tag -a "$DSP_VERSION" -m "$DSP_VERSION"

# push tag to origin
git push origin "$DSP_VERSION"
