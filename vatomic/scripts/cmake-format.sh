#!/bin/sh
# Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
# SPDX-License-Identifier: MIT
set -e

if [ $# -lt 1 ]; then
    echo "Usage: $0 <PATH> [PATH ...]"
    echo ""
    echo "  PATH                one or more directories (or source files) to recursively run "
    echo "                      cmake-format on."
    echo "Environment variables:"
    echo "  SILENT=true         disable git diff and error code"
    echo "  STYLE=FILE          FILE is a configuration file, default = .cmake-format"
    echo "  "
    echo ""
    exit 1
fi

if [ -z "${STYLE}" ]; then
    STYLE=".cmake-format"
fi

if [ -z "${IGNORE}" ]; then
    IGNORE=.cmake-format.ignore
fi

# check if the ignore file really exists otherwise do not use it
if [ -f $IGNORE ]; then
    # Apply cmake-format to all committed CMake files in the repo.
    git ls-files "$@" |
        grep -E 'CMakeLists.txt$|.*\.cmake(\.in)?$' |
        grep -E --invert-match -f "${IGNORE}" |
        xargs cmake-format -c "${STYLE}" -i
else
    echo -e "\e[33mWarning: $IGNORE does not exist!\e[0m"
    git ls-files "$@" |
        grep -E 'CMakeLists.txt$|.*\.cmake(\.in)?$' |
        xargs cmake-format -c "${STYLE}" -i
fi

if [ "${SILENT}" != "true" ]; then
    # Display changed files and exit with 1 if there were differences.
    git --no-pager diff --exit-code
fi
