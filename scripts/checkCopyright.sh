#!/bin/sh


PATTERN="BEGIN_COMMON_COPYRIGHT_HEADER"
EXCLUDE='--exclude-dir src/qxt '
EXCLUDE='--exclude-dir librazor-upower '$EXCLUDE

DIR=$1
if [ -z "${DIR}" ]; then
    DIR='.'
fi

grep  --recursive ${EXCLUDE} --include "*.h" --include "*.cpp" --files-without-match  ${PATTERN} ${DIR}
