#!/bin/sh

export PROJECT=clock
#export OPTS="-noobsolete"
export TARGET="../*.h ../*.cpp ../*.ui"

../../../scripts/translate-one.sh "$@"
