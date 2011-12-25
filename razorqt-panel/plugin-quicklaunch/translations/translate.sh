#!/bin/sh

export PROJECT=quicklaunch
export OPTS="-noobsolete"
export TARGET="../*.h ../*.cpp"

../../../scripts/translate-one.sh "$@"
