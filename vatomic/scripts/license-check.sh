#!/usr/bin/env bash
# Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
# SPDX-License-Identifier: MIT
set -e

# Adds copyright notice if missing and fix the year range if necessary

if [ -f "$1" ]; then
    FILES=$1
else
    DIR=$1
    if [ -z "$DIR" ]; then
        DIR=.
    fi
    FILES=$(find $DIR\
        -name '*.h' -o \
        -name '*.hpp' -o \
        -name '*.c' -o \
        -name '*.cpp' -o \
        -name '*.c.in' -o \
        -name '*.cpp.in' -o \
        -name '*.h.in' -o \
        -name '*.hpp.in' -o \
        -name '*.rs' -o \
        -name '*.bpl' -o \
        -name '*.cmake.in' -o \
        -name 'CMake*.txt' -o \
        -name '*.cmake' | grep -v "build" | grep -v "examples")
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

COPYRIGHT_TEXT_RUST="\
/*\n\
 * Copyright (C) Huawei Technologies Co., Ltd. <RANGE>. All rights reserved.\n\
 * SPDX-License-Identifier: MIT\n\
 */\n"
COPYRIGHT_TEXT_CMAKE="\
\# Copyright (C) Huawei Technologies Co., Ltd. <RANGE>. All rights reserved.\n\
\# SPDX-License-Identifier: MIT\n\n"
COPYRIGHT_TEXT_BOOGIE="\
/*\n\
 * Copyright (C) Huawei Technologies Co., Ltd. <RANGE>. All rights reserved.\n\
 * SPDX-License-Identifier: MIT\n\
 */\n"
COPYRIGHT_TEXT_C="\
/*\n\
 * Copyright (C) Huawei Technologies Co., Ltd. <RANGE>. All rights reserved.\n\
 * SPDX-License-Identifier: MIT\n\
 */\n"

for f in ${FILES}; do
    echo CHECK: ${f}

    fname=$(basename -- "${f}")
    ext="${fname##*.}"
    if [ "$ext" = "txt" ] || echo $fname | grep cmake > /dev/null; then
        COPYRIGHT_TEXT="$COPYRIGHT_TEXT_CMAKE"
    elif [ "$ext" = "rs" ]; then
        COPYRIGHT_TEXT="$COPYRIGHT_TEXT_RUST"
    elif [ "$ext" = "bpl" ]; then
        COPYRIGHT_TEXT="$COPYRIGHT_TEXT_BOOGIE"
    else
        COPYRIGHT_TEXT="$COPYRIGHT_TEXT_C"
    fi

    # add copyright notice
    EXP_END=$(git log --follow --format=%ad --date=format:'%Y' $f | head -1)
    EXP_START=$(git log --follow --format=%ad --date=format:'%Y' $f | tail -1)

    if [[ "$EXP_START" == "$EXP_END" ]]; then
        REPLACEMENT="$EXP_START"
    else
        REPLACEMENT="$EXP_START-$EXP_END"
    fi

    # Update (if necessary) date of the first (top-most) copyright line
    # The echo empty is necessary for the case when there is no copyright
    # notice in the file
    COPYRIGHT=$(grep Copyright ${f} | head -n1 || true)

    if [ -z "$COPYRIGHT" ]; then
        NOTICE="${COPYRIGHT_TEXT/<RANGE>/$REPLACEMENT}"
        echo "Adding copyright notice to $f"
        $SED -i "1s;^;${NOTICE};" ${f}
    else
        REGEX="Copyright.* ([0-9]+)\-?([0-9]+)?"
        if [[ "$COPYRIGHT" =~ $REGEX ]]; then
            CUR_START="${BASH_REMATCH[1]}"
            CUR_END="${BASH_REMATCH[2]}"

            if [ -z "$CUR_END" ]; then
                ORIGINAL=$CUR_START
                if [[ $CUR_START -lt $EXP_END ]]; then
                    REPLACEMENT="$CUR_START-$EXP_END"
                fi
            else
                ORIGINAL="$CUR_START-$CUR_END"
                if [[ $CUR_START -lt $EXP_START ]]; then
                    REPLACEMENT="$CUR_START-$EXP_END"
                fi
            fi
        else
            echo "[ERROR] copyright without year in $f"
            exit 1
        fi

        if [[ $REPLACEMENT != $ORIGINAL ]]; then
            echo "Updating copyrights notice in ${f} ${ORIGINAL} ==> ${REPLACEMENT}"
            $SED -i 's|\(.*Copyright.*\) '${ORIGINAL}'|\1 '${REPLACEMENT}'|g' ${f}
        fi
    fi

    # Ensure file has SDPX line after the *last* Copyright line
    if ! grep SPDX $f > /dev/null; then
        echo "Adding license to ${f}"
        PREFIX="$(echo "$COPYRIGHT" | sed 's/^\(.*\)Copyright.*$/a\\\n\1/')"
        LINE=$(grep -n Copyright ${f} | tail -1 | cut -d: -f1)
        $SED -i "${LINE}${PREFIX}SPDX-License-Identifier: MIT" ${f} | head -n 10
    fi
done
