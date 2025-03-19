#!/bin/bash
set -e

# This script fixes code formatting issues, which are not covered by normal clang-format.
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
# - Add new-line at the end of file, if missing. It is checked by clang-format but not added
# - Add copyright notice if missing or fix the year range if exists
# - Check if forbidden types are used.

echo "Running sanatize on $(pwd)"

FILES=$(git ls-files '*.h' '*.c' | grep -v "/build/" | grep -v "tmplr/" | grep -vE "^$(git ls-files -d | paste -sd "|" -)$")

#YEAR=$(date +%Y)

# copyright notice
COPYRIGHT_TEXT="\
/*\n\
 * Copyright (C) Huawei Technologies Co., Ltd. <RANGE>. All rights reserved.\n\
 * SPDX-License-Identifier: MIT\n\
 */\n\n"

declare -a VTYPE_HEADERS=("<vsync/vstdint.h>" "<stdint.h>" "<stdbool.h>" "<inttypes.h>" "<stdbool.h>" "<stddef.h>")
declare -a VTYPE_TYPES=("size_t" "bool")

for f in ${FILES}; do
    # echo $f
    if [[ "$f" == include/vsync/* ]] || [[ "$f" == test/atomics_gen/* ]] ; then
        # insert space after /* if missing
        sed -i -e 's|\/\*\([^ *]\)|/* \1|g' $f

        # insert space before */ if missing
        sed -i -e 's|\([^ *]\)\*\/|\1 */|g' $f

        # insert space after .*// if missing (not starting line)
        sed -i -e 's|\([^:]\)\/\/\([^\/ ]\)|\1// \2|g' $f

        # insert space after ^// if missing (starting line)
        sed -i -e 's|^\/\/\([^\/ ]\)|// \1|g' $f

        # remove blank lines at the end of code block defined by braces
        sed -i -r -z 's|\n+(\n\s*})|\1|g' $f

        # remove consequtive empty lines
        sed -i 'N;/^\n$/D;P;D;' $f

        # add newline at end of file
        sed -i -e '$a\' ${f}
    fi

    # add copy write notice to public files

    EXP_END=$(git log --follow --format=%ad --date=format:'%Y' $f | head -1)
    EXP_START=$(git log --follow --format=%ad --date=format:'%Y' $f | tail -1)

    if [[ "$EXP_START" == "$EXP_END" ]]; then
        REPLACEMENT="$EXP_START"
    else
        REPLACEMENT="$EXP_START-$EXP_END"
    fi

    # The echo empty is neseccary for the case when there is no copyrights notice in the file
    EXISTING_COPYRIGHT=$(grep Copyright ${f} || echo "")

    if [ -z "$EXISTING_COPYRIGHT" ]; then
        NOTICE="${COPYRIGHT_TEXT/<RANGE>/$REPLACEMENT}"
        echo "Adding copyrights to $f"
        sed -i "1s;^;${NOTICE};" ${f}
    else
        REGEX="Ltd\. ([0-9]+)\-?([0-9]+)?"
        if [[ $EXISTING_COPYRIGHT =~ $REGEX ]]; then
            CUR_START="${BASH_REMATCH[1]}"
            CUR_END="${BASH_REMATCH[2]}"

            if [ -z "$CUR_END" ]; then
                ORIGINAL=$CUR_START
            else
                ORIGINAL="$CUR_START-$CUR_END"
                if [[ $CUR_START -lt $EXP_START ]]; then
                    REPLACEMENT="$CUR_START-$EXP_END"
                fi
            fi
        else
            echo "[ERROR] there is copyrights without year digits!! in $f"
            exit 1
        fi

        if [[ $REPLACEMENT != $ORIGINAL ]]; then
            echo "Updating copyrights notice in ${f} ${ORIGINAL} ==> ${REPLACEMENT}"
            sed -i 's|Ltd. '${ORIGINAL}'\. All|Ltd. '${REPLACEMENT}'. All|g' ${f}
        fi
    fi

    if [[ "$f" == include/vsync/* ]]; then
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
                    echo "Forbidden type $type is used in file $f"
                    exit 1
                fi
            done
        fi
    fi

    # check if there is a header guard in header files
    if [[ "$f" == *".h"* ]]; then
        HEADER_GUARD=$(grep "#ifndef.*_H" ${f} || echo "")
        if [ -z "$HEADER_GUARD" ]; then
            echo "$f does not seem to have a header guard ending with _H"
            exit 1
        fi
    fi

done
