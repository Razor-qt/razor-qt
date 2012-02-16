#!/bin/bash

EXCLUDE="sr_BA"
PROJECT_ID=razor-qt

DIR=$1
if [ -z "$DIR" ]; then
    DIR='../'
fi


function scanRecource()
{
  local SRC_FILE=$1
  local RESOURCE_ID=`basename ${SRC_FILE} .ts.src`
  local D=`dirname ${SRC_FILE}`

  tx --quiet set --source -r ${PROJECT_ID}.${RESOURCE_ID} -l en ${SRC_FILE}
  echo "${PROJECT_ID}.${RESOURCE_ID}:"
  echo -n "  "
  for f in `find ${D} -name *.ts`; do
    local lang=`echo $f | sed -e's/.*\(.._..\)\.ts/\1/'`
    #echo "** $lang **************************************************"

    if `echo $EXCLUDE | grep -q $lang`; then
      #echo "   Skip"
      continue
    fi

    tx --quiet set -r ${PROJECT_ID}.${RESOURCE_ID}  -l ${lang} ${f}
    echo -n " ${lang}"
  done
  echo
}



for SRC_FILE in `find ${DIR} -name *.ts.src`; do
    scanRecource ${SRC_FILE}
done


#PWD=`pwd`
#DIR=`basename $PWD`
#IN_ROOT=`ls | grep -o "scripts" > /dev/null; echo $?`

#if [ "$DIR" = "scripts" ]; then
#	ROOT='..'
#elif [ "$IN_ROOT" = "0" ]; then
#	ROOT='.'
#else
#	echo "This script must be run from the project root or from the scripts/ directory."
#	exit 1
#fi

#cd ..
#echo $ROOT
#for i in `find  -name *ts.src`; do
#    echo $i
#    FILE=`echo $i | sed -e's|\.\./||'`
#    ./addTtranslation.sh ${FILE}
#	cd `dirname $i`
#	LANG=en ./translate.sh "$@"
#	FILE=`ls *_en.ts`
#	PROJECT=`basename ${FILE} _en.ts`
#	echo $PROJECT
#	mv ${FILE} "${PROJECT}.ts.src"
#	cd - > /dev/null
#done
