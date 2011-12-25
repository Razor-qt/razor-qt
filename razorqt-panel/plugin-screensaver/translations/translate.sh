#!/bin/sh

export PROJECT=screensaver
#export OPTS="-noobsolete"
export TARGET="../*.h ../*.cpp"

../../../scripts/translate-one.sh "$@"
