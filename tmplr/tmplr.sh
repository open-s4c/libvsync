#!/bin/bash
set -e
FILE=$0
FILE=${FILE%.*}
FILE=$(readlink -f ${FILE}).c
PROG=$(mktemp)
/usr/bin/cc -Wall -Werror -o ${PROG} ${FILE} && exec ${PROG} "$@"
