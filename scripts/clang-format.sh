#!/bin/sh

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

if [ -z "${IGNORE}" ]; then
    IGNORE=".format.ignore"
fi

if [ ! -f "${IGNORE}" ]; then
    echo "No ignore file"
    exit 1
fi
IGNORE_GREP="grep -E --invert-match -f ${IGNORE}"

git ls-files "$@" |
    grep -E '(\.c|\.cpp|\.h|\.hpp|\.h\.in|\.hpp\.in)$' |
    ${IGNORE_GREP} |
    xargs clang-format -style=file${STYLE} -i

if [ "${SILENT}" != "true" ]; then
    # Display changed files and exit with 1 if there were differences.
    git --no-pager diff --exit-code
fi
