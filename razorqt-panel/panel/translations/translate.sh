#!/bin/sh

export PROJECT=razor-panel
export OPTS="-noobsolete"
export TARGET="-recursive .."

../../../scripts/translate-one.sh "$@"
