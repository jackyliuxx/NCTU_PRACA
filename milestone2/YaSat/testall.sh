#!/bin/bash
SDIR="Sample_benchmarks_for_Milestone_2"
if [ -n "$1" ]; then
    SDIR="$1"
fi
find $SDIR -name '*.cnf' -exec time -p bash -c 'ls {} && ./yasat {}' \;
