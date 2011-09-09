#!/bin/bash
#
# usage: ./scripts/batchReplaceStrings.sh oldstring newstring
#
# check sed's separators for special string... now it uses ','
# because of slashes ('/') in URLs
#

echo "Replacing '$1' with '$2'..."

ORIG=$1
REPL=$2

for f in `find . -type f|grep -v ".git/" | grep -v "scripts/"`
do
    echo "$f"
    sed -i "s,${ORIG},${REPL},g" "$f"
    #perl -pi -e 's/'$ORIG'/'$REPL'/' "$f"
done

