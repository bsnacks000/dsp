#!/bin/bash
set -e

# Extract line coverage percentage
LINE_COV=$(lcov --summary coverage.info | grep "lines.*:" | awk '{print $2}' | tr -d '%')
MIN_COVERAGE=90

if (( $(echo "$LINE_COV < $MIN_COVERAGE" | bc -l) )); then
    echo "ERROR: Coverage $LINE_COV% is below minimum $MIN_COVERAGE%"
    exit 1
fi
