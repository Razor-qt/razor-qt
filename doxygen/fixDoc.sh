#!/bin/sh

DIR=$1
#DIR=~/myProgram/razor/trunk/build/docs;
echo "================================================================="
echo Fixing ${DIR}/classes.html
sed --in-place -e 's/class="el"/class="el indexEl"/g' ${DIR}/classes.html
#cp search.js ${DIR}/search.js
#grep -w --only-matching '<a class="el indexEl"[^<]*</a>' ${DIR}/classes.html | \
#    sed -e's/.*href="\(.*\)".*>\(.*\)<\/a.*/\1|\2/' \
#    > ${DIR}/search.idx
