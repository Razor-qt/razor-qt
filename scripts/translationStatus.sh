#!/bin/bash

function help {
  echo "Usage: translationStatus.sh [otions] <path-to-source>"
  echo
  echo "Options"
  echo "  -a|--allfiles         Show statistic for all files files"
  echo "  -l|--lang <LANG>      Show statistic for specify language, like ru_RU"
  echo "                        By default used your curent locale."
  echo "                        If LANG is 'all' show statistic for all languages"
}


TMP="/tmp"
TRANSLATE_ONE='./translate-one.sh'

while [ $# -gt 0 ]; do
  case $1 in
    -a|--allfiles)
        ALLFILES=1
        shift
      ;;

    -l|--lang)
        LANGS="$LANGS $2"
        shift
        shift
      ;;

    --)
        shift
        break
      ;;

    *)
        DIR=$1
        shift
        break
      ;;

  esac
done

if [ -z "${DIR}" ]; then
    DIR="../"
fi

if [ -z "${LANGS}" ]; then
  LANGS=`echo $LANG |  sed -e's/\..*//' `
fi

if [ "${LANGS}" = " all" ]; then
  LANGS=`find ${DIR} -type f -name "*.ts" | sed -e's/.*_\(.._..\).ts/\1/' | sort | uniq`
fi

if [ `which lrelease4 2>/dev/null` ]; then
    LRELEASE='lrelease-qt4'
else
    LRELEASE='lrelease'
fi


function printResults
{
    local all=$1
    local finished=$2
    local path=$3

    if [ -n "$ALLFILES" ] || [ "$finished" != "$all" ]; then
        echo -ne $HEADER
        HEADER=''
        printf "  %d\t%d\t%s\n" $all $finished $path
    fi
}

function checkTS
{
    local SCRIPT=$1
    local DIR=`dirname ${SCRIPT}`
    local TS_FILE=${TMP}/traslationStatusw.ts

    #echo "Script: $SCRIPT ................"
    rm -f ${TS_FILE}

    for file in `find ${DIR} -type f -name "*${lang}.ts" 2>/dev/null`; do
        cp $file ${TS_FILE}
    done

    #(cd ${DIR} && TS_FILE="${TS_FILE}" ./translate.sh  2>/dev/null)
    (cd ${DIR} && TS_FILE="${TS_FILE}" SILENT='-silent' ./translate.sh || echo "in ${SCRIPT}")

    local s=`LANG=C ${LRELEASE} "${TS_FILE}" -qm /dev/null`
    #echo $s

    local finished=`echo $s | grep --regexp="[0-9]\+ finished" --only-matching |  awk '{print ($1)}'`
    let "finished += 0"
    let "totalFinished += $finished"

    local unfinished=`echo $s | grep --regexp="[0-9]\+ unfinished" --only-matching |  awk '{print ($1)}'`
    let "unfinished += 0"
    let "totalUnfinished += $unfinished"

    local ignored=`echo $s | grep --regexp="Ignored [0-9]\+" --only-matching |  awk '{print ($2)}'`
    let "ignored += 0"

    let "all=$finished + $unfinished + $ignored"
    let "totalAll += $all"

    printResults $all $finished $DIR
}


function checkDesktop
{
    local DESKTOP=$1
    let "all = 0"
    let "finished = 0"
    `grep "Name"        -li ${DESKTOP} >/dev/null` && let "all += 1 "
    `grep "Comment"     -li ${DESKTOP} >/dev/null` && let "all += 1 "
    `grep "GenericName" -li ${DESKTOP} >/dev/null` && let "all += 1 "


    `grep "Name\[${l}\]"        -li ${DESKTOP} >/dev/null` && let "finished += 1 "
    `grep "Comment\[${l}\]"     -li ${DESKTOP} >/dev/null` && let "finished += 1 "
    `grep "GenericName\[${l}\]" -li ${DESKTOP} >/dev/null` && let "finished += 1 "

    let "totalFinished += $finished"
    let "totalAll += $all"

    printResults $all $finished ${DESKTOP}
}


HEADER="All\tDone\tFile\n"
for lang in ${LANGS}; do
    let "totalFinished = 0"
    let "totalUnfinished = 0"
    let "totalAll = 0"

    echo "${lang} .............................................."

    for script in `find ${DIR} -type f -name "translate.sh" 2>/dev/null`; do
        checkTS "$script"
    done

    l=`echo $lang | sed -e's/_.*//'`
    for file in `find ${DIR} -type f -name "*.desktop"  2>/dev/null`; do
        checkDesktop $file

    done

    printf "Total: %d from %d\n\n" $totalFinished $totalAll
done;
