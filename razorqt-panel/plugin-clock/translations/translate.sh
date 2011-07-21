#!/bin/bash

PROJECT=clock

#OPTS="-noobsolete" 

if [ "$1" = "" ]; then
    TS_FILE=razorpanel_${PROJECT}_`echo $LANG | awk -F"." '{print($1)}'`.ts
else
    TS_FILE=$1
fi



lupdate ${OPTS} ../*.h ../*.cpp ../*.ui -ts ${TS_FILE}  && \
linguist ${TS_FILE}

