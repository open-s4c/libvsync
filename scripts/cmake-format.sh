#!/bin/sh

set -e
set -x

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
    IGNORE=".format.ignore"
fi

if [ ! -f "${IGNORE}" ]; then
    echo "No ignore file"
    exit 1
fi
IGNORE_GREP="grep -E --invert-match -f ${IGNORE}"

git ls-files "$@" |
    grep -E 'CMakeLists.txt$|.*\.cmake(\.in)?$' |
    ${IGNORE_GREP} |
    xargs cmake-format -c "${STYLE}" -i

if [ "${SILENT}" != "true" ]; then
    # Display changed files and exit with 1 if there were differences.
    git --no-pager diff --exit-code
fi
