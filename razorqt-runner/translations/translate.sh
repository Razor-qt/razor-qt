#!/bin/sh

export PROJECT=razor-runner
export OPTS="-noobsolete"
export TARGET="-recursive .."

../../scripts/translate-one.sh "$@"
