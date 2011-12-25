#!/bin/sh

export PROJECT=analogclock
#export OPTS="-noobsolete"
export TARGET="../*.h ../*.cpp ../*.ui"

../../../../scripts/translate-one.sh "$@"
