#!/bin/sh

export PROJECT=tray
#export OPTS="-noobsolete"
export TARGET="../*.h ../*.cpp"

../../../scripts/translate-one.sh "$@"
