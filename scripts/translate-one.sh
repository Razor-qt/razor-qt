#!/bin/sh

if [ "$PROJECT" = "" ]; then
	echo "This script is not supposed to be run directly. Run scripts/translate-all.sh or translate-one.sh scripts of individual modules."
	exit 1
fi

EDIT=`echo "$@" | grep -o "\-edit"`

TS_FILE=${PROJECT}_`echo $LANG | awk -F"." '{print($1)}'`.ts

lupdate $OPTS $TARGET -ts $TS_FILE \
&& if [ "$EDIT" != "" ]; then
	echo "Running linguist (GUI)..."
	linguist $TS_FILE
fi

