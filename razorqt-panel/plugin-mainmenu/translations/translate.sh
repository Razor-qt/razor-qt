#!/bin/sh

export PROJECT=mainmenu
#export OPTS="-noobsolete"
export TARGET="../*.h ../*.cpp ../*.ui"

../../../scripts/translate-one.sh "$@"
