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
    help
    exit 1
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

HEADER="All\tDone\tFile\n"
for lang in ${LANGS}; do
    let "totalFinished = 0"
    let "totalUnfinished = 0"
    let "totalAll = 0"

    echo "${lang}"

    for file in `find ${DIR} -type f -name "*${lang}.ts" 2>/dev/null`; do
      s=`LANG=C ${LRELEASE} "$file" -qm /dev/null`
      finished=`echo $s | grep --regexp="[0-9]\+ finished" --only-matching |  awk '{print ($1)}'`
      let "finished += 0"
      let "totalFinished += $finished"

      unfinished=`echo $s | grep --regexp="[0-9]\+ unfinished" --only-matching |  awk '{print ($1)}'`
      let "unfinished += 0"
      let "totalUnfinished += $unfinished"

      ignored=`echo $s | grep --regexp="Ignored [0-9]\+" --only-matching |  awk '{print ($2)}'`
      let "ignored += 0"

      let "all=$finished + $unfinished + $ignored"
      let "totalAll += $all"

      if [ -n "$ALLFILES" ] || [ "$finished" != "$all" ]; then
        echo -ne $HEADER
        HEADER=''
#        f=`echo $file | sed s/$DIR//`
        printf "%d\t%d\t%s\n" $all $finished $file
      fi
    done;


    l=`echo $lang | sed -e's/_.*//'`
    for file in `find ${DIR} -type f -name "*.desktop"  2>/dev/null`; do
      let "all = 0"
      let "finished = 0"
      `grep "Name" $file >/dev/null` && let "all += 1 "
      `grep "Comment" $file >/dev/null` && let "all += 1 "
      `grep "GenericName" $file >/dev/null` && let "all += 1 "


      `grep "Name\[${l}\]" $file >/dev/null` && let "finished += 1 "
      `grep "Comment\[${l}\]" $file >/dev/null` && let "finished += 1 "
      `grep "GenericName\[${l}\]" $file >/dev/null` && let "finished += 1 "

      let "totalFinished += $finished"
      let "totalAll += $all"

      if [ -n "$ALLFILES" ] || [ "$finished" != "$all" ]; then
        echo -ne $HEADER
        HEADER=''
#        f=`echo $file | sed s/$DIR//`
        printf "%d\t%d\t%s\n" $all $finished $file
      fi
    done

    echo "Total: $totalFinished from $totalAll"
done;
