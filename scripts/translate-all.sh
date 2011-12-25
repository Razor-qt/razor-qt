#!/bin/sh

PWD=`pwd`
DIR=`basename $PWD`
IN_ROOT=`ls | grep -o "scripts" > /dev/null; echo $?`

if [ "$DIR" = "scripts" ]; then
	ROOT='..'
elif [ "$IN_ROOT" = "0" ]; then
	ROOT='.'
else
	echo "This script must be run from the project root or from the scripts/ directory."
	exit 1
fi

for i in `find $ROOT -name translate.sh`; do
	cd `dirname $i`
	./translate.sh "$@"
	cd - > /dev/null
done
