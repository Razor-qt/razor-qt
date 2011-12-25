#!/bin/sh

export PROJECT=showdesktop
#export OPTS="-noobsolete"
export TARGET="../*.h ../*.cpp"

../../../scripts/translate-one.sh "$@"
