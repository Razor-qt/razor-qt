#!/bin/bash

DIR=../
RED='\E[0;31m'
YELLOW='\E[0;33m'
NORM='\E[0;37;00m'


let 'invlid=0'
let 'lgplCnt=0'
let 'gplCnt=0'
let 'unknownCnt=0'

for file in `find ${DIR} -type f \( -name '*.h' -o -name '*.cpp' \)  2>/dev/null`; do 

    if [ `grep --files-without-match "BEGIN_COMMON_COPYRIGHT_HEADER" $file` ]; then
	license='Invalid'
        color=$RED
        let 'invlid++'
    elif [ `grep --files-with-matches "GNU General Public License" $file` ]; then
	license='GPL'
	color=$YELLOW
	let 'gplCnt++'

    elif [ `grep --files-with-matches "GNU Lesser General Public" $file` ]; then
	license='LGPL'
	color=''
	let 'lgplCnt++'

    else
	license='Unknown'
        color=$RED
	let 'unknownCnt++'
    fi

    echo -e "${color}${license}\t$file${NORM}"
done
echo
echo "LGPL:    $lgplCnt"
echo "GPL:     $gplCnt"
echo "Unknown: $unknownCnt"