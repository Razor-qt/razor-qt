#!/bin/sh

export PROJECT=taskbar
#export OPTS="-noobsolete"
export TARGET="../*.h ../*.cpp ../*.ui"

../../../scripts/translate-one.sh "$@"
