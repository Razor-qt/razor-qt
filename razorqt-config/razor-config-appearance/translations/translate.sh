#!/bin/sh

export PROJECT=razor-config-appearance
export OPTS="-noobsolete"
export TARGET="-recursive .."

../../../scripts/translate-one.sh "$@"
