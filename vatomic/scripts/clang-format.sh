#!/bin/sh
# Copyright (C) Huawei Technologies Co., Ltd. 2024-2025. All rights reserved.
# SPDX-License-Identifier: MIT
set -e

if [ $# -lt 1 ]; then
    echo "Usage: $0 <PATH> [PATH ...]"
    echo ""
    echo "  PATH                one or more directories (or source files) to recursively run "
    echo "                      clang-format on."
    echo "Environment variables:"
    echo "  SILENT=true         disable git diff and error code"
    echo "  STYLE=FILE          FILE is a configuration file, default = .clang-format"
    echo "  "
    echo ""
    exit 1
fi

if [ "${STYLE}" != "" ]; then
    STYLE=":${STYLE}"
fi

# Apply clang-format to all *.h and *.c files in src folder.
find "$@" \( -name '*.h' -o -name '*.c' -o -name '*.cpp' -o -name '*.hpp' -o -name '*.cxx' \) \
    -not -path '*/build/*' \
    -not -path '*/deps/*' \
    -type f \
    -exec clang-format -style=file${STYLE} -i {} +

if [ "${SILENT}" != "true" ]; then
    # Display changed files and exit with 1 if there were differences.
    git --no-pager diff --exit-code
fi
