#!/bin/sh

export PROJECT="desktopswitch"
export OPTS="-noobsolete"
export TARGET="-recursive .."

../../../scripts/translate-one.sh "$@"
