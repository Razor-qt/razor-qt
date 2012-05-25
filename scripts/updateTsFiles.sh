#!/bin/bash

function help
{ 
  echo "Usage: updateTsFiles.sh <path-to-source>"
}


OPTS="-noobsolete"

ROOT_DIR=$1
if [ -z "${ROOT_DIR}" ]; then
    ROOT_DIR="../"
fi


for f in `find "$ROOT_DIR" -name translate.sh`; do
    DIR=`dirname $f`
    TARGET=`awk -F"=" '/TARGET=/ {print($2)}' $f | sed -e's/\"//g'`
    PROJECT=`awk -F"=" '/PROJECT=/ {print($2)}' $f | sed -e's/\"//g'`
    TS_FILE=${PROJECT}.ts.src

    (cd ${DIR} &&  \
    cat $TS_FILE > ${TS_FILE}.ts && \
    lupdate $OPTS ${TARGET} -ts ${TS_FILE}.ts && \
    cat $TS_FILE.ts > ${TS_FILE}

    rm  $TS_FILE.ts
    )

done