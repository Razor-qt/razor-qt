#!/bin/sh

export PROJECT=razor-power
export OPTS="-noobsolete"
export TARGET="-recursive .."

../../scripts/translate-one.sh "$@"
