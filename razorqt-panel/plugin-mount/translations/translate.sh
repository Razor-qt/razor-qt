#!/bin/sh

export PROJECT=mount
export OPTS="-noobsolete"
export TARGET="../*.cpp ../*.ui"

../../../scripts/translate-one.sh "$@"
