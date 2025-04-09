#!/usr/bin/env bash
# Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
# SPDX-License-Identifier: MIT
set -e

# This script fixes code formatting issues, which are not covered by normal
# clang-format, and which might trigger warnings in the pipeline.
#
# The script fixes:
#
# - Comment characters should have spaces preceding/following them
# //comment => // comment
# /*comment*/ => /* comment */
#
# - Code blocks should not end with newline before braces
#
#  if(X) {               =>  if(X) {
#    return true;        =>    return true;
#                        =>   }
#  }
#
# - Add new-line at the end of file, if missing. That is checked by clang-format
#   but not added
# - Check if "forbidden" types are used.

if [ -f "$1" ]; then
    FILES=$1
else
    DIR=$1
    if [ -z "$DIR" ]; then
        DIR=.
    fi
    FILES=$(find $DIR\
        -name '*.h' -o \
        -name '*.c' -o \
        -name '*.c.in' -o \
        -name '*.h.in' | grep -v "build")
fi

# we use GNU sed
SED=sed
if ! sed --version > /dev/null 2>&1; then
    # if this is not GNU sed, try gsed
    if gsed --version > /dev/null 2>&1; then
        SED=gsed
    else
        echo "This script requires GNU sed"
        exit 1
    fi
fi

declare -a VTYPE_HEADERS=("<vsync/vstdint.h>" "<stdint.h>" "<stdbool.h>" "<inttypes.h>" "<stdbool.h>" "<stddef.h>")
declare -a VTYPE_TYPES=("size_t" "bool")

for f in ${FILES}; do
    echo SANITIZE: ${f}

    # insert space after /* if missing
    $SED -i -e 's|\/\*\([^ *]\)|/* \1|g' $f

    # insert space before */ if missing
    $SED -i -e 's|\([^ *]\)\*\/|\1 */|g' $f

    # insert space after .*// if missing (not starting line)
    $SED -i -e 's|\([^:]\)\/\/\([^\/ ]\)|\1// \2|g' $f

    # insert space after ^// if missing (starting line)
    $SED -i -e 's|^\/\/\([^\/ ]\)|// \1|g' $f

    # remove blank lines at the end of code block defined by braces
    $SED -i -r -z 's|\n+(\n\s*})|\1|g' $f

    # remove consequtive empty lines
    $SED -i 'N;/^\n$/D;P;D;' $f

    # add newline at end of file
    $SED -i -e '$a\' ${f}

    # check if all files that are not vtypes.h do not include forbidden headers
    if [[ "$f" != *"vsync/vtypes.h" ]]; then
        for header in "${VTYPE_HEADERS[@]}"; do
            if grep "#include $header" ${f} >/dev/null; then
                echo "ERROR use #include <vsync/vtypes.h> instead of #include $header in $f"
                exit 1
            fi
        done
    fi

    # check for usage of forbidden types
    if [[ "$f" != *"vsync/vtypes.h" ]]; then
        for type in "${VTYPE_TYPES[@]}"; do
            if grep -w $type ${f} >/dev/null; then
                echo "Forbidden type $type is in file $f"
                exit 1
            fi
        done
    fi
done
